#pragma once
#include "Command.h"

class ScenarioCommand :	public Command
{
public:
    ScenarioCommand(){}

    virtual void interpret(string &line, ifstream &file, ScriptReader &reader);
    virtual string get_start_regex_str();

private:
    string scenario_file;
    map<string, string> args;
    
    void process_args(string& line);
};

