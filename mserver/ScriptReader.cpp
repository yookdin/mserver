#include "common_headers.h"
#include "ScriptReader.h"
#include "Command.h"
#include "ScenarioCommand.h"
#include "SendCommand.h"
#include "RecvCommand.h"
#include "PauseCommand.h"
#include "AnswerCommand.h"
#include "MServer.hpp"

//==========================================================================================================
//==========================================================================================================
regex ScriptReader::command_start_regex("^ *<(answer)|^ *<(pause)|^ *<(recv)|^ *<(scenario)|^ *<(send)>");


//==========================================================================================================
//==========================================================================================================
ScriptReader::ScriptReader(string filepath)
{
    commands["scenario"] = new ScenarioCommand(*this);
    commands["send"] = new SendCommand(*this);
    commands["recv"] = new RecvCommand(*this);
    commands["pause"] = new PauseCommand(*this);
    commands["answer"] = new AnswerCommand(*this);
    
    // TMP, for testing purposes, will replaced by values from MServer or values generated during read
    vars["transport"] = "tcp";
    vars["branch"] = "z9hG4bKPjkCnP1tH3DTBjBrlojZPYdV6aaDWiiV.e";
    vars["call_number"] = "123456";
    vars["call_id"] = "x4eVMfdlOg3XN2Tp0Ucy7btabxHb-gby1446389861";
    vars["len"] = "13";

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
            // Skip the first match, it is the entire match
			for(auto iter = ++res.begin(); iter != res.end(); iter++)
			{
				if(! iter->str().empty())
				{
					command_name = iter->str();
					break;
				}
			}

			line = res.suffix().str(); // Skip the matched part, so the command won't need to deal with it
            commands[command_name]->interpret(line, file);
		}
	}
}


//==========================================================================================================
//==========================================================================================================
string & ScriptReader::get_value(string var)
{
    if(vars.count(var) == 0)
    {
        return MServer::inst.get_value(var);
    }
    
    return vars[var];
}























