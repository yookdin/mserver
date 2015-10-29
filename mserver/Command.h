#pragma once

#include "common_headers.h"

class Command
{
public:
	Command();
	~Command();
	
	string get_start_regex();
	virtual void interpret(string &line, ifstream &file) = 0;

protected:
	string start_regex;
};

