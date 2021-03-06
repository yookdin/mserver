//
//  ExpectCommand.cpp
//  mserver
//
//  Created by Yuval Dinari on 11/19/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "SipParser.hpp"
#include "ExpectCommand.hpp"
#include "ScriptReader.h"
#include "ExpressionEvaluator.hpp"


//==========================================================================================================
// Parse and evaluate the given expression. If it evaluates to false throw an error. Multiple expression are
// allowed, each line should contain a single expression
//==========================================================================================================
void ExpectCommand::interpret(string &line, ifstream &file, ScriptReader &reader)
{
    trim(line);
    
    if(!ExpressionEvaluator::inst()->eval_bool(line, reader))
    {
        throw string("Expected condition: (" + line + ") evaluated to false!");
    }
}

