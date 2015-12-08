//
//  SetCommand.cpp
//  mserver
//
//  Created by Yuval Dinari on 12/8/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "common_headers.h"
#include "SetCommand.hpp"
#include "ScriptReader.h"


//==========================================================================================================
// If 'default' is specified, it means this is the default value for this variable, and will be set only
// if that variable isn't set yet.
//==========================================================================================================
SetCommand::SetCommand(): end_regex("</set>"), var_set_regex("(default +)?(\\w+) *= *(\\w+|" + string_regex_str + ")")
{}


//==========================================================================================================
//==========================================================================================================
void SetCommand::interpret(string &line, ifstream &file, ScriptReader &reader)
{
    for(string line; getline(file, line);)
    {
        if(regex_search(line, end_regex))
        {
            break;
        }
        
        trim(line); // Remove leading and trailing spaces, and // comments
        smatch match;
        
        if(!regex_match(line, match, var_set_regex))
        {
            throw string("Wrong format of line in set command: " + line);
        }
        
        // If 'default' is specified don't overwrite, o/w do
        bool overwrite = (match[1].length() == 0);
        string var = match[2];
        
        // match[4] is the content of a string if specified; if not string then value is match[3]
        string value = (match[4].length() != 0 ? match[4] : match[3]);
        reader.set_value(var, value, overwrite);
    }
}


//==========================================================================================================
//==========================================================================================================
string SetCommand::get_start_regex_str()
{
    return "<(set)>";
}
