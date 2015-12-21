//
//  BBTest.cpp
//  VoxipBBTests
//
//  Created by Yuval Dinari on 6/4/15.
//  Copyright (c) 2015 Yuval Dinari. All rights reserved.
//

#include "BBTest.h"
#include <signal.h>
#include <pcap.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <json/json.h>


#undef __CLASS__
#define __CLASS__ "BBTest"

extern bool sig_received;

#define TCP_ID 6 // The value that indicate tcp in an ip header protocol field


//======================================================================================================================
// Default implementation of TestBody(). This is the method the GTEST mechanism calls for each test. Use BB_TEST_F to
// define InternalTestBody(). In InternalTestBody() throw errors, don't use FAIL().
//======================================================================================================================
void BBTest::TestBody()
{
    string error;
    
    //------------------------------------------------------------------------------------------------------------------
    // Run the test, catch any exception thrown and put the error message in 'error'
    //------------------------------------------------------------------------------------------------------------------
    try
    {
        internal_test_body();
        post_internal_test_body();
    }
    catch(string err)
    {
        error = err;
    }

    //------------------------------------------------------------------------------------------------------------------
    // MServer status needs to be checked no matter whether the test already failed or not, o/w the mserver message will
    // remain in the port and will confuse the following tests.
    // But - sometimes check_mserver_status() is already called from within the test, or the test doesn't use mserver at
    // all, and this will be indicated by dont_check_mserver = true.
    //------------------------------------------------------------------------------------------------------------------
    if(!dont_check_mserver)
    {
        try
        {
            check_mserver_status();
        }
        catch(string err)
        {
            error += "\n" + err; // Append to existing error message (which might be empty)
        }
    }
    
    //------------------------------------------------------------------------------------------------------------------
    // Someone threw an error, fail the test, and notify mserver if necessary
    //------------------------------------------------------------------------------------------------------------------
    if(!error.empty())
    {
        FAIL() << error;
    }
}


//======================================================================================================================
// Called before each test
//======================================================================================================================
void BBTest::SetUp()
{
    TestsEnv::inst()->setup_test(this);
    config();
}


//======================================================================================================================
// Called after each test
//======================================================================================================================
void BBTest::TearDown()
{
    TestsEnv::inst()->teardown_test();
}


//======================================================================================================================
// Default config(), should be overridden if necessary
//======================================================================================================================
void BBTest::config()
{
    VOIPManager::Inst()->SetLog(true);
    VOIPManager::Inst()->SetConfigItem(VXSI_ANSWER_INTERSECTING_CALLS, true);
    VOIPManager::Inst()->SetConfigItem(VXSI_ALLOW_CALL_HANDOFF, true);
    VOIPManager::Inst()->SetConfigItem(VXSI_REGISTER_BASED_READY_FOR_CALL, true);
    VOIPManager::Inst()->SetConfigItem(VXSI_TCP_ENABLED, true);
    VOIPManager::Inst()->SetConfigItem(VXSI_ICE_ENABLED, false);
    //VOIPManager::Inst()->SetConfigItem(VXSI_STUN_SERVER_NAME, "stun.l.google.com:19302");
    VOIPManager::Inst()->SetConfigItem(VXSI_STUN_SERVER_NAME, "");
    VOIPManager::Inst()->SetConfigItem(VXSI_MAX_NUMBER_OF_ACTIVE_CALLS, 2);
    VOIPManager::Inst()->SetQualityServer("UDP", "127.0.0.1", 44444,  true);
    VOIPManager::Inst()->SetDeviceInfo("apple", "6", "iphone", "apple_factory", "ios", "8", "vonage", "100");
    VOIPManager::Inst()->SetAccountData("www.dummy.com", "1800707070", "pass1234", MSERVER_SIP_PORT, "Elvin Jones");
    VxConnectivity::Inst()->Network()->setValue(1);
}

//======================================================================================================================
// Default empty implementation, to be overrided in subclasses
//======================================================================================================================
void BBTest::voxip_ui_callback(EVoXIPUIEvent event, const char* data) {}


//======================================================================================================================
// Send mserver a control message telling it the parameters of thest to run
//======================================================================================================================
void BBTest::run_mserver_scenario(string filename, string params)
{
    // Quote paths in case thay contain spaces.
    string ctrl_msg = "scenario=" + quote(filename) + " test_dir=" + quote(TestsEnv::inst()->get_test_dir()) + " " + params;

    TestsEnv::inst()->send_mserver_ctrl_msg(ctrl_msg);
}


//======================================================================================================================
// Get a status message from mserver, telling if the test passed or failed from its point of view.
//======================================================================================================================
void BBTest::check_mserver_status()
{
    dont_check_mserver = true;
    string status = TestsEnv::inst()->get_mserver_status();
    
    if(status != "success")
    {
        throw string("MServer reported failure: \"" + status + "\"");
    }
}


//======================================================================================================================
// Tell voxip to register. If it fails, create error message, kill external processes and return false, o/w return
// true
//======================================================================================================================
void BBTest::register_voxip(int timeout, bool throwIfFails)
{
    if(TestsEnv::inst()->need_ready_to_register)
    {
        if(!TestsEnv::inst()->wait_for_event(RegState_ReadyToRegister, 11))
        {
            throw string("Ready-to-register event never happened");
        }

        TestsEnv::inst()->need_ready_to_register = false;
    }

    VOIPManager::Inst()->Register();

    if(throwIfFails && !TestsEnv::inst()->wait_for_event(RegState_Registered, timeout))
    {
        throw string("Register failed");
    }
}

//======================================================================================================================
// Tell voxip to unregister. If it fails, create error message, kill external processes and return false, o/w return
// true
//======================================================================================================================
void BBTest::unregister_voxip()
{
    VOIPManager::Inst()->UnRegister();

    if(!TestsEnv::inst()->wait_for_event(RegState_UnRegistered, 10))
    {
        throw string("Unregister failed");
    }
}


//======================================================================================================================
// Return the number of tcp connection attempts for the given interface and port from the time the function was called
// until launch_info->abort will be set to true.
// Return -1 to indicate error.
//
// Note: When the attempt comes from the same host that we're on, listenning on interfaces other than the loopback fails,
// that is, libpcap doesn't capture any packets.
//
// The expected format of the packet received from a loopback interface is:
// - First 4 bytes indicate protocol family, which is expected to be PF_INET (=IP)
// - IP header (with protocol field indicating TCP for the payload)
// - IP payload, which is a TCP packet starting with a TCP header
//======================================================================================================================
int BBTest::count_num_tcp_connection_attempts(string interface_str, string port, LaunchInfo<int> *launch_info)
{
    if(interface_str != "lo0")
    {
        LOG_ERROR("Currently only loopback (lo0) interface supported");
        return -1;
    }

    const char *interface = interface_str.c_str();
    pcap_t *handle;
    char errbuf[PCAP_ERRBUF_SIZE];
    const char *filter_exp = ("port " + port).c_str(); // The filter expression
    bpf_program filter;              // The compiled filter expression
    bpf_u_int32 net;                 // The IP of our sniffing device
    bpf_u_int32 mask;                // The netmask of our sniffing device
    pcap_pkthdr header;	             // The header that pcap gives us
    header.len = 0;
    const u_char *packet = nullptr;     // The actual packet
    int res = 0;

    // The net and mask associated with the interface
    if (pcap_lookupnet(interface, &net, &mask, errbuf) == -1)
    {
        LOG_ERROR("Can't get netmask for interface %s\n", interface);
        return -1;
    }

    handle = pcap_open_live(interface, BUFSIZ, 0, 1000, errbuf); // Get a handle for the sniffing session

    if(handle == nullptr)
    {
        LOG_ERROR("Couldn't open interface %s: %s\n", interface, errbuf);
        return -1;
    }

    if(pcap_datalink(handle) != 0) // This indicates the format of the received packet on the interface
    {
        LOG_ERROR("Expected datalink to be 0 for loopback interface");
        return -1;
    }

    //--------------------------------------------------------------------------------------------------------
    // Set the filter on what we want to sniff from the interface
    //--------------------------------------------------------------------------------------------------------
    if (pcap_compile(handle, &filter, filter_exp, 0, net) == -1)
    {
        LOG_ERROR("Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return -1;
    }

    if (pcap_setfilter(handle, &filter) == -1)
    {
        LOG_ERROR("Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return -1;
    }

    //--------------------------------------------------------------------------------------------------------
    // Wait for a packet to arrive
    //--------------------------------------------------------------------------------------------------------
    while(launch_info->abort == false)
    {
        packet = pcap_next(handle, &header); // Grab a packet

        if(header.len == 0 || packet == nullptr) // For some reason packet may be nullptr when len != 0
        {
            continue;
        }
        else
        {
            int protocol_family = *((int*)packet); // First 4 bytes describe the protocol family (PF_INET, etc.)

            if(protocol_family != PF_INET)
            {
                continue;
            }

            ip *iph = (ip*)(packet + 4); // Cast into an ip header

            if(iph->ip_p != TCP_ID)
            {
                continue;
            }

            tcphdr *tcph = (tcphdr*)(packet + 4 + iph->ip_hl * 4); // Get offset, and cast into a tcp header

            if((tcph->th_flags & TH_SYN) != 0) // The SYN flag indicates that this is the initiating packet
            {
                res++;
            }
        }
    }

    pcap_close(handle);
    launch_info->abort = false; // Must do this, because the caller which initiated the abort usually wait for this
    return res;
} // count_num_tcp_connection_attempts()

