//
//  TestsEnv.h
//  VoxipBBTests
//
//  Created by Yuval Dinari on 5/28/15.
//  Copyright (c) 2015 Yuval Dinari. All rights reserved.
//

#ifndef __VoxipBBTests__TestsEnv__
#define __VoxipBBTests__TestsEnv__

#include "common_headers.h"
#include "defines.h"
#include "ControlConnection.hpp"

using namespace std;

class BBTest;

//==================================================================================================
//==================================================================================================
class TestsEnv
{
public:
    static TestsEnv* inst();
    ~TestsEnv();
    void init(int argc, char **argv);
    bool wait_for_event(EVoXIPUIEvent event, int timeout);
    bool wait_for_event(EVoXIPUIEvent event, int timeout, bool erase_old);
    bool wait_for_event(EVoXIPUIEvent event, int timeout, string *pdata);
    bool wait_for_event(EVoXIPUIEvent event, int timeout, bool erase_old, string *pdata);
    void clear_events();
    void setup_test(BBTest *cur_test);
    void teardown_test();
    void set_metric(string field, string value);
    
    void send_mserver_ctrl_msg(string msg);
    string get_mserver_status();
    
    string get_test_dir();
    string get_run_dir();

    // Need to wait for event ready-to-register only the first time registering after voxip init
    bool need_ready_to_register = true;

    voxip_shared_ptr<BBSimulatedDNSResolver> resolver;

private:
    static TestsEnv *the_instance;
    TestsEnv();

    BBTest *cur_test;
    ControlConnection ctrl_connection;
    
    //==============================================================================================
    // A class to wrap an event and its data for storage
    //==============================================================================================
    class VoxipEvent {
    public:
        VoxipEvent(EVoXIPUIEvent event, const char *data);
        EVoXIPUIEvent event;
        string data;
    };
    
    map<EVoXIPUIEvent, VoxipEvent*> events; // The events map is used to keep track of which events occurred
    map<string, string> opt_val_map; // Map of command line options and their values
    
    // Each test will run in its own directory to keep log files seperated. This dir will be under the
    // run dir which was given as a parameter to the program.
    string test_dir;
    int test_serial_num = 0;

    // Will get called each time a UI event occurs
    static void static_voxip_ui_callback(int eventid, const char* data);
    void voxip_ui_callback(int eventid, const char* data);

    void launch_mserver();
    void set_log_file(string dir);
    void create_dir(string dir);
    void parse_args(int argc, char **argv);
};

#endif /* defined(__VoxipBBTests__TestsEnv__) */

