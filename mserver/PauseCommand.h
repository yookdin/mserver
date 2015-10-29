#pragma once
#include "Command.h"

class PauseCommand : public Command
{
public:
	PauseCommand(void);
	~PauseCommand(void);
	virtual void interpret(string &line, ifstream &file);
};

