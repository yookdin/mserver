//
//  SetCommand.cpp
//  mserver
//
//  Created by Yuval Dinari on 12/8/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "common.h"
#include "SetCommand.hpp"
#include "ScriptReader.h"


//==========================================================================================================
// If 'default' is specified, it means this is the default value for this variable, and will be set only
// if that variable isn't set yet.
//==========================================================================================================
SetCommand::SetCommand():
    Command("set"),
    value_str("-?\\w+|" + string_regex_str + "|" +  brackets_var_regex_str), // 9 submatches
    ternarry_op_str("(" + brackets_var_regex_str + ") *\\? *(" + value_str + ") *: *(" + value_str + ")"),
    var_set_regex("(default +)?(\\w+) *= *((" + ternarry_op_str + ")|(" + value_str + "))")
{}


//==========================================================================================================
// Submatches of var_set_regex:
// 1 : default
// 2 : var
// 3 : value|ternary. If ternary is empty, then this is the value
// 4 : ternary
// 5 : condition
// 13: ternary-value-1
// 23: ternary-value-2
//==========================================================================================================
void SetCommand::interpret(string &line, ifstream &file, ScriptReader &reader)
{
    trim(line); // Remove leading and trailing spaces, and comments (#...)
    smatch match;
    
    if(!regex_match(line, match, var_set_regex))
    {
        throw string("Wrong format of line in set command: " + line);
    }

    // If 'default' is specified don't overwrite, o/w do
    bool overwrite = (match[1].length() == 0);
    string var = match[2]; // Variable name
    string value;
    
    if(match[4].length() == 0)   // "var = value"
    {
        value = match[3];
    }
     else                       // "var = [cond] ? val1 : val2"
    {
        bool condition;
        string condition_str = match[5];
        reader.replace_vars(condition_str);
        
        if(condition_str == "true")
        {
            condition = true;
        }
        else if(condition_str == "false")
        {
            condition = false;
        }
        else
        {
            throw string("Condition of set command must evalutate to true or false");
        }
        
        if(condition)
        {
            value = match[13];
        }
        else
        {
            value = match[23];
        }
    }
    
    value = unquote(value); // Remove quotes if they exist
    
    if(regex_match(value, regex(brackets_var_regex_str))) // If value is [var-name], get its literal value
    {
        reader.replace_vars(value);
    }
    
    reader.set_value(var, value, overwrite);
}

