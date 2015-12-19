#pragma once
#include "Command.h"

class RecvCommand :	public Command
{
public:
    RecvCommand(): Command("recv") {}

	virtual void interpret(string &line, ifstream &file, ScriptReader &reader);

private:
    string message_kind; // Either a METHOD name of status code
    bool optional;
    int timeout; // In seconds
    int call_number;

    void process_args(string& line, ScriptReader &reader);
};

