//
//  RegisterTests.cpp
//  VoxipBBTests
//
//  Created by Yuval Dinari on 7/5/15.
//  Copyright (c) 2015 Yuval Dinari. All rights reserved.
//

#include "RegisterTest.h"


//BB_TEST_F(RegisterTest, debug_mserver)
//{
//    run_mserver_scenario("debug1.scr");
//    ssleep(2);
//}


//======================================================================================================================
// Basic registration test
//======================================================================================================================
BB_TEST_F(RegisterTest, BasicRegister)
{
    run_mserver_scenario("register.scr");
    register_voxip();
}

//======================================================================================================================
// Register and unregister
//======================================================================================================================
BB_TEST_F(RegisterTest, RegisterUnregister)
{
    run_mserver_scenario("reg_unreg.scr");
    register_voxip();
    unregister_voxip();
}


//======================================================================================================================
// On recurring 401 we should notify the client for bad credentials
//======================================================================================================================
BB_TEST_F(RegisterTest, RegisterFail401BadCredentials)
{
    run_mserver_scenario("reg_fail_401_bad_cred.scr");
    register_voxip(5, false);
    string data;
    
    if(!TestsEnv::inst()->wait_for_event(RegState_NotAuthErr, 20, &data))
    {
        throw string("Expected event RegState_NotAuthErr didn't occur");
    }
}


//==================================================================================================
// SIP server will respond to the first register request with 423 message, expecting voxip to
// increase the min-expires header value accordingly.
//==================================================================================================
BB_TEST_F(RegisterTest, Register423)
{
    run_mserver_scenario("reg_423.scr", "min_exp_val=700");
    register_voxip();
    unregister_voxip();
}


//======================================================================================================================
//======================================================================================================================
BB_TEST_F(RegisterTest, RegisterFail400)
{
    test_register_fail("400 Bad Request");
}


//======================================================================================================================
//======================================================================================================================
BB_TEST_F(RegisterTest, RegisterFail403)
{
    test_register_fail("403 Forbidden");
}


//======================================================================================================================
//======================================================================================================================
BB_TEST_F(RegisterTest, RegisterFail404)
{
    test_register_fail("404 Not Found");
}

//======================================================================================================================
//======================================================================================================================
BB_TEST_F(RegisterTest, RegisterFail408)
{
    test_register_fail("408 Request Timeout");
}

//======================================================================================================================
//======================================================================================================================
BB_TEST_F(RegisterTest, RegisterFail500)
{
    test_register_fail("500 Server Internal Error");
}


//======================================================================================================================
//======================================================================================================================
BB_TEST_F(RegisterTest, RegisterFail503)
{
    test_register_fail("503 Service Unavailable");
}


//======================================================================================================================
//======================================================================================================================
BB_TEST_F(RegisterTest, RegisterFail600)
{
    test_register_fail("600 Busy Everywhere");
}


//======================================================================================================================
// No network at beginning of registration, then network is up, with just one IP in the list of server ips.
// voxip should just try the same ip again.
//======================================================================================================================
BB_TEST_F(RegisterTest, NoNetworkRegisterSameIP)
{
    TestsEnv::inst()->resolver->reduce_ips(1); // Leave just the first ip in the list
    LaunchInfo<int> tcp_sniffer_launch_info("count_num_tcp_connection_attempts");
    tcp_sniffer_launch_info.future_res = async(launch::async, &BBTest::count_num_tcp_connection_attempts, this, LOOPBACK_INTERFACE, MSERVER_SIP_PORT, &tcp_sniffer_launch_info);
    
    run_mserver_scenario("no_network_same_ip.scr");
    register_voxip();
    check_mserver_status();
    
    // Tell count_num_tcp_connection_attempts() to stop and return the number of attempts
    tcp_sniffer_launch_info.soft_abort();
    int num_attempts = tcp_sniffer_launch_info.future_res.get(); // Get the return value of count_num_tcp_connection_attempts() from the future object
    
    if(num_attempts < 2)
    {
        throw string("Expected at least two tcp connection attempts but got " + to_string(num_attempts));
    }
    else
    {
        LOG_DEBUG("%d TCP connection attempts on %s:%s", num_attempts, LOOPBACK_INTERFACE, MSERVER_SIP_PORT);
    }
    
    TestsEnv::inst()->resolver->find_ips(); // Repopulate the ips list for following tests
}


//======================================================================================================================
// No network at beginning of registration, then network is up, with at least two IPs in the list of server ips.
// voxip should try the next IP.
//======================================================================================================================
BB_TEST_F(RegisterTest, NoNetworkRegisterNextIP)
{
    if(TestsEnv::inst()->resolver->get_num_ips() < 2) // We need at least 2 ips for this test
    {
        throw string("Can't run NoNetworkRegisterNextIP test without at least two server IPs");
    }

    LaunchInfo<int> tcp_sniffer_launch_info("count_num_tcp_connection_attempts");
    tcp_sniffer_launch_info.future_res = async(launch::async, &BBTest::count_num_tcp_connection_attempts, this, LOOPBACK_INTERFACE, MSERVER_SIP_PORT, &tcp_sniffer_launch_info);

    run_mserver_scenario("no_network_next_ip.scr");
    register_voxip();
    check_mserver_status();

    // Tell count_num_tcp_connection_attempts() to stop and return the number of attempts
    tcp_sniffer_launch_info.soft_abort();
    int num_attempts = tcp_sniffer_launch_info.future_res.get(); // Get the return value of count_num_tcp_connection_attempts() from the future object

    if(num_attempts < 1)
    {
        throw string("Expected at least one tcp connection attempts but got " + to_string(num_attempts));
    }
    else
    {
        LOG_DEBUG("%d TCP connection attempts on %s:%s", num_attempts, LOOPBACK_INTERFACE, MSERVER_SIP_PORT);
    }
}

