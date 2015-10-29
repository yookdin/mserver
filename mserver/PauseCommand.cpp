#include "PauseCommand.h"

PauseCommand::PauseCommand(void)
{
	start_regex = "<(pause)";
}

PauseCommand::~PauseCommand(void)
{
}

void PauseCommand::interpret(string &line, ifstream &file)
{
	//cout << "PauseCommand::interpret()" << endl;
}