#pragma once
#include "Command.h"

class SendCommand :	public Command
{
public:
	SendCommand();
	~SendCommand();
	virtual void interpret(string &line, ifstream &file);

private:
};

