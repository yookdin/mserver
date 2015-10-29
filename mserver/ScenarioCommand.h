#pragma once
#include "Command.h"

class ScenarioCommand :	public Command
{
public:
	ScenarioCommand(void);
	~ScenarioCommand(void);
	virtual void interpret(string &line, ifstream &file);

private:
	regex param_regex;
};

