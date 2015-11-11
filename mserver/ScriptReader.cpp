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
// Static regular expression describing commands start
//==========================================================================================================
regex ScriptReader::command_start_regex("^ *<(answer)|^ *<(pause)|^ *<(recv)|^ *<(scenario)|^ *<(send)");


//==========================================================================================================
// Init command map, read and execute file.
//==========================================================================================================
ScriptReader::ScriptReader(string filepath)
{
    commands["scenario"] = new ScenarioCommand(*this);
    commands["send"] = new SendCommand(*this);
    commands["recv"] = new RecvCommand(*this);
    commands["pause"] = new PauseCommand(*this);
    commands["answer"] = new AnswerCommand(*this);
    
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
// Get the value for a variable that appear in the script in brackets (like [call_id]). Some are static to
// the test (received from MServer), some generated, some stored from previous messages.
//==========================================================================================================
string ScriptReader::get_value(string var, string last_descriptor)
{
    if(var == BRANCH)
    {
        return gen_branch();
    }
    
    if(var == CALL_ID)
    {
        return gen_call_id();
    }
    
    if(var == TAG)
    {
        return gen_tag();
    }
    
    if(is_last_var(var))
    {
        return get_last_value(var, last_descriptor);
    }
    
    if(vars.count(var) == 0)
    {
        return MServer::inst.get_value(var);
    }
    
    return vars[var];
}


//==========================================================================================================
//==========================================================================================================
string ScriptReader::gen_branch()
{
    return "i_am_a_branch_string"; // TODO
}


//==========================================================================================================
//==========================================================================================================
string ScriptReader::gen_call_id()
{
    return "i_am_a_call_id_string"; // todo
}


//==========================================================================================================
//==========================================================================================================
string ScriptReader::gen_tag()
{
    return "i_am_a_tag_string"; // todo
}


//==========================================================================================================
//==========================================================================================================
void ScriptReader::add_message(SipMessage& msg)
{
    messages.push_back(msg);
}


//==========================================================================================================
//==========================================================================================================
bool ScriptReader::is_last_var(string& var)
{
    return regex_match(var, regex("last_\\w+"));
}


//==========================================================================================================
//==========================================================================================================
string ScriptReader::get_last_value(string& var, string& last_descriptor)
{
    SipMessage& last_msg = get_last_message(last_descriptor);
    string short_var = var.substr(5); // w/o the "last_"
    return last_msg.get_value(short_var);
}


//==========================================================================================================
//==========================================================================================================
SipMessage& ScriptReader::get_last_message(string& last_descriptor)
{
    if(messages.empty())
    {
        throw string("Can't use last_*, no previous messages");
    }
    
    if(last_descriptor.empty())
    {
        return messages.back();
    }
    else
    {
        throw string("Last descriptor \""  + last_descriptor + "\" not supported");
    }
}












