//
//  TestsEnv.cpp
//  VoxipBBTests
//
//  Created by Yuval Dinari on 5/28/15.
//  Copyright (c) 2015 Yuval Dinari. All rights reserved.
//

#include "common_headers.h"
#include "MediaCodecMgr.h"
#include "BBTest.h"
#include "OptionParser.hpp"

#undef __CLASS__
#define __CLASS__ "TestsEnv"


TestsEnv *TestsEnv::the_instance = nullptr;

//======================================================================================================================
// Constructor does nothing, but we need it in order to make it private and prevent unwanted instantiation of TestsEnv.
//======================================================================================================================
TestsEnv::TestsEnv(): cur_test(nullptr), ctrl_connection(LOOPBACK_IP, stoi(MSERVER_CONTROL_PORT)) {}


//======================================================================================================================
//======================================================================================================================
TestsEnv::~TestsEnv()
{
    ctrl_connection.send_message("bye");
}


//======================================================================================================================
// Return/create the only instance of this class
//======================================================================================================================
TestsEnv* TestsEnv::inst()
{
    if(the_instance == nullptr)
    {
        the_instance = new TestsEnv();
    }

    return the_instance;
}


//======================================================================================================================
// Environment initialization stuff
//======================================================================================================================
void TestsEnv::init(int argc, char **argv)
{
    try
    {
        parse_args(argc, argv);
        set_log_file(get_run_dir()); // This is a general log; when running tests each test will have its own log

        // The resolver is needed for mserver launch, so it is created here, and this pointer will be used in VoXIPDeviceLayer
        resolver = voxip_make_shared<BBSimulatedDNSResolver>();
        launch_mserver();
    }
    catch(string err)
    {
        cout << "Fatal error: " << err << endl;
        exit(PERIPHERAL_ERR);
    }
}


//======================================================================================================================
//======================================================================================================================
void TestsEnv::parse_args(int argc, char **argv)
{
    map<string, Option> options;
    
    options.emplace(RUN_DIR_OPT, Option(true, true));
    options.emplace(SCENARIO_DIR_OPT, Option(false, true));
    options.emplace(REAL_SERVER_OPT, Option(false, false));
    options.emplace(MSERVER_OPT, Option(true, true));
    
    OptionParser parser(argc, argv, options); // Parse command line option and put values in the map

    bool real_server = options.at(REAL_SERVER_OPT).was_found();
    
    //------------------------------------------------------------------------------------------------------
    // Check needed options given
    //------------------------------------------------------------------------------------------------------
    if(!real_server && !options.at(SCENARIO_DIR_OPT).was_found())
    {
        throw string("Missing scenario dir option");
    }
    
    //------------------------------------------------------------------------------------------------------
    // Put values in opt_val_map
    //------------------------------------------------------------------------------------------------------
    for(auto pair: options)
    {
        opt_val_map[pair.first] = pair.second.get_value();
    }

    
    //------------------------------------------------------------------------------------------------------
    // Check needed files and dirs
    //------------------------------------------------------------------------------------------------------
    for(auto it : opt_val_map)
    {
        if(it.first == RUN_DIR_OPT && !ifstream(it.second))
        {
            create_dir(it.second);
        }
        else if(!real_server && (it.first == SIPP_OPT || it.first == PROXY_OPT || it.first == SCENARIO_DIR_OPT) &&
                !ifstream(it.second))
        {
            throw string("Directory/file \"" + it.second + "\" doesn't exists");
        }
    }
}


//======================================================================================================================
// Called before each test
//======================================================================================================================
void TestsEnv::setup_test(BBTest *cur_test)
{
    try
    {
        this->cur_test = cur_test;
        const testing::TestInfo *test_info = testing::UnitTest::GetInstance()->current_test_info();
        string test_name = string(test_info->test_case_name()) + "." + test_info->name();
        test_dir = test_name + "." + to_string(++test_serial_num);
        create_dir(get_test_dir());
        set_log_file(get_test_dir()); // To <test-dir>/voxip.log
        printf("Running test: %s\n", test_name.c_str());
        
        // Init the simulate device layer
        BBSimulatedDeviceLayer::Inst()->Init();
        
        // Reset ips to initial values, just in case previous test changed them and forgot to reset them
        resolver->reset_ip();
        auto connectivity = voxip_pointer_cast<BBSimulatedConnectivity>(VxConnectivity::Inst());
        connectivity->setCurrentIP(resolver->get_real_ip());
        
        // Init voxip
        VOIPManager::Inst()->SetUICallback(TestsEnv::static_voxip_ui_callback);
        VOIPManager::Inst()->Start();
        VOIPManager::Inst()->InitServices();
    }
    catch(string err)
    {
        cout << "Fatal error: " << err << endl;
        exit(PERIPHERAL_ERR);
    }
}

//======================================================================================================================
// Called after each test
//======================================================================================================================
void TestsEnv::teardown_test()
{
    need_ready_to_register = true;
    cur_test = nullptr;
    
    // Destroy voxip
    VOIPManager::Inst()->DestroyServices();
    ssleep(1);
    VOIPManager::Inst()->Stop();
    
    clear_events();
}


//======================================================================================================================
//======================================================================================================================
void TestsEnv::clear_events()
{
    // Need to delete all the new-ed objects before clearing the map
    for(auto &iter: events)
    {
        delete iter.second;
    }
    
    events.clear();
}



//======================================================================================================================
//======================================================================================================================
TestsEnv::VoxipEvent::VoxipEvent(EVoXIPUIEvent event, const char *data): event(event), data(data) {}


//======================================================================================================================
// Need a static function because voip manager need a regular function for SetUICallBack(). This
// function just calls the TestsEnv member function to handle the event.
//======================================================================================================================
void TestsEnv::static_voxip_ui_callback(int eventid, const char *data)
{
    TestsEnv::inst()->voxip_ui_callback(eventid, data);
}

//======================================================================================================================
// Put the event in the list, for use by wait_for_event()
//======================================================================================================================
void TestsEnv::voxip_ui_callback(int eventid, const char* data)
{
    EVoXIPUIEvent event = (EVoXIPUIEvent)eventid;
    events[event] = new VoxipEvent(event, data);

    if(cur_test != nullptr)
    {
        try
        {
            cur_test->voxip_ui_callback(event, data);
        }
        catch(string err) // Note: this shouldn't happen, if it does it probably means a BB error, not a VOXIP one
        {
            LOG_ERROR("Exception caught in cur_test->voxip_ui_callback(%s): %s", VoXIPUIEvents::GetUICallbackEventString(event).c_str(), err.c_str());
            exit_nicely(UNKNOWN_ERR);
        }
    }
}


//======================================================================================================================
//======================================================================================================================
bool TestsEnv::wait_for_event(EVoXIPUIEvent event, int timeout)
{
    return wait_for_event(event, timeout, false, nullptr);
}

//======================================================================================================================
//======================================================================================================================
bool TestsEnv::wait_for_event(EVoXIPUIEvent event, int timeout, bool erase_old)
{
    return wait_for_event(event, timeout, erase_old, nullptr);
}

//======================================================================================================================
//======================================================================================================================
bool TestsEnv::wait_for_event(EVoXIPUIEvent event, int timeout, string *pdata)
{
    return wait_for_event(event, timeout, false, pdata);
}

//======================================================================================================================
// Wait max <timeout> seconds for the event to appear in the list. This is a little tricky because an old event might be
// there, and sometimes it's ok and sometimes not. So the caller should decide that, and if erase_old is true, erase old
// event before waiting.
//======================================================================================================================
bool TestsEnv::wait_for_event(EVoXIPUIEvent event, int timeout, bool erase_old, string *pdata)
{
    if(erase_old)
    {
        delete events[event];
        events.erase(event);
    }
    
    time_t start_time = time(nullptr);

    while(time(nullptr) < start_time + timeout)
    {
        if(events.count(event) != 0)
        {
            break;
        }
        else
        {
            msleep(500);
        }
    }

    if(events.count(event) == 0)
    {
        LOG_DEBUG("Event %s didn't occur", VoXIPUIEvents::GetUICallbackEventString(event).c_str());
        return false;
    }
    else
    {
        if(pdata != nullptr)
        {
            *pdata = events[event]->data;
        }
        delete events[event];
        events.erase(event);
        return true;
    }
}


//======================================================================================================================
// Redirect stdout and stderr to a log file
//======================================================================================================================
void TestsEnv::set_log_file(string dir)
{
    string log_file_name = VOXIP_LOG;
    log_file_name = dir + "/" + log_file_name;

    // Redirect both stdout and stderr to file
    if(freopen(log_file_name.c_str(), "a", stdout) == nullptr || freopen(log_file_name.c_str(), "a", stderr) == nullptr)
    {
        throw string("freopen Failed [" + to_string(errno) + ":" + strerror(errno) + "]");
    }
}


//======================================================================================================================
// Try to create directory if it doesn't exist
//======================================================================================================================
void TestsEnv::create_dir(string dir)
{
    if(!ifstream(dir))
    {
        // Don't fail the test if directory already exists. This can happen if another instance of bb created it between
        // the check and the call to mkdir()
        if(mkdir(dir.c_str(), 0777) != 0 && errno != EEXIST)
        {
            throw string("Couldn't create run directory: " + dir + ", strerror(): " + strerror(errno));
        }
    }
}

string TestsEnv::get_test_dir()
{
    return opt_val_map[RUN_DIR_OPT] + "/" + test_dir;
}

string TestsEnv::get_run_dir()
{
    return opt_val_map[RUN_DIR_OPT];
}

string TestsEnv::get_sipp_exe()
{
    return opt_val_map[SIPP_OPT];
}

string TestsEnv::get_proxy_exe()
{
    return opt_val_map[PROXY_OPT];
}

string TestsEnv::get_sipp_scenarios_dir()
{
    return opt_val_map[SCENARIO_DIR_OPT];
}

string TestsEnv::get_sipp_log()
{
    return get_test_dir() + "/" + SIPP_LOG;
}

string TestsEnv::get_sipp_err_log()
{
    return get_test_dir() + "/" + SIPP_ERR_LOG;
}

string TestsEnv::get_sipp_msg_log()
{
    return get_test_dir() + "/" + SIPP_MSG_LOG;
}

string TestsEnv::get_sipp_reg_script()
{
    return get_sipp_scenarios_dir() + "/" + SIPP_REG_SCRIPT;
}

string TestsEnv::get_sipp_complete_call_script()
{
    return get_sipp_scenarios_dir() + "/" + SIPP_COMPLETE_CALL_SCRIPT;
}

string TestsEnv::get_sipp_partial_call_script()
{
    return get_sipp_scenarios_dir() + "/" + SIPP_PARTIAL_CALL_SCRIPT;
}

string TestsEnv::get_proxy_log()
{
    return get_test_dir() + "/" + PROXY_LOG;
}


//==================================================================================================
//==================================================================================================
void TestsEnv::send_mserver_ctrl_msg(string msg)
{
    ctrl_connection.send_message(msg);
}


//==================================================================================================
//==================================================================================================
string TestsEnv::get_mserver_status()
{
    return ctrl_connection.get_message();
}


//==================================================================================================
//==================================================================================================
void TestsEnv::launch_mserver()
{
    vector<VxIpAddress> ips = resolver->get_ips();
    string ips_str;
    
    for(auto ip: ips)
    {
        ips_str += ip.to_string() + " ";
    }
    
    const char *const args[] = {
        opt_val_map[MSERVER_OPT].c_str(),
        "-ips",             ips_str.c_str(),
        "-server_port",     MSERVER_SIP_PORT,
        "-run_dir",         opt_val_map[RUN_DIR_OPT].c_str(),
        "-scenarios_dir",   opt_val_map[SCENARIO_DIR_OPT].c_str(),
        "-control_port",    MSERVER_CONTROL_PORT,
        nullptr
    };
    
    string command; // The command as one string, used for messages
    
    for(int i = 0; args[i] != nullptr; i++)
    {
        command += args[i] + string(" ");
    }
    
    LOG_DEBUG("Launching command: \"%s\"\n", command.c_str());

    switch(pid_t pid = fork())
    {
        case -1: // Fork error
        {
            throw string("fork() error");
        }
        case 0:   // Child process
        {
            execv(args[0], (char * const *)args);

            // We reach here only if execv() failed
            LOG_ERROR("execv() failed\n");
            exit(-1); // Exit from the child process
        }
    } // switch
}
