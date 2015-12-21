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
#include "AssignmentEvaluator.hpp"


//==========================================================================================================
// If 'default' is specified, it means this is the default value for this variable, and will be set only
// if that variable isn't set yet.
//==========================================================================================================
void SetCommand::interpret(string &line, ifstream &file, ScriptReader &reader)
{
    trim(line); // Remove leading and trailing spaces, and comments (#...)
    string org_line = line;
    bool overwrite = true;
    smatch match;
    
    if(regex_search(line, match, regex("^ *default +")))
    {
        overwrite = false;
        line = match.suffix();
    }
    
    map<string, string> vars;
    AssignmentEvaluator::inst()->eval(line, vars, reader);
    
    if(vars.size() != 1)
    {
        throw string("Error evaluating set statement: " + org_line);
    }
    
    reader.set_value(vars.begin()->first, vars.begin()->second, overwrite);
}

