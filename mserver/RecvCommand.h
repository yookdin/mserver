#pragma once
#include "Command.h"

class RecvCommand :	public Command
{
public:
    RecvCommand(ScriptReader &_reader): Command(_reader) {}

	virtual void interpret(string &line, ifstream &file);
};

