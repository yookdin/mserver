#pragma once
#include "Command.h"

class SendCommand :	public Command
{
public:
    SendCommand();

	virtual void interpret(string &line, ifstream &file, ScriptReader &reader);
    virtual string get_start_regex_str();

private:
    const regex end_regex;
    int call_number;
    
    void process_args(string& line);
    void replcae_vars(ifstream &file, vector<string>& msg_lines, ScriptReader &reader);
};

