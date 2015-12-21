#pragma once

#include "common.h"

class ScriptReader;

class Command
{
public:
    Command(string _name): name(_name) {}
    virtual void interpret(string &line, ifstream &file, ScriptReader &reader) = 0;
    virtual bool is_control_flow_command() { return false; }
    const string name;
};

