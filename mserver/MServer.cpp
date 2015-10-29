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
MServer::MServer(int argc, char * argv[]): call_id_kind(NONE)
{
    process_args(argc, argv);
    
    try
    {
        ScriptReader reader(scenario_file);
    } catch (string err)
    {
        error(err);
    }
}

//==========================================================================================================
//==========================================================================================================
MServer::Option::Option(bool mandatory, bool _need_arg): need_arg(_need_arg), found(false){}

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

// Optional options:
// -call_id_<min|max>   generate either minimal or maximal call id for a generated call
//==========================================================================================================
void MServer::process_args(int argc, char * argv[])
{
    map<string, Option> options;

    options.emplace("-ip", Option(true, true));
    options.emplace("-port", Option(true, true));
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
        
        Option opt = options.at(opt_name);
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
                        missing_opts += val.first;
                });
    
    if(!missing_opts.empty())
    {
        error("Missing options: " + missing_opts);
    }
    
    ip = options.at("-ip").val;
    port = stoi(options.at("-port").val);
    test_dir = options.at("-test_dir").val;
    scenario_file = options.at("-scenario").val;
    
    if(!ifstream(test_dir))
    {
        error("Dir " + test_dir + " doesn't exist");
    }

    if(!ifstream(scenario_file))
    {
        error("File " + scenario_file + " doesn't exist");
    }
    
    if(options.at("-call_id_min").found)
    {
        call_id_kind = MIN;
    }
    else if(options.at("-call_id_max").found)
    {
        call_id_kind = MAX;
    }
}