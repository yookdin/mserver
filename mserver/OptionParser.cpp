//
//  OptionParser.cpp
//  mserver
//
//  Created by Yuval Dinari on 11/8/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "OptionParser.hpp"


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
// A regex for opt[=val] appearing in a script file. May be opt = "val with spaces", and may be just opt.
//==========================================================================================================
const regex OptionParser::eq_pair_regex("(\\w+)( *= *(\\w+|\"([^\"]+)\"))?");


//==========================================================================================================
// A regex for opt=val from the command line. This is used when it's already known that a value exists.
// Any ", if existed, were already stripped, since this is what happens on the command line.
//==========================================================================================================
const regex OptionParser::cmd_line_eq_pair_regex("(\\w+)=(.*)");

//==========================================================================================================
// Parse options received in a string, of type: opt=val, but only look at the portion of the line before
// end_char. Set the line the everything after the end_char.
// This means that options are not supposed to contain the end_char. If this is not the case need to fix
// this function.
//==========================================================================================================
OptionParser::OptionParser(string& line, char end_char, map<string, Option>& _options): options(_options) {
    long end_pos = line.find_first_of(end_char);

    if(end_pos == string::npos)
    {
        throw string("Expected end char " + string(1, end_char) + " not found");
    }
    
    sregex_iterator iter(line.begin(), line.begin() + end_pos, eq_pair_regex);
    sregex_iterator end;
    set_option_names(options);
    
    for(;iter != end; ++iter)
    {
        string opt_name = (*iter)[1];
        string opt_val = (*iter)[3];

        if(opt_val[0] == '"')
        {
            opt_val = (*iter)[4];
        }
        
        Option& opt = check_option(opt_name);
        opt.set_value(opt_val);
    }

    line = line.substr(end_pos + 1);
    check_missing_options(options);
}
    
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
        Option& opt = check_option(opt_name);
        
        if(i < argc - 1 && argv[i+1][0] != '-') // Next arg can be considered option-value
        {
            opt_val = argv[i+1];
        }
        
        if(opt.set_value(opt_val))
        {
            ++i; // Skip the next argument which is the option value
        }
    }
    
    check_missing_options(options);
}


//==========================================================================================================
//==========================================================================================================
void OptionParser::set_option_names(map<string, Option>& opts)
{
    for(auto& opt: opts)
    {
        opt.second.name = opt.first;
    }
}


//==========================================================================================================
// Check that the option exists and was not found yet
//==========================================================================================================
OptionParser::Option& OptionParser::check_option(string& opt_name)
{
    if(options.count(opt_name) == 0)
    {
        throw string("Option " + opt_name + " doesn't exist");
    }
    
    Option &opt = options.at(opt_name);
    opt.set_found();
    return opt;
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


//==========================================================================================================
//==========================================================================================================
void OptionParser::parse_cmd_line_eq_pair(string& pair, string& name, string& val)
{
    smatch match;
    
    if(regex_search(pair, match, cmd_line_eq_pair_regex))
    {
        name = match[1];
        val = match[2];
    }
}















