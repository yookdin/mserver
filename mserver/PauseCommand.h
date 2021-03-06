#pragma once
#include "Command.h"

class PauseCommand : public Command
{
public:
    PauseCommand();
	virtual void interpret(string &line, ifstream &file, ScriptReader &reader);
    
private:
    const regex params_regex;
    
    int get_factor_for_units(string untis);
    string get_time_string();
};

