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
// Static commands, shared by all script readers
//==========================================================================================================
map<string, Command*> ScriptReader::commands = init_commands();


//==========================================================================================================
// Static regular expression describing commands start. Must initialize it after commands initialization
//==========================================================================================================
const regex ScriptReader::command_start_regex = ScriptReader::init_command_start_regex();


//==========================================================================================================
// Initialize the commands map and calculate the commands start regex
//==========================================================================================================
map<string, Command*> ScriptReader::init_commands()
{
    map<string, Command*> local_commands;
    local_commands["scenario"] = new ScenarioCommand();
    local_commands["send"] = new SendCommand();
    local_commands["recv"] = new RecvCommand();
    local_commands["pause"] = new PauseCommand();
    local_commands["expect"] = new ExpectCommand();

    return local_commands;
}


//==========================================================================================================
//==========================================================================================================
regex ScriptReader::init_command_start_regex()
{
    string command_start_str;
    
    for(auto com: commands)
    {
        command_start_str += "^ *" + com.second->get_start_regex_str() + "|";
    }
    
    command_start_str.erase(command_start_str.length() - 1); // Remove the last '|'
    return regex(command_start_str);
}


//==========================================================================================================
// Some commong regular expressions for identifying variable in script
//==========================================================================================================
const string ScriptReader::query_str("(last_)?([-\\w]+)(:value)?");
const regex ScriptReader::query_regex(ScriptReader::query_str);
const regex ScriptReader::script_var_regex("\\[(" + ScriptReader::query_str + ")\\]");


//==========================================================================================================
// Init command map, read and execute file.
//==========================================================================================================
ScriptReader::ScriptReader(string filepath, map<string, string> _vars, ScriptReader* parent):
    vars(_vars), root(parent == nullptr)
{
    if(root)
    {
        calls_num_map = new CallsNumMap;
    }
    else
    {
        calls_num_map = parent->calls_num_map;
    }
    
    read_file(filepath);
}


//==========================================================================================================
//==========================================================================================================
ScriptReader::~ScriptReader()
{
    if(root)
    {
        for(auto msg: messages)
        {
            delete msg;
        }
        
        delete calls_num_map;
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

    cout << "Running scenario " << filepath << endl;
    
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
            commands[command_name]->interpret(line, file, *this);
		}
	}
}


//==========================================================================================================
// Get the value for a variable that appear in the script in brackets (like [call_id]). Some are static to
// the test (received from MServer), some generated, some stored from previous messages.
//==========================================================================================================
string ScriptReader::get_value(string var, int call_number)
{
    if(var == BRANCH)
    {
        return gen_branch();
    }
    
    CallIDKind cid_kind = string_to_call_id_kind(var);
    
    if(cid_kind != NONE)
    {
        return gen_call_id(cid_kind);
    }
    
    if(var == TAG)
    {
        return gen_tag();
    }
    
    if(is_last_var(var))
    {
        return get_last_value(var, call_number);
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
    gen_random_string(res, 1, &SipParser::inst().sip_token_chars);
    return res;
}


//==========================================================================================================
// Generate the call-id string
//==========================================================================================================
string ScriptReader::gen_call_id(CallIDKind kind)
{
    switch(kind)
    {
        case RANDOM:
        {
            string res;
            gen_random_string(res, 1, &SipParser::inst().sip_word_chars);
            return res;
        }
        case MIN:
        {
            return "!";
        }
        case MAX:
        {
            return string(50, '~');
        }
        default:
            throw string("ScriptReader::gen_call_id() called with NONE");
    }
}


//==========================================================================================================
//==========================================================================================================
ScriptReader::CallIDKind ScriptReader::string_to_call_id_kind(string str)
{
    if(str == CALL_ID)
    {
        return RANDOM;
    }
    
    if(str == MIN_CALL_ID)
    {
        return MIN;
    }
    
    if(str == MAX_CALL_ID)
    {
        return MAX;
    }

    return NONE;
}

//==========================================================================================================
// Generate a random tag string, at least 4 bytes long
//==========================================================================================================
string ScriptReader::gen_tag()
{
    string res;
    gen_random_string(res, 4, &SipParser::inst().sip_token_chars); // Must be at least 32-bits long
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
void ScriptReader::add_message(SipMessage* msg, bool from_child_script)
{
    messages.push_back(msg);
    
    if(!from_child_script) // o/w call number already set in child script
    {
        msg->set_call_number(calls_num_map->get_call_num(msg->get_call_id()));
    }
}


//==========================================================================================================
//==========================================================================================================
void ScriptReader::add_messages(vector<SipMessage*>& _messages)
{
    for(auto m: _messages)
    {
        add_message(m, true);
    }
}


//==========================================================================================================
//==========================================================================================================
vector<SipMessage*>& ScriptReader::get_messages()
{
    return messages;
}


//==========================================================================================================
// Return true if var is a valid query string starting with "last_"
//==========================================================================================================
bool ScriptReader::is_last_var(string& var)
{
    smatch match;

    if(regex_match(var, match, query_regex))
    {
        return !(match[1].str().empty());
    }
    
    return false;
}


//==========================================================================================================
// Return the value of last_* variable
//==========================================================================================================
string ScriptReader::get_last_value(string& var, int call_number)
{
    SipMessage* last_msg = get_last_message(call_number);
    
    if(last_msg == nullptr)
    {
        throw string("Can't use last_*, no previous messages");
    }
    return last_msg->get_value(var);
}


//==========================================================================================================
// Return the last message of the given call number. If the the call_number is -1, return the last message.
//==========================================================================================================
SipMessage* ScriptReader::get_last_message(int call_number)
{
    if(messages.empty())
    {
        return nullptr;
    }
    
    if(call_number == -1) // Default is the last message
    {
        return messages.back();
    }

    // Go over messages in reverse order
    for(long i = messages.size() - 1; i >= 0; --i)
    {
        if(messages[i]->get_call_number() == call_number)
        {
            return messages[i];
        }
    }

    // This is reached only if no matching message found
    throw string("ScriptReader::get_last_message(): call number " + to_string(call_number) + " doesn't exist");
}



//==========================================================================================================
//==========================================================================================================
int ScriptReader::CallsNumMap::get_call_num(string call_id)
{
    if(id_num_map.count(call_id) != 0)
    {
        return id_num_map[call_id];
    }
    
    id_num_map[call_id] = ++last_call_num;
    return last_call_num;
}


































