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


//==========================================================================================================
//==========================================================================================================
MServer MServer::inst;

//==========================================================================================================
//==========================================================================================================

MServer::MServer(): call_id_kind(NONE)
{
    // TMP, to be replaced by getting actual value from OS
    vars["client_ip"] = "10.0.0.116";
    vars["client_port"] = "5060";
    vars["pid"] = "666";
}

//==========================================================================================================
//==========================================================================================================
void MServer::run(int argc, char * argv[])
{
    process_args(argc, argv);
    
    try
    {
        ScriptReader reader(get_value("scenario"));
    } catch (string err)
    {
        error(err);
    }
}

//==========================================================================================================
//==========================================================================================================
MServer::Option::Option(bool _mandatory, bool _need_arg): mandatory(_mandatory), need_arg(_need_arg), found(false){}

//==========================================================================================================
//==========================================================================================================
bool MServer::Option::missing()
{
    return mandatory && !found;
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

    options.emplace("-server_ip", Option(true, true));
    options.emplace("-server_port", Option(true, true));
    options.emplace("-test_dir", Option(true, true));
    options.emplace("-scenario", Option(true, true));
    options.emplace("-call_id_min", Option(false, false));
    options.emplace("-call_id_max", Option(false, false));

    for(int i = 1; i < argc; i++)
    {
        string opt_name = argv[i];
        bool last_arg = (i == argc - 1);
        
        if(options.count(opt_name) == 0)
        {
            error("Option " + opt_name + " doesn't exist");
        }
        
        Option &opt = options.at(opt_name);
        opt.found = true;
        
        if(last_arg && opt.need_arg)
        {
            error("Option " + opt_name + " needs an argument");
        }
        
        if(opt.need_arg)
        {
            opt.val = argv[++i];
        }
    }
    
    typedef pair<string, Option> string_opt_pair;
    string missing_opts;
    
    // Collect missing options
    for_each(options.begin(), options.end(),
             [&missing_opts](string_opt_pair val)
                {
                    if(val.second.missing())
                        missing_opts += val.first + " " ;
                });
    
    if(!missing_opts.empty())
    {
        error("Missing options: " + missing_opts);
    }
    
    //------------------------------------------------------------------------------------------------------
    // Put collected values in the vars map. Why not just put it in fields? because script reader will need
    // these values and it will query for them by their variable names.
    //------------------------------------------------------------------------------------------------------
    for(auto pair: options)
    {
        if(pair.first == "-call_id_min" || pair.first == "-call_id_max")
        {
            continue;
        }
        
        string var_name = pair.first;
        var_name.erase(0, 1);
        vars[var_name] = pair.second.val;
    }
    
    if(options.at("-call_id_min").found)
    {
        call_id_kind = MIN;
    }
    else if(options.at("-call_id_max").found)
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
































