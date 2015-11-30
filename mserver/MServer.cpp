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
MServer::MServer()
{
    vars[PID] = to_string(getpid());
    vars[TRANSPORT] = "TCP"; // Currently supporting only TCP

    // Not sure these actually need real values
    vars[CLIENT_IP] = "10.0.0.116";
    vars[CLIENT_PORT] = "5060";
    vars[SIP_IP_TYPE] = "4";
    vars[CLIENT_IP_TYPE] = "4";
    vars[MEDIA_IP_TYPE] = "4";
    vars[MEDIA_IP] = "127.0.0.1";
    vars[MEDIA_PORT] = "2000";
}

//==========================================================================================================
// Run multiple tests. Control messagaes are sent to tell mserver which scenario to run. After the run a
// message is sent back to indicate success or failure.
// Special message "bye" tells mserver to quit.
//==========================================================================================================
void MServer::run(int argc, char * argv[])
{
    try
    {
        process_args(argc, argv);
        sip_connection = new SipConnection(vars[SERVER_IP], stoi(vars[SERVER_PORT]));
        
        if(! get_value(SCENARIO).empty())
        {
            single_run();
            return;
        }
        
        ctrl_connection = new ControlConnection(vars[SERVER_IP], stoi(vars[CONTROL_PORT]));
        
        while(true)
        {
            string ctrl_msg = ctrl_connection->get_message();
            
            if(ctrl_msg == "bye")
            {
                break;
            }
            
        	map<string, string> script_vars;
            process_control_message(ctrl_msg, script_vars);
            
            try
            {
                ScriptReader reader(get_value(SCENARIO), script_vars);
            }
            catch(string err) // Error caught here is a test error: report it and continue
            {
                cout << endl << err << endl;
                ctrl_connection->send_message(err);
                continue;
            }
            
            ctrl_connection->send_message("success"); // Test succeeded as far as mserver is concerned
        }
    }
    catch (string err) // Error caught here is fatal, and mserver can't run anymore
    {
        error(err);
    }
}


//==========================================================================================================
// TMP. remove this later
//==========================================================================================================
void MServer::single_run()
{
    try
    {
        ScriptReader reader(get_value(SCENARIO), {});
    }
    catch(string err)
    {
        error(err);
    }
}


//==========================================================================================================
//==========================================================================================================
void MServer::error(string msg)
{
    cout << endl << msg << endl;
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
    options.emplace("var", ParamValOption()); // -var name=value

    OptionParser parser(argc, argv, options); // Parse command line option and put values in the map
    
    //------------------------------------------------------------------------------------------------------
    // Put collected values in the vars map. Why not just put it in fields? because script reader will need
    // these values and it will query for them by their variable names.
    //------------------------------------------------------------------------------------------------------
    for(auto pair: options)
    {
        if(pair.first != "var")
        {
            vars[pair.first] = pair.second.get_value();
        }
    }
    
    // -scenario_dir option should be given only in developing phase because xcode puts exe in weird places.
    // in "production", the exe will be in a default location relative to voxip root, and will resolve the
    // scenario dir location automatically.
    if(vars[SCENARIO_DIR].empty())
    {
        set_scenario_dir(argv[0]);
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
SipMessage* MServer::get_sip_message(string kind, int timeout)
{
    return sip_connection->get_message(kind, timeout);
}


//==========================================================================================================
//==========================================================================================================
bool MServer::send_sip_message(SipMessage &message)
{
    return sip_connection->send_message(message);
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
// Process a control message, that is sent for specifying what and how to run a scenario for a single test.
// TODO: should enable overriding global vars here?
//==========================================================================================================
void MServer::process_control_message(string& ctrl_msg, map<string, string>& script_vars)
{
    map<string, Option> options;
    
    options.emplace(SCENARIO, Option(true, true));
    options.emplace(DEFAULT_PV_NAME, ParamValOption()); // Will match any var=val and put it as a new option in the map
    
    OptionParser parser(ctrl_msg, options);
    
    // Scenario option is the name of scenario file to run. All other options are passed to the scenario.
    for(auto pair: options)
    {
        if(pair.first ==  SCENARIO)
        {
            vars[SCENARIO] = pair.second.get_value();
        }
        else if(pair.first != DEFAULT_PV_NAME)
        {
            script_vars[pair.first] = pair.second.get_value();
        }
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
























