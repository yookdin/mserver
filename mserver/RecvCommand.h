#pragma once
#include "Command.h"

class RecvCommand :	public Command
{
public:
    RecvCommand(){}

	virtual void interpret(string &line, ifstream &file, ScriptReader &reader);
    virtual string get_start_regex_str();

private:
    string message_kind; // Either a METHOD name of status code
    bool optional;
    int timeout; // In seconds
    int call_number;

    void process_args(string& line);
};

