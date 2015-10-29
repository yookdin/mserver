#include "RecvCommand.h"


RecvCommand::RecvCommand(void)
{
	start_regex = "^ *<(recv)>";
}


RecvCommand::~RecvCommand(void)
{
}

void RecvCommand::interpret(string &line, ifstream &file)
{
	//cout << "RecvCommand::interpret()" << endl;
}