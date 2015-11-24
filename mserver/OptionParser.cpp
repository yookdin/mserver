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


//==========================================================================================================
// A special option that allows parameters to be passed without providing the option name! For example:
// "var1=val1 var2=val2 ..."
// This enables to pass arbitary number of pairs like these, without preceding them with an option like:
// "-var var1=val1 -var var1=val1..."
// Or worse, in commands parameter string:
// "var=var1=val1 var=var2=val2..."
// You can still use this option with an actual option name by specifying a name other than "*"
//==========================================================================================================
ParamValOption::ParamValOption(): Option(false, true, true)
{
    param_val_opt = true;
}


/***********************************************************************************************************
 *                                                                                                         *
 *                                  CLASS OptionParser                                                     *
 *                                                                                                         *
 ***********************************************************************************************************/

//==========================================================================================================
// A regex for opt[=val] appearing in a script file. May be opt = "val with spaces", and may be just opt.
// This is used when searching for such pairs within a line.
//==========================================================================================================
const regex OptionParser::eq_pair_regex("(\\w+)( *= *(\\w+|\"([^\"]+)\"))?");


//==========================================================================================================
// This is used on a string that was already extracted from its context, which can either be the command
// line or a file, and we parse the var=val pair separately.
//==========================================================================================================
const regex OptionParser::naked_eq_pair_regex("(\\w+)=(.*)");

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
        
        set_value(opt_name, opt_val, false);
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
        throw string("Option " + opt_name + " doesn't exist");
    }
    
    if(!opt->is_param_val_opt()) // Normal option
    {
        return opt->set_value(val);
    }
    
    // Option is a param-val option. If its name is the same as the given name, it means that the name
    // appeared in the input, and the value (var=val) was NOT parsed - parse it.
    // If the names are different, it means that there was no option provided, the var=val pair was already parsed
    // and it is now in opt_name and val
    if(opt->name == opt_name)
    {
        parse_eq_pair(val, opt_name, val, from_cmd_line);
    }
    
    // Add an option for the given name, and set its value with the given value
    options.emplace(opt_name, Option(false, !val.empty()));
    options.at(opt_name).name = opt_name;
    return options.at(opt_name).set_value(val);
}


//==========================================================================================================
// Returned an option for the given name. If it doesn't exist and a default param-value option exists,
// return that one.
//==========================================================================================================
Option* OptionParser::get_opt(string& opt_name)
{
    if(options.count(opt_name) != 0)
    {
        return &options.at(opt_name);
    }
    else if(options.count(DEFAULT_PV_NAME) != 0)
    {
        return &options.at(DEFAULT_PV_NAME);
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


//==========================================================================================================
// Parse a pair string var=val into its part. If comming from the command line there are no extra " around
// the value. If coming from a file, there could be, so strip them.
//==========================================================================================================
void OptionParser::parse_eq_pair(string& pair, string& name, string& val, bool from_cmd_line)
{
    smatch match;
    
    if(regex_search(pair, match, naked_eq_pair_regex))
    {
        name = match[1];
        val = match[2];
    }
    else
    {
        throw string("Expected value format to be var=val, but received " + pair);
    }
    
    if(!from_cmd_line)
    {
        if(val[0] == '"') // If starting with ", must end with it too
        {
            val.erase(0,1);
            val.erase(val.length() - 1);
        }
    }
}















