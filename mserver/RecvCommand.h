#pragma once
#include "Command.h"

class RecvCommand :	public Command
{
public:
	RecvCommand(void);
	~RecvCommand(void);
	virtual void interpret(string &line, ifstream &file);
};

