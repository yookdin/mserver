#pragma once

#include "common_headers.h"

class ScriptReader;

class Command
{
public:
    Command(ScriptReader &_reader): reader(_reader) {}
	virtual void interpret(string &line, ifstream &file) = 0;

protected:
    ScriptReader &reader;

    void trim(string &line);    
    bool replace_vars(string &line, string last_descriptor = "");
    void replace_len(string &line, string& len);
};

