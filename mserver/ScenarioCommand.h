#pragma once
#include "Command.h"

class ScenarioCommand :	public Command
{
public:
    ScenarioCommand(ScriptReader &_reader): Command(_reader) {}

    virtual void interpret(string &line, ifstream &file);

private:
    static const regex param_regex;
};

