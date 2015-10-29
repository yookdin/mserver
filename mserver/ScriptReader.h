#pragma once

#include "Command.h"

class ScriptReader
{
public:
	ScriptReader(string filepath);

private:
	static regex command_start_regex;
	static map<string, Command*> commands;

    map<string, Command*>& init_commands();
    void read_file(string filepath);
};

