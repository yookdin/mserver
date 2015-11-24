#pragma once
#include "Command.h"

class ScenarioCommand :	public Command
{
public:
    ScenarioCommand(ScriptReader &_reader): Command(_reader) {}

    virtual void interpret(string &line, ifstream &file);

private:
    string scenario_file;
    map<string, string> args;
    
    void process_args(string& line);
};

