#include "common_headers.h"
#include "ScriptReader.h"
#include "Command.h"
#include "ScenarioCommand.h"
#include "SendCommand.h"
#include "RecvCommand.h"
#include "PauseCommand.h"
#include "ExpectCommand.hpp"
#include "MServer.hpp"

//==========================================================================================================
// Static regular expression describing commands start
//==========================================================================================================
const regex ScriptReader::command_start_regex("^ *<(expect)>|^ *<(pause)|^ *<(recv)|^ *<(scenario)|^ *<(send)");


//==========================================================================================================
// Last message descriptor regular expression
// Interesting submatches:
// 1: Ordinal or 'last'
// 2: Numerical part of ordinal
// 4: in/out
// 6: message kind (Method/status-code)
//==========================================================================================================
const regex ScriptReader::last_desc_regex("((\\d+)[[:alpha:]]{2}|last)( +(in|out))?( +(" + SipParser::inst().method_str + "|\\d{3}))?$");

//==========================================================================================================
// Some commong regular expressions for identifying variable in script
//==========================================================================================================
const string ScriptReader::query_str("[-\\w]+");
const regex ScriptReader::last_query_regex("last_" + ScriptReader::query_str);
const regex ScriptReader::script_var_regex("\\[(" + ScriptReader::query_str + ")\\]");

//==========================================================================================================
// A set of characters allowed in SIP protocol for element 'token'
//==========================================================================================================
const string ScriptReader::sip_token_chars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-.!%*_+`'~");

//==========================================================================================================
// Init command map, read and execute file.
//==========================================================================================================
ScriptReader::ScriptReader(string filepath, map<string, string> _vars, bool _root): vars(_vars), root(_root)
{
    commands["scenario"] = new ScenarioCommand(*this);
    commands["send"] = new SendCommand(*this);
    commands["recv"] = new RecvCommand(*this);
    commands["pause"] = new PauseCommand(*this);
    commands["expect"] = new ExpectCommand(*this);
    
    read_file(filepath);
}


//==========================================================================================================
//==========================================================================================================
ScriptReader::~ScriptReader()
{
    for(auto pair: commands)
    {
        delete pair.second;
    }
    
    if(root)
    {
        for(auto msg: messages)
        {
            delete msg;
        }
    }
}

//==========================================================================================================
//==========================================================================================================
void ScriptReader::read_file(string filename)
{
    string filepath = MServer::inst.get_value(SCENARIO_DIR) + "/" + filename;
	ifstream file(filepath);

	if(!file.is_open())
	{
        throw string("File " + filepath + " not found");
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

			line = res.suffix(); // Skip the matched part, so the command won't need to deal with it
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
// Generate a random branch string, that must begin with the constant prefix according to the spec.
//==========================================================================================================
string ScriptReader::gen_branch()
{
    string res = "z9hG4bK"; // Must start with this
    gen_random_string(res);
    return res;
}


//==========================================================================================================
// Generate the call-id string
//==========================================================================================================
string ScriptReader::gen_call_id()
{
    switch(MServer::inst.get_call_id_kind())
    {
        case MServer::NONE: {
            string res;
            gen_random_string(res);
            return res;
        }
        case MServer::MIN: {
            return "!";
        }
        case MServer::MAX: {
            return string(50, '~');
        }
    }
}


//==========================================================================================================
// Generate a random tag string, at least 4 bytes long
//==========================================================================================================
string ScriptReader::gen_tag()
{
    string res;
    gen_random_string(res, 4, &sip_token_chars); // Must be at least 32-bits long
    return res;
}


//==========================================================================================================
// Generate a random string, no smaller than min_len. If input str isn't empty append generated chars.
// If char_set isn't null, use it to choose the chars from.
//==========================================================================================================
void ScriptReader::gen_random_string(string& str, int min_len, const string* char_set)
{
    int max_len = 30; // Arbitrary

    if(min_len < 1 || min_len > max_len)
    {
        throw string("gen_random_string(): min_len must be in [1.." + to_string(max_len) + "]");
    }
    
    min_len -= str.length();                // Reduce because string already contains data
    int len_range = max_len - min_len + 1;  // Number of values from min_len to max_len
    int len = rand() % len_range + min_len; // add min_len because [rand() % x] is [0..x-1]
    
    for(int i = 0; i < len; ++i)
    {
        if(char_set != nullptr)
        {
            int index = rand() % char_set->length();
            str += (*char_set)[index];
        }
        else
        {
            int char_range = (126-33+1); // ASCII table printables are between 33 and 126
            char c = rand() % char_range + 33;
            str += c;
        }
    }
}


//==========================================================================================================
//==========================================================================================================
void ScriptReader::add_message(SipMessage* msg)
{
    messages.push_back(msg);
}


//==========================================================================================================
//==========================================================================================================
void ScriptReader::add_messages(vector<SipMessage*>& _messages)
{
    for(auto m: _messages)
    {
        messages.push_back(m);
    }
}


//==========================================================================================================
//==========================================================================================================
vector<SipMessage*>& ScriptReader::get_messages()
{
    return messages;
}


//==========================================================================================================
//==========================================================================================================
bool ScriptReader::is_last_var(string& var)
{
    return regex_match(var, last_query_regex);
}


//==========================================================================================================
// Return the value of last_* variable
//==========================================================================================================
string ScriptReader::get_last_value(string& var, string& last_descriptor)
{
    SipMessage& last_msg = get_last_message(last_descriptor);
    string short_var = var.substr(5); // without the "last_"
    return last_msg.get_value(short_var);
}


//==========================================================================================================
// Return a message from which the values of last_* params will be taken, according the last_descriptor
// string, which format is:
// <ordinal>|last [in|out] [Method|status-code]
// For example:
// "2nd in INVITE"  - return the second incoming INVITE
// "last 183"       - return the last 183 response, no matter if it was incoming or outgoing
// If last_descriptor is empty, return the last message.
//==========================================================================================================
SipMessage& ScriptReader::get_last_message(string& last_descriptor)
{
    if(messages.empty())
    {
        throw string("Can't use last_*, no previous messages");
    }
    
    if(last_descriptor.empty()) // Default is the last message
    {
        return *messages.back();
    }

    smatch match;
    
    if(!regex_match(last_descriptor, match, last_desc_regex))
    {
        throw string("Wrong last descriptor format: \"" + last_descriptor + "\"");
    }
    
    //------------------------------------------------------------------------------------------------------
    // Get values for the search from the submatches:
    // 1: Ordinal or 'last'
    // 2: Numerical part of ordinal
    // 4: in|out
    // 6: message kind (Method|status-code)
    //------------------------------------------------------------------------------------------------------
    int num = -1;
    SipMessage::Direction dir = SipMessage::ANY;
    string kind = match[6];
    
    if(!match[2].str().empty())
    {
        num = stoi(match[2]);
        
        if(num <= 0)
        {
            throw string("Ordinal of last_descriptor must be positive");
        }
    }
    
    if(!match[4].str().empty())
    {
        if(match[4] == "in")
        {
            dir = SipMessage::IN;
        }
        else
        {
            dir = SipMessage::OUT;
        }
    }
    
    // If 'last', go over messages from the end and return the first that matches
    if(num == -1)
    {
        for(long i = messages.size() - 1; i >= 0; --i)
        {
            if(messages[i]->match(dir, kind))
            {
                return *messages[i];
            }
        }
    }
    else // If a number specified, go over messages from the begining and count until you reach the requested number
    {
        int cur_num = 0;
        
        for(auto msg: messages)
        {
            if(msg->match(dir, kind) && ++cur_num == num)
            {
                return *msg;
            }
        }
    }
    
    // This is reached only if no matching message found
    throw string("No message matches last descriptor \"" + last_descriptor + "\"");
}












