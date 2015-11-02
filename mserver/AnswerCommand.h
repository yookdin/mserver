#pragma once
#include "Command.h"

class AnswerCommand : public Command
{
public:
    AnswerCommand(ScriptReader &_reader): Command(_reader) {}
    virtual void interpret(string &line, ifstream &file);
};

