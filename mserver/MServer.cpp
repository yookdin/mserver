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
    vars[TRANSPORT] = "TCP"; // Currently supporting only TCP

    // Not sure these actually need real values
    vars[CLIENT_IP] = "10.0.0.116";
    vars[CLIENT_PORT] = "5060";
    vars[SIP_IP_TYPE] = "4";
    vars[MEDIA_IP_TYPE] = "4";
    vars[MEDIA_IP] = "127.0.0.1";
    vars[MEDIA_PORT] = "2000";
}

//==========================================================================================================
//==========================================================================================================
void MServer::run(int argc, char * argv[])
{
    try
    {
        process_args(argc, argv);
        connection.setup(vars[SERVER_IP], stoi(vars[SERVER_PORT]));
        map<string, string> dummy; // TODO: remove this when test runs will not be done from here
        ScriptReader reader(get_value("scenario"), dummy);
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
    options.emplace(SCENARIO_DIR, Option(false, true));
    options.emplace("call_id_min", Option(false, false));
    options.emplace("call_id_max", Option(false, false));
    options.emplace("var", ParamValOption()); // -var name=value

    OptionParser parser(argc, argv, options); // Parse command line option and put values in the map
    
    //------------------------------------------------------------------------------------------------------
    // Put collected values in the vars map. Why not just put it in fields? because script reader will need
    // these values and it will query for them by their variable names.
    //------------------------------------------------------------------------------------------------------
    for(auto pair: options)
    {
        if(pair.first == "call_id_min" || pair.first == "call_id_max" || pair.first == "var")
        {
            continue;
        }
        
        vars[pair.first] = pair.second.get_value();
    }
    
    // -scenario_dir option should be given only in developing phase because xcode puts exe in weird places.
    // in "production", the exe will be in a default location relative to voxip root, and will resolve the
    // scenario dir location automatically.
    if(vars[SCENARIO_DIR].empty())
    {
        set_scenario_dir(argv[0]);
    }
    
    if(options.at("call_id_min").was_found())
    {
        call_id_kind = MIN;
    }
    else if(options.at("call_id_max").was_found())
    {
        call_id_kind = MAX;
    }
    
    //------------------------------------------------------------------------------------------------------
    // Check validity of given parameters
    //------------------------------------------------------------------------------------------------------
    if(!ifstream(get_value(TEST_DIR)))
    {
        error("Dir " + get_value(TEST_DIR) + " doesn't exist");
    }

    if(!ifstream(get_value(SCENARIO_DIR)))
    {
        error("Dir " + get_value(SCENARIO_DIR) + " doesn't exist");
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

//==========================================================================================================
//==========================================================================================================
void MServer::print_vars()
{
    for(auto& pair: vars)
    {
        cout << pair.first << " = " << pair.second << endl;
    }
}


//==========================================================================================================
// Find scenario dir automatically.
// Note: this will not work if the executable was found through a PATH env variable search.
//==========================================================================================================
void MServer::set_scenario_dir(char *argv_0)
{
    string path = argv_0;
    
    if(path[0] != '/') // Not full path
    {
        char cwd[200];
        getcwd(cwd, 200); // The directory from which the program was invoked
        path = cwd + ("/" + path); // path is now absolute
    }
    
    // Clean path:
    // 1. replace "/./" with "/"
    // 2. remove "dir/../" recursively
    path = regex_replace(path, regex("/\\./"), "/");
    
    while(path.find("..") != string::npos) // Remove "dir/../" occurences
    {
        path = regex_replace(path, regex("[^/]+/../"), "");
    }
    
    size_t pos = path.find("build/Debug");

    if(pos != string::npos)
    {
        vars[SCENARIO_DIR] = path.substr(0, pos) + "scenarios";
    }
    else
    {
        throw string("Failed resolving the scenarios dir. path = " + path);
    }
}
























