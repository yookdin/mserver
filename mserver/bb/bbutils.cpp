//
//  utils.cpp
//  VoxipBBTests
//
//  Created by Yuval Dinari on 5/19/15.
//  Copyright (c) 2015 Yuval Dinari. All rights reserved.
//

#include <json/json.h>
#include "common_headers.h"

using namespace std;

// Global boolean indicating a signal was received
extern bool sig_received;


//==================================================================================================
// TODO: perform general closing stuff here, like killing processes
//==================================================================================================
void exit_nicely(BBExitCode code)
{
    exit(code);
}

//==================================================================================================
// Handler for SIGUSR1, setting sig_recieved
//==================================================================================================
void handle_sig(int sig)
{
    LOG_DEBUG("Received signal %d", sig);
    sig_received = true;
}

//======================================================================================================================
// Sleep for the specified amount of miliseconds even if signals interrupted the sleep
//======================================================================================================================
void msleep(int milliseconds)
{
    // The sleep_until() function receives an absolute time to sleep until, and seems not bothered by signals
    // that interrupt other sleep functions (sleep(), usleep(), nanosleep(), this_thread::sleep_for())
    this_thread::sleep_until(chrono::system_clock::now() + chrono::milliseconds(milliseconds));
}

//======================================================================================================================
// Sleep for the specified amount of seconds even if signals interrupted the sleep
//======================================================================================================================
void ssleep(int seconds)
{
    this_thread::sleep_until(chrono::system_clock::now() + chrono::seconds(seconds));
}

//======================================================================================================================
//======================================================================================================================
void check_json_value(string &data, string field_name, string expected_value, string event_name)
{
    Json::Reader reader;
    Json::Value root;
    
    if(reader.parse(data, root))
    {
        if(root[field_name].asString() != expected_value)
        {
            string err = "Expected pair " + field_name + ":" + expected_value + " in event " + event_name +
            " data but received " + field_name + ":" + root[field_name].asString();
            throw err ;
        }
    }
    else
    {
        throw "Error reading json in data for event " + event_name;
    }
}


//======================================================================================================================
//======================================================================================================================
string get_json_value(const char *data, string field_name)
{
    Json::Reader reader;
    Json::Value root;
    
    if(!reader.parse(data, root) || !root.isObject() || !root.isMember(field_name))
    {
        return ""; // Caller is responsible to issue error if it expected a value
    }
    
    return root[field_name].asString();
}

//======================================================================================================================
//======================================================================================================================
string get_json_value(string &data, string field_name)
{
    return get_json_value(data.c_str(), field_name);
}

//======================================================================================================================
//======================================================================================================================
string get_json_value(const char *data, string field_name, string subfield_name)
{
    Json::Reader reader;
    Json::Value root;
    
    if(!reader.parse(data, root) || !root.isObject() || !root.isMember(field_name))
    {
        return "";
    }
    
    return root[field_name][subfield_name].asString();
}

//======================================================================================================================
//======================================================================================================================
string get_json_value(string &data, string field_name, string subfield_name)
{
    return get_json_value(data.c_str(), field_name, subfield_name);
}


//======================================================================================================================
//======================================================================================================================
string quote(string str)
{
    return "\"" + str + "\"";
}

//======================================================================================================================
// Wait for <condition> to become true, but not more than <timeout> seconds. Return the value of <condition>.
//======================================================================================================================
bool wait_true(bool &condition, int timeout)
{
    if(timeout < 0)
    {
        return wait_condition(condition, true);
    }
    else
    {
        return wait_condition(condition, true, timeout);
    }
}

//======================================================================================================================
// Wait for <condition> to become false.
//======================================================================================================================
bool wait_false(bool &condition, int timeout)
{
    if(timeout < 0)
    {
        return wait_condition(condition, false);
    }
    else
    {
        return wait_condition(condition, false, timeout);
    }
}


//======================================================================================================================
// Return the file name part of a path
//======================================================================================================================
string basename(string& path)
{
    // This works even if no / in path, becaue string::npos + 1 = 0
    return path.substr(path.find_last_of('/') + 1);
}

