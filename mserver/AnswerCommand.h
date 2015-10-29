#pragma once
#include "Command.h"

class AnswerCommand : public Command
{
public:
	AnswerCommand(void);
	~AnswerCommand(void);
	virtual void interpret(string &line, ifstream &file);
};

