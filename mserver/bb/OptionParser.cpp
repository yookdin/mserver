//
//  OptionParser.cpp
//
//  Created by Yuval Dinari on 11/8/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "OptionParser.hpp"
#include "bbutils.h"

/***********************************************************************************************************
 *                                                                                                         *
 *                                  CLASS OptionParser::Option                                             *
 *                                                                                                         *
 ***********************************************************************************************************/

//==========================================================================================================
//==========================================================================================================
Option::Option(bool _mandatory, bool _need_arg, bool _multi, string default_val):
    mandatory(_mandatory), need_arg(_need_arg), multi(_multi), found(false)
{
    // Multi options (that can appear more than once) must have arguments, and no sense in having default
    // value
    if(multi && (!need_arg || !default_val.empty()))
    {
        throw string("Multi option must need an argument and can't have a default one");
    }
    
    if(!default_val.empty() && !need_arg)
    {
        throw string("Can't have an option with a default value if the option doesn't need an argument");
    }
    
    if(!default_val.empty())
    {
        values.push_back(default_val);
    }
}

               
//==========================================================================================================
//==========================================================================================================
void Option::set_found()
{
    if(found && !multi)
    {
        throw string("Option " + name + " already found");
    }
    
    found = true;
}


//==========================================================================================================
// Set the option value if necessary. If necessary and no option issue error. Return true to indicate
// option value set, false o/w.
//==========================================================================================================
bool Option::set_value(string& val)
{
    set_found();
    
    if(need_arg)
    {
        // Multi opt must have an arg each time it appears. Single opt must have an arg if no default value was provided
        if((multi || values.empty()) && val.empty())
        {
            throw string("Option " + name + " needs an argument");
        }
        
        if(!val.empty())
        {
            values.push_back(val);
            return true;
        }
    }
    
    return false;
}


/***********************************************************************************************************
 *                                                                                                         *
 *                                  CLASS OptionParser                                                     *
 *                                                                                                         *
 ***********************************************************************************************************/

//==========================================================================================================
// Parse options received argv (from the command line), of type: -opt val
//==========================================================================================================
OptionParser::OptionParser(int argc, char * argv[], map<string, Option>& _options): options(_options) {
    
    set_option_names(options);

    for(int i = 1; i < argc; i++)
    {
        string opt_name = argv[i];
        opt_name.erase(0, 1); // Remove leading '-'
        string opt_val;
        
        if(i < argc - 1 && argv[i+1][0] != '-') // Next arg can be considered option-value
        {
            opt_val = argv[i+1];
        }

        if(set_value(opt_name, opt_val, true))
        {
            ++i; // Skip the next argument which is the option value
        }
    }
    
    check_missing_options(options);
}


//==========================================================================================================
// Set the option value.
//==========================================================================================================
bool OptionParser::set_value(string& opt_name, string& val, bool from_cmd_line)
{
    Option *opt = get_opt(opt_name);

    if(opt == nullptr)
    {
        throw string("Option \"" + opt_name + "\" doesn't exist");
    }
    
    return opt->set_value(val);
}


//==========================================================================================================
// Returned an option for the given name.
//==========================================================================================================
Option* OptionParser::get_opt(string& opt_name)
{
    if(options.count(opt_name) != 0)
    {
        return &options.at(opt_name);
    }
    
    return nullptr;
}


//==========================================================================================================
//==========================================================================================================
void OptionParser::set_option_names(map<string, Option>& opts)
{
    for(auto& pair: opts)
    {
        pair.second.name = pair.first;
    }
}


//==========================================================================================================
//==========================================================================================================
void OptionParser::check_missing_options(map<string, Option>& options)
{
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
        throw string("Missing options: " + missing_opts);
    }
}














