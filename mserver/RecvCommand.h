#pragma once
#include "Command.h"

class RecvCommand :	public Command
{
public:
    RecvCommand(ScriptReader &_reader): Command(_reader) {}

	virtual void interpret(string &line, ifstream &file);
    
private:
    static const regex end_regex;

    void process_args(string& line, string& message_kind, bool& optional, int& timeout, int& call_number);
};

