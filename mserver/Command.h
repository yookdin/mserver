#pragma once

#include "common_headers.h"

class ScriptReader;

class Command
{
public:
	virtual void interpret(string &line, ifstream &file, ScriptReader &reader) = 0;
    virtual string get_start_regex_str() = 0;

protected:
    void trim(string &line);
    void replace_vars(string &line, ScriptReader &reader, int call_number = -1);
    void replace_len(string &line, string& len);
};

