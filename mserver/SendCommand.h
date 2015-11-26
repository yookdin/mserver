#pragma once
#include "Command.h"

class SendCommand :	public Command
{
public:
    SendCommand(ScriptReader &_reader): Command(_reader) {}

	virtual void interpret(string &line, ifstream &file);

private:
    static const regex end_regex;
    int call_number = -1;
    
    void process_args(string& line);
    void replcae_vars(ifstream &file, vector<string>& msg_lines);
};

