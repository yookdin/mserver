#pragma once

#include "Command.h"
#include "SipMessage.hpp"

class ScriptReader
{
public:
	ScriptReader(string filepath);
    string get_value(string var, string last_descriptor = "");
    void add_message(SipMessage*);
    void add_messages(vector<SipMessage*>& messages);
    vector<SipMessage*>& get_messages();
    
private:
    static const regex command_start_regex;
    static regex last_desc_regex;
    
    map<string, Command*> commands;
    map<string, string> vars; // Map of var names and their values

    vector<SipMessage*> messages;
    
    void read_file(string filepath);
    string gen_branch();
    string gen_call_id();
    string gen_tag();
    bool is_last_var(string&);
    string get_last_value(string& var, string& last_descriptor);
    SipMessage& get_last_message(string& last_descriptor);
};

