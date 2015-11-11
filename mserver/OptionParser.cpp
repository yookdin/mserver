//
//  OptionParser.cpp
//  mserver
//
//  Created by Yuval Dinari on 11/8/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "OptionParser.hpp"


//==========================================================================================================
//==========================================================================================================
OptionParser::Option::Option(bool _mandatory, bool _need_arg, string default_val):
    mandatory(_mandatory), need_arg(_need_arg), val(default_val), found(false)
{
    if(!default_val.empty() && !need_arg)
    {
        throw string("Can't have an option with a default value if the option doesn't need an argument");
    }
}


//==========================================================================================================
//==========================================================================================================
bool OptionParser::Option::missing() {
    return mandatory && !found;
}


//==========================================================================================================
// Parse options received in a string, of type: opt=val
//==========================================================================================================
OptionParser::OptionParser(string& line, map<string, Option>& _options): options(_options) {
    regex param_re("(\\w+)( *= *(\\w+|\"([^\"]+)\"))?");
    sregex_iterator iter(line.begin(), line.end(), param_re);
    sregex_iterator end;
    
    for(;iter != end; ++iter)
    {
        string opt_name = (*iter)[1];
        string opt_val = (*iter)[3];

        if(opt_val[0] == '"')
        {
            opt_val = (*iter)[4];
        }
        
        Option& opt = check_option(opt_name);
        set_option_value(opt, opt_name, opt_val);
    }
    
    check_missing_options(options);
}
    
//==========================================================================================================
// Parse options received argv (from the command line), of type: -opt val
//==========================================================================================================
OptionParser::OptionParser(int argc, char * argv[], map<string, Option>& _options): options(_options) {
    
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
        
        if(set_option_value(opt, opt_name, opt_val))
        {
            ++i; // Skip the next argument which is the option value
        }
    }
    
    check_missing_options(options);
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
    
    if(opt.found)
    {
        throw string("Option " + opt_name + " already found");
    }
    
    opt.found = true;
    return opt;
}

//==========================================================================================================
// Set the option value if necessary. If necessary and no option issue error. Return true to indicate
// option value set, false o/w.
//==========================================================================================================
bool OptionParser::set_option_value(Option& opt, string& opt_name, string& opt_val)
{
    if(opt.need_arg)
    {
        if(opt_val.empty() && opt.val.empty())
        {
            throw string("Option " + opt_name + " needs an argument");
        }
        
        if(!opt_val.empty())
        {
            opt.val = opt_val;
            return true;
        }
    }
    
    return false;
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

