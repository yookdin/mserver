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
#include "mserver_utils.hpp"


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
    vars[AUDIO_IP_TYPE] = "4";
    vars[AUDIO_IP] = "127.0.0.1";
    vars[AUDIO_PORT] = "20000";
    vars[VIDEO_IP_TYPE] = "4";
    vars[VIDEO_IP] = "127.0.0.1";
    vars[VIDEO_PORT] = "30000";
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
        ctrl_connection = new ControlConnection(vars[SERVER_IP], stoi(vars[CONTROL_PORT]));
        
        while(true)
        {
            set_log_file(get_value(RUN_DIR) + "/mserver.log"); // Set the log file back to the global log file between tests
            reset_ip(); // In case it was change in the previous test. This will also restart listening in case it was stopped
            
            //----------------------------------------------------------------------------------------------
            // Get and process a control message (indicating which scenario to run, etc.)
            //----------------------------------------------------------------------------------------------
            string ctrl_msg = ctrl_connection->get_message();
            
            if(ctrl_msg == "bye")
            {
                break;
            }
            
        	map<string, string> script_vars;
            process_control_message(ctrl_msg, script_vars);
            
            //----------------------------------------------------------------------------------------------
            // Run the scenario
            //----------------------------------------------------------------------------------------------
            try
            {
                ScriptReader reader(get_value(SCENARIO), script_vars);
                
                if(sip_connection->are_pending_messages())
                {
                    throw string("There are pending messages after test finished");
                }
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
        if(!log_file_set)
        {
            // This will be in the directory from which the executable was run. This is to catch any messages that
            // occur before the run dir is known
            set_log_file("mserver.log");
        }
        
        cout << endl << err << endl;
        exit(-1);
    }
}


//==========================================================================================================
// Mandatory options:
// -ip <IP>             ip to use
// -port <PORT>         port to use
// -test_dir <NAME>     test directory (for log file)
// -scenario <NAME>     scenario file to run
//==========================================================================================================
void MServer::process_args(int argc, char * argv[])
{    
    //------------------------------------------------------------------------------------------------------
    // Collect and check options
    //------------------------------------------------------------------------------------------------------
    map<string, Option> options;

    options.emplace(IPS, Option(true, true));
    options.emplace(SERVER_PORT, Option(true, true));
    options.emplace(CONTROL_PORT, Option(true, true));
    options.emplace(SCENARIO_DIR, Option(false, true));
    options.emplace(RUN_DIR, Option(true, true));
    options.emplace("debug", Option(false, false));
    options.emplace("var", ParamValOption()); // -var name=value

    OptionParser parser(argc, argv, options); // Parse command line option and put values in the map
    
    //------------------------------------------------------------------------------------------------------
    // Put collected values in the vars map. Why not just put it in fields? because script reader will need
    // these values and it will query for them by their variable names.
    //------------------------------------------------------------------------------------------------------
    for(auto pair: options)
    {
        if(pair.first == "debug" && pair.second.was_found())
        {
            debug = true;
        }
        else if(pair.first == IPS)
        {
            extract_ips(pair.second.get_value());
            vars[SERVER_IP] = ips[cur_ip_index];
        }
        else
        {
            vars[pair.first] = pair.second.get_value();
        }
    }
    
    // -scenario_dir option should be given only in developing phase because xcode puts exe in weird places.
    // in "production", the exe will be in a default location relative to voxip root, and we'll resolve the
    // scenario dir location automatically.
    if(vars[SCENARIO_DIR].empty())
    {
        set_scenario_dir(argv[0]);
    }
    
    //------------------------------------------------------------------------------------------------------
    // Check validity of given parameters
    //------------------------------------------------------------------------------------------------------
    if(!debug && !ifstream(get_value(RUN_DIR)))
    {
        throw string("Dir " + get_value(RUN_DIR) + " doesn't exist");
    }

    // Once the run dir is known, direct messages there
    set_log_file(get_value(RUN_DIR) + "/mserver.log");
    log_file_set = true;

    if(!ifstream(get_value(SCENARIO_DIR)))
    {
        throw string("Dir " + get_value(SCENARIO_DIR) + " doesn't exist");
    }
}


//==========================================================================================================
// Process a control message, that is sent for specifying what and how to run a scenario for a single test.
// TODO: should enable overriding global vars here?
//==========================================================================================================
void MServer::process_control_message(string& ctrl_msg, map<string, string>& script_vars)
{
    cout << "Received control message: " << ctrl_msg << endl;
    map<string, Option> options;
    
    options.emplace(SCENARIO, Option(true, true));
    options.emplace(TEST_DIR, Option(true, true));
    options.emplace(DEFAULT_PV_NAME, ParamValOption()); // Will match any var=val and put it as a new option in the map
    
    OptionParser parser(ctrl_msg, options);
    
    // Mandatory options are scenario to run and test dir. All others are parameters to the scenario.
    for(auto pair: options)
    {
        if(pair.first == SCENARIO || pair.first == TEST_DIR)
        {
            vars[pair.first] = pair.second.get_value();
        }
        else
        {
            script_vars[pair.first] = pair.second.get_value();
        }
    }
    
    if(!ifstream(get_value(TEST_DIR)))
    {
        throw string("Dir " + get_value(TEST_DIR) + " doesn't exist");
    }
    
    ctrl_connection->send_message("ok"); // Notify sender that the message was received
    set_log_file(get_value(TEST_DIR) + "/mserver.log");
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
SipMessage* MServer::get_sip_message(string kind, bool optional, int timeout)
{
    return sip_connection->get_message(kind, optional, timeout);
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


//======================================================================================================================
// Redirect stdout and stderr to a log file
//======================================================================================================================
void MServer::set_log_file(string filepath)
{
    if(!debug) // If debug print all messages to standard output
    {
        // Redirect both stdout and stderr to file
        if(freopen(filepath.c_str(), "a", stdout) == nullptr || freopen(filepath.c_str(), "a", stderr) == nullptr)
        {
            throw string("freopen Failed [" + to_string(errno) + ":" + strerror(errno) + "]");
        }
    }
}


//==========================================================================================================
//==========================================================================================================
void MServer::extract_ips(string ip_list)
{
    regex re(SipParser::inst().ip_regex_str);
    sregex_iterator iter(ip_list.begin(), ip_list.end(), re);
    sregex_iterator end;
    
    for(;iter != end; ++iter)
    {
        ips.push_back(iter->str());
    }
}


//==========================================================================================================
//==========================================================================================================
void MServer::advance_ip()
{
    if(ips.size() == 1)
    {
        throw string("Can't move to next IP, only one IP in the list!");
    }
    
    cur_ip_index = (cur_ip_index + 1) % ips.size();
    vars[SERVER_IP] = ips[cur_ip_index];
    sip_connection->start(vars[SERVER_IP]);
}


//==========================================================================================================
//==========================================================================================================
void MServer::reset_ip()
{
    vars[SERVER_IP] = ips[cur_ip_index = 0];
    sip_connection->start(vars[SERVER_IP]);
}


//==========================================================================================================
//==========================================================================================================
void MServer::stop_listening()
{
    cout << "Stopping to listen on SIP port" << endl;
    sip_connection->stop();
}


//==========================================================================================================
//==========================================================================================================
void MServer::start_listening()
{
    cout << "Starting to listen on SIP port" << endl;
    sip_connection->start();
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
























