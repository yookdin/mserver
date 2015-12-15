//
//  utils.h
//  VoxipBBTests
//
//  Created by Yuval Dinari on 5/19/15.
//  Copyright (c) 2015 Yuval Dinari. All rights reserved.
//

#ifndef __VoxipBBTests__utils__
#define __VoxipBBTests__utils__

#include <unistd.h>
#include <string>
#include "VoXIPUIEvents.h"
#include "defines.h"

using namespace std;

//======================================================================================================================
// Exit codes for bb tester
//======================================================================================================================
enum BBExitCode
{
    OK,             // Successful
    PERIPHERAL_ERR, // Error of the environment that prevented normal execution
    VOXIP_ERR,      // Real error of voxip
    USAGE_ERR,      // Error in the usage of bb tester
    UNKNOWN_ERR
};

//======================================================================================================================
// Modes for sipp launch
//======================================================================================================================
enum SippMode {
    SERVER_MODE, // sipp run as server
    CLIENT_MODE  // sipp run as client
};

void exit_nicely(BBExitCode);
void handle_sig(int sig);
void msleep(int miliseconds);
void ssleep(int seconds);
void check_json_value(string &data, string field_name, string expected_value, string event_name);

string get_json_value(const char *data, string field_name);
string get_json_value(string &data, string field_name);
string get_json_value(const char *data, string field_name, string subfield_name);
string get_json_value(string &data, string field_name, string subfield_name);

string quote(string str);
string basename(string& path);

bool wait_true(bool &condition, int timeout = -1);
bool wait_false(bool &condition, int timeout = -1);

//======================================================================================================================
//======================================================================================================================
#define KILL_AND_FAIL()\
    kill_processes();\
    FAIL()

//======================================================================================================================
// Wait for <var> to have the value <val>, but no longer then <timeout> seconds
//======================================================================================================================
template<typename T>
bool wait_condition(T &var, T val, int timeout)
{
    time_t start_time = time(nullptr);
    
    while(time(nullptr) < start_time + timeout && var != val)
    {
        msleep(500);
    }
    
    return (var == val);
}

//======================================================================================================================
// Wait for <var> to have the value <val>
//======================================================================================================================
template<typename T>
bool wait_condition(T &var, T val)
{
    while(var != val)
    {
        msleep(500);
    }
    
    return true;
}



#endif /* defined(__VoxipBBTests__utils__) */
