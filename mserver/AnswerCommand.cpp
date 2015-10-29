#include "AnswerCommand.h"


AnswerCommand::AnswerCommand(void)
{
	start_regex = "^ *<(answer)";
}


AnswerCommand::~AnswerCommand(void)
{
}

void AnswerCommand::interpret(string &line, ifstream &file)
{
	//cout << "AnswerCommand::interpret()" << endl;
}