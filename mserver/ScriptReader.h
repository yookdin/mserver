#pragma once

#include "Command.h"

class ScriptReader
{
public:
	ScriptReader(string filepath);
    string get_value(string var);
    
private:
    static regex command_start_regex;
    map<string, Command*> commands;
    map<string, string> vars; // Map of var names and their values

    void read_file(string filepath);
    string gen_branch();
    string gen_call_id();
    string gen_tag();
};

