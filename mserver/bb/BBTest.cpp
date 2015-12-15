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
}


//======================================================================================================================
// Called after each test
//======================================================================================================================
void BBTest::TearDown()
{
    for(VoidLaunchInfo *launch_info : launch_infos)
    {
        delete launch_info;
    }

    TestsEnv::inst()->teardown_test();
}


//======================================================================================================================
// Default config(), should be overridden if necessary
//======================================================================================================================
void BBTest::config(string port)
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
    //VOIPManager::Inst()->SetAccountData("www.dummy.com", "1800707070", "pass1234", port.c_str(), "Elvin Jones");
    VOIPManager::Inst()->SetAccountData("www.dummy.com", "1800707070", "pass1234", MSERVER_SIP_PORT, "Elvin Jones");
    VxConnectivity::Inst()->Network()->setValue(1);
}

//======================================================================================================================
// Default empty implementation, to be overrided in subclasses
//======================================================================================================================
void BBTest::voxip_ui_callback(EVoXIPUIEvent event, const char* data) {}

//======================================================================================================================
// Wait for process to notify it is running for up to <timeout> seconds.
//======================================================================================================================
void BBTest::wait_for_process_to_run(VoidLaunchInfo *launch_info, int timeout)
{
    if(!wait_true(launch_info->is_running, timeout))
    {
        throw string(launch_info->name + " process failed to run in " + to_string(timeout) + " seconds");
    }
}


//======================================================================================================================
//======================================================================================================================
void BBTest::run_mserver_scenario(string filename, string params)
{
    TestsEnv::inst()->run_mserver_scenario(filename, params);
}


//======================================================================================================================
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
// Launch external process using execv(). Wait for the process to terminate unless abort is set. If abort is set stop
// waiting, kill process, and return.
//======================================================================================================================
void BBTest::launch_command(char *const args[], VoidLaunchInfo *launch_info)
{
    string command; // The command as one string, used for messages

    for(int i = 0;; i++)
    {
        command += args[i];

        if(args[i + 1] != nullptr)
        {
            command += " ";
        }
        else
        {
            break;
        }
    }

    if(launch_info != nullptr)
    {
        ssleep(launch_info->delay);
    }

    LOG_DEBUG("Launching command: \"%s\"\n", command.c_str());
    pid_t pid;

    switch(pid = fork())
    {
        case -1: // Fork error
            throw string("fork() error");

        case 0:   // Child process
        {
            execv(args[0], args);
            //
            // We reach here only if execv() failed
            LOG_ERROR("execv() failed with command: \"%s\"\n", command.c_str());
            exit(-1); // Exit from the child process
        }

        default: // Parent process
            int status;

            // Wait for the child process in a non-blocking way, but if abort is set quit and kill it!
            while(launch_info == nullptr || launch_info->abort == false)
            {
                if(launch_info != nullptr && !launch_info->is_running && sig_received == true)
                {
                    launch_info->is_running = true;
                    sig_received = false;
                }

                int res_pid = waitpid(pid, &status, WNOHANG);

                if (res_pid == -1)
                {
                    throw string("waitpid() error");
                }
                else if(res_pid == 0)     // Child hasn't ended yet
                {
                    msleep(500); // Sleep for half a second
                }
                else     // child ended
                {
                    if(!WIFEXITED(status))
                    {
                        throw string("Command \n\"" + command + "\"\n didn't exit properly");
                    }
                    else if(WEXITSTATUS(status) != 0)
                    {
                        throw string("Command \n\"" + command + "\"\nexited with status: " + to_string(WEXITSTATUS(status)));
                    }
                    else
                    {
                        return; // Child ended successfuly
                    }
                }
            } // while

            // We reach here only if abort is true
            string abort_kind = (launch_info->abort_kind == SOFT ? "Soft" : "Hard");
            LOG_DEBUG("%s abort was set for launch_command() of command: \"%s\"\n", abort_kind.c_str(), command.c_str());
            kill_child_process(pid);
            launch_info->abort = false;

            if(launch_info->abort_kind == HARD)
            {
                throw string("Hard abort");
            }
    } // switch
} // launch_command()


//======================================================================================================================
// Try to kill the child process. First try nicely by sending the SIGTERM signal, then unnicely by sending the SIGKILL
// signal.
//======================================================================================================================
void BBTest::kill_child_process(pid_t pid)
{
    int res = 0;

    for(int i = 0; i < 2; i++)
    {
        LOG_DEBUG("Killing process %d...", pid);
        int signal = (i == 0) ? SIGTERM : SIGKILL;
        int status, timeout = 5; // TODO: why does it take so long?
        time_t start_time = time(nullptr);

        while(time(nullptr) < start_time + timeout)
        {
            res = kill(pid, signal);

            if(res != 0) // This should never happen
            {
                LOG_ERROR("kill(%d, %d) failed", pid, signal);
                return;
            }

            // Must call waitpid, o/w child process will become a zombie
            res = waitpid(pid, &status, WNOHANG);

            if (res == -1)
            {
                LOG_ERROR("waitpid() error");
                return;
            }
            else if(res == 0)     // Child hasn't ended yet
            {
                msleep(50); // Sleep for 50 ms
            }
            else     // child ended
            {
                if(res != pid)
                {
                    LOG_ERROR("waitpid(%d) returned %d (after kill()), something is horribly wrong!", pid, res);
                    return;
                }
                else
                {
                    LOG_DEBUG("Process %d is really dead", pid);
                    return;
                }
            }
        } // while loop

        if(res == 0)
        {
            LOG_DEBUG("Failed to kill process %d on attempt %d", pid, i + 1);
        }
    } // for loop
} // kill_child_process()


//======================================================================================================================
// Turn string into array of char* (args) and call launch_command(args)
//======================================================================================================================
void BBTest::launch_command(string command, VoidLaunchInfo *launch_info)
{
    // Split command to parts
    istringstream iss(command);
    vector<string> command_parts {istream_iterator<string>{iss}, istream_iterator<string>{}};
    long size = command_parts.size();
    const char *args[size + 1];

    for(int i = 0; i < size; i++)
    {
        args[i] = command_parts[i].c_str();
    }

    args[size] = nullptr; // args last element must be nullptr (in order to pass to execv())
    launch_command((char *const*)args, launch_info);
}


//======================================================================================================================
// Kill all external processes, via the launch_infos list
//======================================================================================================================
void BBTest::kill_processes(int timeout)
{
    // Stop running processes
    for(VoidLaunchInfo *launch_info : launch_infos)
    {
        if(timeout != -1)
        {
            launch_info->timeout = timeout;
        }

        try
        {
            LOG_DEBUG("Trying to kill %s", launch_info->name.c_str());
            get_future_res(launch_info);
        }
        catch (string err)
        {
            launch_info->successful = false;
            LOG_ERROR("Exception in process %s: %s", launch_info->name.c_str(), err.c_str());
        }
    }
}


//======================================================================================================================
// Get a result from a future object but if more than <timeout> seconds pass, set abort (to make the
// async call terminate) and return -1.
//======================================================================================================================
void BBTest::get_future_res(VoidLaunchInfo *launch_info)
{
    chrono::seconds max_wait(launch_info->timeout);
    future_status future_status = launch_info->future_res.wait_for(max_wait);

    if(future_status != future_status::ready)
    {
        LOG_ERROR("Timed out while waiting for future object for launch of %s", launch_info->name.c_str());
        launch_info->abort = true;
    }

    launch_info->future_res.get();
}


//======================================================================================================================
// Launch proxy process using command string
//======================================================================================================================
void BBTest::launch_proxy(VoidLaunchInfo *launch_info)
{
    string cmd = TestsEnv::inst()->get_proxy_exe() + " " + TestsEnv::inst()->get_proxy_log() + " " +
                 TestsEnv::inst()->resolver->get_cur_ip().to_string();
    launch_command(cmd, launch_info);
}

//======================================================================================================================
// General sipp launch in server mode
//======================================================================================================================
void BBTest::launch_sipp_server(VoidLaunchInfo *launch_info, string script)
{
    string cmd = build_sipp_command(script, SIPP_SERVER_PORT);
    launch_command(cmd, launch_info);
}

//======================================================================================================================
//======================================================================================================================
void BBTest::launch_sipp_server_full(VoidLaunchInfo *launch_info, string script, string params_file, string extra_parmas)
{
    string cmd = build_sipp_command(script, SIPP_SERVER_PORT, params_file, SERVER_MODE, extra_parmas);
    launch_command(cmd, launch_info);
}


//======================================================================================================================
// General sipp launch in client mode
//======================================================================================================================
void BBTest::launch_sipp_client(VoidLaunchInfo *launch_info, string script)
{
    launch_sipp_client(launch_info, script, "", "");
}

//======================================================================================================================
//======================================================================================================================
void BBTest::launch_sipp_client(VoidLaunchInfo *launch_info, string script, string params_file, string extra_params)
{
    string cmd = build_sipp_command(script, PROXY_SIPP_SERVER_PORT, params_file, CLIENT_MODE, extra_params);
    launch_command(cmd);
}


//======================================================================================================================
//======================================================================================================================
void BBTest::launch_sipp_for_registration(VoidLaunchInfo *launch_info)
{
    launch_sipp_server(launch_info, SIPP_REG_SCRIPT);
}


//======================================================================================================================
// Launch sipp with a script that will return an error code to voxip that is supposed to make voxip try the next ip for
// a sipp server (caller is responsible to give the correct script!). Then launch sipp again with the second ip and see
// that normal registration is successful.
//======================================================================================================================
void BBTest::launch_sipp_for_registration_fail(VoidLaunchInfo *launch_info, string error_code)
{
    // Create an actual script file from script template and put it in the test dir
    string script_path;
    create_fail_script(SIPP_REG_FAIL_SCRIPT_TEMPLATE, error_code, script_path);
    launch_sipp_server(launch_info, script_path);
}


//======================================================================================================================
// Copy script template to test dir, then replace error code place holder with actual error code.
//======================================================================================================================
void BBTest::create_fail_script(string script_template, string error_code, string &script_path)
{
    //----------------------------------------------------------------------------------------------
    // Copy template script into test dir
    //----------------------------------------------------------------------------------------------
    string script_template_path = TestsEnv::inst()->get_sipp_scenarios_dir() + "/" + script_template;
    script_path = TestsEnv::inst()->get_test_dir() + "/" + template_to_actual_script_name(script_template);
    string cmd = "/bin/cp " + script_template_path + " " + script_path;
    launch_command(cmd);
    //----------------------------------------------------------------------------------------------
    // Replace error code place holder with actual error in-place using sed command.
    //----------------------------------------------------------------------------------------------
    vector<string> args; // Can't put command in string and split by spaces because replacement string contains spaces
    args.push_back("/usr/bin/sed");
    args.push_back("-i");
    args.push_back("");
    args.push_back("s@\\[error_code\\]@" + error_code + "@g");
    args.push_back(script_path);
    long size = args.size();
    const char *cargs[size + 1];

    for(int i = 0; i < size; i++)
    {
        cargs[i] = args[i].c_str();
    }

    cargs[size] = nullptr; // cargs must end with a nullptr pointer
    launch_command((char * const *)cargs);
}


//======================================================================================================================
// Remove the "_template" in the script name
//======================================================================================================================
string BBTest::template_to_actual_script_name(string script_template)
{
    string t = "_template";
    long pos = script_template.find(t);
    script_template.replace(pos, t.size(), "");
    return script_template;
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


//======================================================================================================================
//======================================================================================================================
string BBTest::build_sipp_command(string script, string port)
{
    return build_sipp_command(script, port, "", SERVER_MODE);
}
string BBTest::build_sipp_command(string script, string port, SippMode sipp_mode)
{
    return build_sipp_command(script, port, "", sipp_mode);
}
string BBTest::build_sipp_command(string script, string port, string params_file)
{
    return build_sipp_command(script, port, params_file, SERVER_MODE);
}

//======================================================================================================================
// Construct a sipp launch command according to parameters and TestsEnv state
//======================================================================================================================
string BBTest::build_sipp_command(string script, string port, string params_file, SippMode sipp_mode, string extra_params)
{
    string test_dir = TestsEnv::inst()->get_test_dir();

    // If script is actually a path containing the test dir, no need to change it. Otherwise it's a name of a script, and
    // preappend it with the scenarios dir path.
    if(script.find(test_dir) == string::npos) // script doesn't contain test dir
    {
        script = TestsEnv::inst()->get_sipp_scenarios_dir() + "/" + script;
    }

    string ip = TestsEnv::inst()->resolver->get_cur_ip().to_string();
    string cmd = TestsEnv::inst()->get_sipp_exe();

    // In client mode the ip and port must appear immediately after the command name with no preceding option
    if(sipp_mode == CLIENT_MODE)
    {
        cmd += " " + ip + ":" + port;
    }
    else // In server mode we could actually add these options anywhere
    {
        cmd += " -i " + ip + " -p " + port;
    }

    string params_file_option;

    // The parameter file is an optional file in the test directory, containing dynamically created data,
    // such as call-id of previous calls, etc.
    if(!params_file.empty())
    {
        params_file_option = " -inf " + test_dir + "/" + params_file + " ";
    }

    cmd += " -sf " + script + " -bg -nostdin -m 1 -t t1 -nd -key UA_type \"Sipp-Test\" -key test_folder " +
           test_dir + params_file_option + " " + extra_params +
           " -trace_msg -message_file " + TestsEnv::inst()->get_sipp_msg_log() + " -message_overwrite false" +
           " -trace_logs -log_file "    + TestsEnv::inst()->get_sipp_log()    + " -log_overwrite false" +
           " -trace_err -error_file "   + TestsEnv::inst()->get_sipp_err_log() + " -error_overwrite false";
    return cmd;
}
