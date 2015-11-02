#pragma once
#include "Command.h"

class PauseCommand : public Command
{
public:
    PauseCommand(ScriptReader &_reader): Command(_reader) {}
	virtual void interpret(string &line, ifstream &file);
};

