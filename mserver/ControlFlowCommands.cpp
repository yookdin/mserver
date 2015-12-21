//
//  ControlFlowCommands.cpp
//  mserver
//
//  Created by Yuval Dinari on 12/21/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "ControlFlowCommands.hpp"
#include "IfStatement.hpp"
#include "ScriptReader.h"

//==========================================================================================================
//==========================================================================================================
void IfCommand::interpret(string &line, ifstream &file, ScriptReader &reader)
{
    reader.if_stack.push(new IfStatement(reader.should_execute(), line, reader));
}


//==========================================================================================================
//==========================================================================================================
void ElseCommand::interpret(string &line, ifstream &file, ScriptReader &reader)
{
    if(reader.if_stack.empty())
    {
        throw string("else with no preceding if");
    }
    
    reader.if_stack.top()->switch_to_else();
}


//==========================================================================================================
//==========================================================================================================
void ElseifCommand::interpret(string &line, ifstream &file, ScriptReader &reader)
{
    if(reader.if_stack.empty())
    {
        throw string("elseif with no preceding if");
    }
    
    reader.if_stack.top()->switch_to_else();
    reader.if_stack.push(new IfStatement(reader.should_execute(), line, reader, true));

}


//==========================================================================================================
//==========================================================================================================
void EndifCommand::interpret(string &line, ifstream &file, ScriptReader &reader)
{
    if(reader.if_stack.empty())
    {
        throw string("endif with no preceding if");
    }

    IfStatement* top;
    
    do
    {
        top = reader.if_stack.top();
        reader.if_stack.pop();
    }
    while(top->implicit);
}

