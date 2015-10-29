#include "SendCommand.h"


SendCommand::SendCommand(void)
{
	start_regex = "^ *<(send)>";
}


SendCommand::~SendCommand(void)
{
}

void SendCommand::interpret(string &line, ifstream &file)
{
	//cout << "SendCommand::interpret()" << endl;
}