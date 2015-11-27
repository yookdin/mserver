#pragma once
#include "Command.h"

class PauseCommand : public Command
{
public:
    PauseCommand(ScriptReader &_reader): Command(_reader) {}
	virtual void interpret(string &line, ifstream &file);
    
private:
    static const regex params_regex;
    
    int get_factor_for_units(string untis);
};

