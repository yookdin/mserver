#include "common_headers.h"
#include "ScriptReader.h"
#include "Command.h"
#include "ScenarioCommand.h"
#include "SendCommand.h"
#include "RecvCommand.h"
#include "PauseCommand.h"
#include "AnswerCommand.h"
#include "SipParser.hpp"

//==========================================================================================================
// Declare ScriptReader static varibales
//==========================================================================================================
regex ScriptReader::command_start_regex;
map<string, Command*> ScriptReader::commands;

//==========================================================================================================
// Init commands map and command_start_regex; will be called only once
//==========================================================================================================
map<string, Command*>& ScriptReader::init_commands()
{
    commands["scenario"] = new ScenarioCommand();
    commands["send"] = new SendCommand();
    commands["recv"] = new RecvCommand();
    commands["pause"] = new PauseCommand();
    commands["answer"] = new AnswerCommand();

    string s;
    auto last = --commands.end();
    
    for(auto iter = commands.begin(); iter != commands.end(); iter++)
    {
        s += iter->second->get_start_regex();
        if(iter != last)
        {
            s += "|";
        }
    }
    
    command_start_regex = s;

    return commands;
}

//==========================================================================================================
//==========================================================================================================
ScriptReader::ScriptReader(string filepath)
{
    if(commands.empty())
    {
        init_commands();
    }
    
    read_file(filepath);
}

//==========================================================================================================
//==========================================================================================================
void ScriptReader::read_file(string filepath)
{
	ifstream file(filepath);

	if(!file.is_open())
	{
		cout << "File " << filepath << " not found" << endl;
		return;
	}

	for(string line; getline(file, line);)
	{
		smatch res;

		if(regex_search(line, res, command_start_regex))
		{
			string command_name;
			
			// Find the sub-match which succeeded; that is the command name
			for(auto iter = ++res.begin(); iter != res.end(); iter++)
			{
				if(! iter->str().empty())
				{
					command_name = iter->str();
					break;
				}
			}

			line = res.suffix().str(); // Skip the matched part, so the command won't need to deal with it
			cout << command_name << endl;
			commands[command_name]->interpret(line, file);
		}
	}
}

