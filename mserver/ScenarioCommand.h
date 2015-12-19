#pragma once
#include "Command.h"

class ScenarioCommand :	public Command
{
public:
    ScenarioCommand(): Command("run_scenario") {}

    virtual void interpret(string &line, ifstream &file, ScriptReader &reader);

private:
    string scenario_file;
    map<string, string> args;
    
    void process_args(string& line, ScriptReader &reader);
};

