//
//  Call.cpp
//  VoxipBBTests
//
//  Created by Yuval Dinari on 10/8/15.
//  Copyright © 2015 Yuval Dinari. All rights reserved.
//

#include "Call.h"


//======================================================================================================================
// Called when creating object due to calling or alert-incoming events.
//======================================================================================================================
Call::Call(Direction _dir): dir(_dir), connected(false){}

//======================================================================================================================
// Called when creating object due to connected event
//======================================================================================================================
Call::Call(string dir_str): dir(string_to_dir(dir_str)), connected(true){}


//======================================================================================================================
//======================================================================================================================
Call::Direction Call::string_to_dir(string dir_str)
{
    if(dir_str == "in")
    {
        return IN;
    }
    else if(dir_str == "out")
    {
        return OUT;
    }
    else
    {
        throw string("Wrong direction parameter: " + dir_str);
    }
}


//======================================================================================================================
//======================================================================================================================
void Call::toggle_mute()
{
    mutted = !mutted;
}


//======================================================================================================================
//======================================================================================================================
void Call::gsm_hold_on()
{
    gsm_hold = true;
    was_on_hold = true;
}


//======================================================================================================================
//======================================================================================================================
void Call::gsm_hold_off()
{
    gsm_hold = false;
}


//======================================================================================================================
//======================================================================================================================
void Call::local_hold_on()
{
    // If local_hold already true, this indicates that we were taken off remote hold (the event reflect the current state)
    if(local_hold)
    {
        remote_hold = false;
    }

    local_hold = true;
    was_on_hold = true;
}


//======================================================================================================================
//======================================================================================================================
void Call::remote_hold_on()
{
    // If local_hold already true, this indicates that we were taken off remote hold (the event reflect the current state)
    if(remote_hold)
    {
        local_hold = false;
    }
    
    remote_hold = true;
    was_on_hold = true;
}

//======================================================================================================================
//======================================================================================================================
void Call::connect()
{
    if(gsm_hold)
    {
        throw string("Can't get off hold or connect while on GSM hold");
    }
    
    local_hold = remote_hold = false;
    connected = true;
}

//======================================================================================================================
//======================================================================================================================
void Call::end()
{
    connected = false;
    ended = true;
}


//======================================================================================================================
//======================================================================================================================
void Call::check_direction(string dir_str)
{
    if(dir != string_to_dir(dir_str))
    {
        throw string("Wrong direction");
    }
}


//======================================================================================================================
//======================================================================================================================
void Call::check_telemetries()
{
    if(telemetries.empty()) // Some tests finish before the telemetries are sent
    {
        return;
    }
    
    LOG_DEBUG("Performing telemetries check for call %s", sip_id.c_str());
    
    string recv_dir_str = get_json_value(telemetries, "DIRECTION");
    string dir_str = (dir == IN ? "callee" : "caller");
    
    if(dir_str != recv_dir_str)
    {
        throw string("Telemtry error: wrong direction, expected " + dir_str + ", received " + recv_dir_str);
    }
    
    bool recv_was_on_hold = (bool)stoi(get_json_value(telemetries, "WASONHOLD"));
    if(was_on_hold != recv_was_on_hold)
    {
        throw string("Telemtry error: expected WASONHOLD to be " + to_string(was_on_hold) + " but received " + to_string(recv_was_on_hold));
    }
}








































