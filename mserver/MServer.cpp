//
//  MServer.cpp
//  mserver
//
//  Created by Yuval Dinari on 10/27/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "MServer.hpp"
#include "ScriptReader.h"
#include "SipParser.hpp"
#include "OptionParser.hpp"


//==========================================================================================================
//==========================================================================================================
MServer MServer::inst;

//==========================================================================================================
//==========================================================================================================
MServer::MServer(): call_id_kind(NONE)
{
    vars[PID] = to_string(getpid());
    vars[TRANSPORT] = "tcp"; // Currently supporting only TCP

    // Not sure these actually need real values
    vars[CLIENT_IP] = "10.0.0.116";
    vars[CLIENT_PORT] = "5060";
}

//==========================================================================================================
//==========================================================================================================
void MServer::run(int argc, char * argv[])
{
    try
    {
        process_args(argc, argv);
        connection.setup(vars[SERVER_IP], stoi(vars[SERVER_PORT]));
        ScriptReader reader(get_value("scenario"));
    }
    catch (string err)
    {
        error(err);
    }
}

//==========================================================================================================
//==========================================================================================================
void MServer::error(string msg)
{
    cout << msg << endl;
    exit(-1);
}

//==========================================================================================================
// Mandatory options:
// -ip <IP>             ip to use
// -port <PORT>         port to use
// -test_dir <NAME>     test directory (for log file)
// -scenario <NAME>     scenario file to run
//
// Optional options:
// -call_id_<min|max>   generate either minimal or maximal call id for a generated call
//==========================================================================================================
void MServer::process_args(int argc, char * argv[])
{
    //------------------------------------------------------------------------------------------------------
    // Collect and check options
    //------------------------------------------------------------------------------------------------------
    map<string, Option> options;

    options.emplace(SERVER_IP, Option(true, true));
    options.emplace(SERVER_PORT, Option(true, true));
    options.emplace(TEST_DIR, Option(true, true));
    options.emplace(SCENARIO, Option(true, true));
    options.emplace("call_id_min", Option(false, false));
    options.emplace("call_id_max", Option(false, false));

    OptionParser parser(argc, argv, options); // Parse command line option and put values in the map
    
    //------------------------------------------------------------------------------------------------------
    // Put collected values in the vars map. Why not just put it in fields? because script reader will need
    // these values and it will query for them by their variable names.
    //------------------------------------------------------------------------------------------------------
    for(auto pair: options)
    {
        if(pair.first == "call_id_min" || pair.first == "call_id_max")
        {
            continue;
        }
        
        string var_name = pair.first;
        vars[var_name] = pair.second.val;
    }
    
    if(options.at("call_id_min").found)
    {
        call_id_kind = MIN;
    }
    else if(options.at("call_id_max").found)
    {
        call_id_kind = MAX;
    }
    
    //------------------------------------------------------------------------------------------------------
    // Check validity of given parameters
    //------------------------------------------------------------------------------------------------------
    if(!ifstream(get_value("test_dir")))
    {
        error("Dir " + get_value("test_dir") + " doesn't exist");
    }

    if(!ifstream(get_value("scenario")))
    {
        error("File " + get_value("scenario") + " doesn't exist");
    }
}


//==========================================================================================================
//==========================================================================================================
string& MServer::get_value(string var)
{
    if(vars.count(var) == 0)
    {
        throw string("Variable '" + var + "' doesn't exist");
    }
    
    return vars[var];
}


//==========================================================================================================
//==========================================================================================================
SipMessage* MServer::get_message(string kind, int timeout)
{
    return connection.get_message(kind, timeout);
}


//==========================================================================================================
//==========================================================================================================
bool MServer::send_message(SipMessage &message)
{
    return connection.send_message(message);
}

//==========================================================================================================
//==========================================================================================================
MServer::CallIDKind MServer::get_call_id_kind()
{
    return call_id_kind;
}




























