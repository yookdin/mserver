#include "common.h"
#include "ScriptReader.h"
#include "Command.h"
#include "ScenarioCommand.h"
#include "SendCommand.h"
#include "RecvCommand.h"
#include "PauseCommand.h"
#include "ExpectCommand.hpp"
#include "NextIPCommand.hpp"
#include "StopListeningCommand.hpp"
#include "StartListeningCommand.hpp"
#include "SetCommand.hpp"
#include "PrintCommand.hpp"
#include "MServer.hpp"


//==========================================================================================================
// Init command map, read and execute file.
//==========================================================================================================
ScriptReader::ScriptReader(string _filename, map<string, string> _vars, ScriptReader* parent):
    filename(_filename), vars(_vars), root(parent == nullptr)
{
    print_title();
    
    // Check validity of variable-value pairs
    for(auto& p: vars)
    {
        check_valid_value(p.first, p.second);
    }
    
    if(root)
    {
        calls_map = new CallsMap;
        messages = new vector<SipMessage*>;
    }
    else
    {
        calls_map = parent->calls_map;
        messages = parent->messages;
    }
    
    vars[DEFAULT_REQUEST_BODY] = default_request_body;
    vars[DEFAULT_RESPONSE_BODY] = default_response_body;
    vars[DEFAULT_VIDEO_REQUEST_BODY] = default_video_request_body;
    vars[DEFAULT_VIDEO_RESPONSE_BODY] = default_video_response_body;
    vars[DEFAULT_100_TRYING] = default_100_trying;
    vars[DEFAULT_ACK] = default_ack;
    
    keyword_funcs["if"] = &ScriptReader::handle_if;
    keyword_funcs["elseif"] = &ScriptReader::handle_elseif;
    keyword_funcs["else"] = &ScriptReader::handle_else;
    keyword_funcs["endif"] = &ScriptReader::handle_endif;

    
    interpret(filename);
    print_end_title();
}


//==========================================================================================================
//==========================================================================================================
ScriptReader::~ScriptReader()
{
    if(root)
    {
        for(auto msg: *messages)
        {
            delete msg;
        }
        
        delete messages;
        delete calls_map;
    }
}


//==========================================================================================================
//==========================================================================================================
void ScriptReader::interpret(string filename)
{
    string filepath = MServer::inst.get_value(SCENARIO_DIR) + "/" + filename;
    ifstream file(filepath);
    
    if(!file.is_open())
    {
        throw string("File " + filepath + " not found");
    }

    for(string line; getline(file, line);) {
        keyword_func func = get_keyword_func(line);
        
        if(func == nullptr)
        {
            if(should_execute())
                search_command(line, file);
        }
        else
        {
            (this->*func)(line);
        }
    }
    
    bool non_closed_if = !if_stack.empty();
    
    while(!if_stack.empty())
    {
        delete if_stack.top();
        if_stack.pop();
    }
    
    if(non_closed_if)
    {
        throw string("If statement not closed");
    }
}


//==================================================================================================
//==================================================================================================
ScriptReader::keyword_func ScriptReader::get_keyword_func(string& line)
{
    regex re("^ *(\\w+)");
    smatch match;
    bool found = regex_search(line, match, re);
    string keyword = match[1];
    
    if(found && keyword_funcs.count(keyword) > 0)
    {
        line = match.suffix();
        return keyword_funcs[keyword];
    }
    
    return nullptr;
}


//==================================================================================================
// Search for a command on the given line and execute it if found.
//==================================================================================================
void ScriptReader::search_command(string& line, ifstream& file)
{
    smatch match;
    
    if(regex_search(line, match, command_start_regex))
    {
        string command_name;
        
        // Find the sub-match which succeeded; that is the command name
        // Skip the first match, it is the entire match
        for(auto iter = ++match.begin(); iter != match.end(); iter++)
        {
            if(! iter->str().empty())
            {
                command_name = iter->str();
                break;
            }
        }
        
        line = match.suffix(); // Skip the matched part, so the command won't need to deal with it
        commands[command_name]->interpret(line, file, *this);
    }
}


//==================================================================================================
//==================================================================================================
void ScriptReader::handle_if(string& line)
{
    cout << "handle_if " << line << endl;
    if_stack.push(new IfStatement(should_execute(), line));
}


//==================================================================================================
//==================================================================================================
void ScriptReader::handle_else(string& line)
{
    cout << "handle_else" << endl;
    if(if_stack.empty())
    {
        throw string("else with no preceding if");
    }
    
    if_stack.top()->switch_to_else();
}


//==================================================================================================
//==================================================================================================
void ScriptReader::handle_elseif(string& line)
{
    cout << "handle_elseif " << line << endl;

    if(if_stack.empty())
    {
        throw string("elseif with no preceding if");
    }

    if_stack.top()->switch_to_else();
    if_stack.push(new IfStatement(should_execute(), line, true));
}


//==================================================================================================
//==================================================================================================
void ScriptReader::handle_endif(string& line)
{
    cout << "handle_endif " << endl;
    IfStatement* top;
    
    do
    {
        top = if_stack.top();
        if_stack.pop();
    }
    while(top->implicit);
}


//==========================================================================================================
// Should current line be executed, according to if statement status if one exists.
//==========================================================================================================
bool ScriptReader::should_execute() {
    if(if_stack.empty())
    {
        return true;
    }
    else
    {
        return if_stack.top()->should_execute;
    }
}


//==========================================================================================================
// First replace [var] with values, then replace \[var\] with [var].
// If no occurrences, the line is unchanged.
//==========================================================================================================
void ScriptReader::replace_vars(string &line, int call_number)
{
    replace_regular_vars(line, call_number);
    replace_literal_vars(line);
}


//==========================================================================================================
// A returning value version of replace_vars()
//==========================================================================================================
string ScriptReader::get_replaced_str(string line, int call_number)
{
    string result = line;
    replace_vars(result, call_number);
    return result;
}


//==========================================================================================================
// Replace variable specified by a name inside brackets: [var_name] with their value.
// Note: the replacement itself may contain references to other variables, which will be replaced too.
//==========================================================================================================
void ScriptReader::replace_regular_vars(string &line, int call_number)
{
    sregex_iterator iter(line.begin(), line.end(), ScriptReader::script_var_regex);
    sregex_iterator end;
    
    if(iter == end)
    {
        return;
    }

    string line2 = line;
    long offset = 0;
        
    // Replace all occurrences of [var] in the string
    for(;iter != end; ++iter) {
        string var = (*iter)[1].str();
        
        if(var == "len")
        {
            continue; // [len] is not yet known, it should be replaced after reading the entire message and knowing the body length
        }
        
        string value = get_final_value(var, call_number); // Get final value returns a string with no [var] in it, replacing recursively if needed
        line2.replace(iter->position() + offset, iter->length(), value);
        offset += value.length() - iter->length();
    }
    
    line = line2;
}


//==========================================================================================================
// Replace all occurrences of \[var\] with [var].
//==========================================================================================================
void ScriptReader::replace_literal_vars(string &line)
{
    sregex_iterator iter(line.begin(), line.end(), ScriptReader::literal_var_regex);
    sregex_iterator end;
    
    if(iter == end)
    {
        return;
    }
    
    string line2 = line;
    long offset = 0;
    
    for(;iter != end; ++iter) {
        string var = (*iter)[0].str();
        
        // Remove the backslahes
        var.erase(0, 1);
        var.erase(var.length() - 2, 1);
        
        line2.replace(iter->position() + offset, iter->length(), var);
        offset += var.length() - iter->length();
    }
    
    line = line2;
}


//==========================================================================================================
// This is called from replace_regular_vars(), and may call replace_regular_vars() itself, which creates the
// recursive replacements of [var] occurrences. When no more vars, replace_regular_vars() will not call
// get_final_value(), and thus the recursion will end.
//==========================================================================================================
string ScriptReader::get_final_value(string var, int call_number)
{
    string result = get_value(var, call_number);
    
    // Random call-id might contain a string looking like a var: "...[xxx]...", or even "[call_id]", and
    // anyway we don't want to recursively replace random generated strings
    bool final = (var == CALL_ID || var == MIN_CALL_ID || var == MAX_CALL_ID || var == BRANCH || var == TAG);

    if(!final)
    {
        replace_regular_vars(result, call_number); // If no further vars in result, then it will be unchanged.
    }

    return result;
}


//==========================================================================================================
// Get the value for a variable that appear in the script in brackets (like [call_id]). Some are static to
// the test (received from MServer), some generated, some stored from previous messages.
//==========================================================================================================
string ScriptReader::get_value(string var, int call_number, bool try_as_last)
{
    smatch match;
    
    if(!regex_match(var, match, var_regex))
    {
        throw string("Wrong format for var in ScriptReader::get_value(): " + var);
    }
    
    // var_regex:   "(last_)?(((cseq)\\+(\\d+))|([-\\w]+))(:value)?"
    // submatches:   1       234        5       6         7
    bool is_last = (match[1].length() > 0);
    bool just_value = (match[7].length() >  0);
    int add_to_cseq = 0;
    string name = match[2];

    if(match[3].length() > 0)
    {
        name = match[4];
        add_to_cseq = stoi(match[5]);
    }
    
    if(is_last || (try_as_last && SipMessage::is_message_var(name)))
    {
        string result = get_last_value(name, call_number, just_value);
        
        if(name == CSEQ)
        {
            return to_string(stoi(result) + add_to_cseq);
        }
        
        return result;
    }
    
    if(name == BRANCH)
    {
        return gen_branch();
    }
    
    CallIDKind cid_kind = string_to_call_id_kind(name);
    
    if(cid_kind != NONE)
    {
        return gen_call_id(cid_kind);
    }
    
    if(name == TAG)
    {
        return gen_tag();
    }
    
    if(vars.count(name) != 0)
    {
        return vars[name];
    }
    
    return MServer::inst.get_value(name);
}


//==========================================================================================================
// Generate a random branch string, that must begin with the constant prefix according to the spec.
//==========================================================================================================
string ScriptReader::gen_branch()
{
    string res = "z9hG4bK"; // Must start with this
    gen_random_string(res, 10, &SipParser::inst().sip_token_chars);
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
void ScriptReader::add_message(SipMessage* msg)
{
    messages->push_back(msg);
    msg->set_call(calls_map->get_call(msg));
}


//==========================================================================================================
// Return the value of last_* variable
//==========================================================================================================
string ScriptReader::get_last_value(string& var, int call_number, bool just_value)
{
    SipMessage* last_msg = get_last_message(call_number);
    
    if(last_msg == nullptr)
    {
        throw string("Can't use last_*, no previous messages");
    }
    return last_msg->get_value(var, just_value);
}


//==========================================================================================================
// Return the last message of the given call number. If the the call_number is -1, return the last message.
//==========================================================================================================
SipMessage* ScriptReader::get_last_message(int call_number)
{
    if(messages->empty())
    {
        return nullptr;
    }
    
    if(call_number == -1) // Default is the last message
    {
        return messages->back();
    }

    // Go over messages in reverse order
    for(long i = messages->size() - 1; i >= 0; --i)
    {
        if(messages->at(i)->get_call_number() == call_number)
        {
            return messages->at(i);
        }
    }

    // This is reached only if no matching message found
    throw string("ScriptReader::get_last_message(): call number " + to_string(call_number) + " doesn't exist");
}


//==========================================================================================================
//==========================================================================================================
void ScriptReader::set_value(string var, string value, bool overwirte)
{
    check_valid_value(var, value);
    
    if(vars.count(var) == 0 || overwirte)
    {
        vars[var] = value;
    }
}


//==========================================================================================================
// Check for self-reference: if var=[var], it will cause an infinite loop when trying to get value for this
// variable.
//==========================================================================================================
void ScriptReader::check_valid_value(string var, string value)
{
    if(value == '[' + var + ']')
    {
        throw string("Self reference error: setting variable \"" + var + "\" to \"" + value + "\" will cause an infinite loop");
    }
}


//==========================================================================================================
//==========================================================================================================
ScriptReader::CallsMap::~CallsMap()
{
    for(auto& p: calls_map)
    {
        delete p.second;
    }
}


//==========================================================================================================
// If call-id exists, return the call that it is mapped to.
// If not create a new call object. Its direction will be the direction of its first message.
//==========================================================================================================
Call* ScriptReader::CallsMap::get_call(SipMessage* msg)
{
    string call_id = msg->get_call_id();
    Call* call;

    if(calls_map.count(call_id) != 0)
    {
        call = calls_map[call_id];
        call->update(msg);
        return call;
    }
    else
    {
        calls_map[call_id] = new Call(++last_call_num, msg);
        return calls_map[call_id];
    }
}


//==========================================================================================================
//==========================================================================================================
void ScriptReader::print_title()
{
    cout << "====================================================================================================" << endl;
    cout << "RUNNING SCENARIO: " << filename << " ";
    
    for(auto p: vars)
    {
        cout << p.first << "=" << p.second << " ";
    }

    cout << endl << "====================================================================================================" << endl;
}


//==========================================================================================================
//==========================================================================================================
void ScriptReader::print_end_title()
{
    cout << "----------------------------------------------------------------------------------------------------" << endl;
    cout << "                      FINISHED SCENARIO: " << filename << endl;
    cout << "----------------------------------------------------------------------------------------------------" << endl << endl;
}


//==========================================================================================================
//==========================================================================================================
void ScriptReader::print_continue_title()
{
    cout << "CONTINUING SCENARIO: " << filename << endl << endl;
}


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
    local_commands["move_to_next_ip"] = new NextIPCommand();
    local_commands["stop_listening"] = new StopListeningCommand();
    local_commands["start_listening"] = new StartListeningCommand();
    local_commands["set"] = new SetCommand();
    local_commands["print"] = new PrintCommand();
    
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
// A regular expression for identifying variable in script
//==========================================================================================================
const regex ScriptReader::var_regex(var_regex_str);

// Matches [var]
const regex ScriptReader::script_var_regex("\\[(" + var_regex_str + ")\\]");

// Matches \[var\]
const regex ScriptReader::literal_var_regex("\\\\\\[" + var_regex_str + "\\\\\\]");


//==========================================================================================================
// A default body for SIP responses. Usually used where the body doesn't really matter to the test.
//==========================================================================================================
const string ScriptReader::default_response_body =
"v=0\n\
o=- 3607660610 3607660611 IN IP[sip_ip_type] [server_ip]\n\
s=voxip_media\n\
b=AS:174\n\
t=0 0\n\
a=X-nat:0\n\
m=audio [audio_port] RTP/AVP 0 101\n\
c=IN IP[audio_ip_type] [audio_ip]\n\
b=TIAS:150000\n\
a=sendrecv\n\
a=rtpmap:0 PCMU/8000\n\
a=rtpmap:101 telephone-event/8000\n\
a=fmtp:101 0-15";


//==========================================================================================================
// A default body for SIP requests. Usually used where the body doesn't really matter to the test.
//==========================================================================================================
const string ScriptReader::default_request_body =
"v=0\n\
o=- 3607660610 3607660611 IN IP[client_ip_type] [client_ip]\n\
s=voxip_media\n\
b=AS:174\n\
t=0 0\n\
a=X-nat:0\n\
m=audio [audio_port] RTP/AVP 0 101\n\
c=IN IP[audio_ip_type] [audio_ip]\n\
b=TIAS:150000\n\
a=sendrecv\n\
a=rtpmap:0 PCMU/8000\n\
a=rtpmap:101 telephone-event/8000\n\
a=fmtp:101 0-15";


//==========================================================================================================
//==========================================================================================================
const string ScriptReader::default_video_response_body =
"v=0\n\
o=- 3607660610 3607660611 IN IP[sip_ip_type] [server_ip]\n\
s=voxip_media\n\
b=AS:174\n\
t=0 0\n\
a=X-nat:0\n\
m=audio [audio_port] RTP/AVP 104 101\n\
c=IN IP[audio_ip_type] [audio_ip]\n\
b=TIAS:150000\n\
a=sendrecv\n\
a=rtpmap:104 ISAC/16000\n\
a=rtpmap:101 telephone-event/8000\n\
a=fmtp:101 0-15\n\
m=video [video_port] RTP/AVP 110 111 112\n\
c=IN IP[video_ip_type] [video_ip]\n\
b=TIAS:2000000\n\
a=sendrecv\n\
a=rtpmap:110 VP8/90000\n\
a=fmtp:110 max-fs=300; max-fr=15; width=212; height=362\n\
a=rtpmap:111 red/90000\n\
a=rtpmap:112 ulpfec/90000";


//==========================================================================================================
//==========================================================================================================
const string ScriptReader::default_video_request_body =
"v=0\n\
o=- 3607660610 3607660611 IN IP[client_ip_type] [client_ip]\n\
s=voxip_media\n\
b=AS:174\n\
t=0 0\n\
a=X-nat:0\n\
m=audio [audio_port] RTP/AVP 104 101\n\
c=IN IP[audio_ip_type] [audio_ip]\n\
b=TIAS:150000\n\
a=sendrecv\n\
a=rtpmap:104 ISAC/16000\n\
a=rtpmap:101 telephone-event/8000\n\
a=fmtp:101 0-15\n\
m=video [video_port] RTP/AVP 110 111 112\n\
c=IN IP[video_ip_type] [video_ip]\n\
b=TIAS:2000000\n\
a=sendrecv\n\
a=rtpmap:110 VP8/90000\n\
a=fmtp:110 max-fs=300; max-fr=15; width=212; height=362\n\
a=rtpmap:111 red/90000\n\
a=rtpmap:112 ulpfec/90000";

//==========================================================================================================
//==========================================================================================================
const string ScriptReader::default_100_trying =
"SIP/2.0 100 Trying\n\
[last_Via]\n\
[last_From]\n\
[last_To]\n\
[last_Call-ID]\n\
[last_CSeq]\n\
Content-Length: 0";


//==========================================================================================================
//==========================================================================================================
const string ScriptReader::default_ack =
"ACK sip:1800707070@[server_ip]:[server_port];transport=tcp SIP/2.0\n\
[last_Via]\n\
[last_From]\n\
[last_To]\n\
[last_Call-ID]\n\
Max-Forwards: 70\n\
CSeq: [last_cseq] ACK\n\
Content-Length: 0";



//==========================================================================================================
//==========================================================================================================
const string ScriptReader::default_183_header =
"SIP/2.0 183 Session Progress\n\
[last_Via]\n\
[last_From]\n\
[last_To];tag=[tag]\n\
[last_Call-ID]\n\
[last_CSeq]\n\
User-Agent: bb-test\n\
Contact: <sip:12345678900@[server_ip]:[server_port];transport=[transport]>\n\
Alert-Info: <file://ringing.wav>\n\
Content-Type: application/sdp\n\
Content-Length: [len]\n";


//==========================================================================================================
//==========================================================================================================
const string ScriptReader::default_183 = ScriptReader::default_183_header + "\n" + ScriptReader::default_response_body;



























