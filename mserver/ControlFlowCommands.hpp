//
//  ControlFlowCommands.hpp
//  mserver
//
//  Created by Yuval Dinari on 12/21/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef ControlFlowCommands_hpp
#define ControlFlowCommands_hpp

#include "Command.h"


//==========================================================================================================
//==========================================================================================================
class ControlFlowCommand: public Command
{
public:
    ControlFlowCommand(string name): Command(name) {}
    virtual bool is_control_flow_command() { return true; }
};


//==========================================================================================================
//==========================================================================================================
class IfCommand: public ControlFlowCommand
{
public:
    IfCommand(): ControlFlowCommand("if") {}
    virtual void interpret(string &line, ifstream &file, ScriptReader &reader);
    
};


//==========================================================================================================
//==========================================================================================================
class ElseCommand: public ControlFlowCommand
{
public:
    ElseCommand(): ControlFlowCommand("else") {}
    virtual void interpret(string &line, ifstream &file, ScriptReader &reader);
};


//==========================================================================================================
//==========================================================================================================
class ElseifCommand: public ControlFlowCommand
{
public:
    ElseifCommand(): ControlFlowCommand("elseif") {}
    virtual void interpret(string &line, ifstream &file, ScriptReader &reader);
};


//==========================================================================================================
//==========================================================================================================
class EndifCommand: public ControlFlowCommand
{
public:
    EndifCommand(): ControlFlowCommand("endif") {}
    virtual void interpret(string &line, ifstream &file, ScriptReader &reader);
};


#endif /* ControlFlowCommands_hpp */
