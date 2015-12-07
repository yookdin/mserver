#pragma once

#include "Command.h"
#include "SipMessage.hpp"

//======================================================================================================================
//======================================================================================================================
class ScriptReader
{
public:
	ScriptReader(string filename, map<string, string> vars, ScriptReader* parent = nullptr);
    ~ScriptReader();
    
    static const regex query_regex;
    static const regex script_var_regex;

    string get_value(string var, int call_number = -1);
    void add_message(SipMessage* msg, bool from_child_script = false);
    void add_messages(vector<SipMessage*>& messages);
    vector<SipMessage*>& get_messages();
    SipMessage* get_last_message(int call_number = -1);
    
private:
    //------------------------------------------------------------------------------------------------------------------
    // Controls the generation of call id
    //------------------------------------------------------------------------------------------------------------------
    enum CallIDKind
    {
        RANDOM, // Genrate random string
        MIN,    // Generate lexicographically minimal call id
        MAX,    // Generate lexicographically maximal call id
        NONE
    };

    static const string query_str;
    static const regex command_start_regex;
    static map<string, Command*> commands;
    static map<string, Command*> init_commands();
    static regex init_command_start_regex();
    static const string default_response_sip_msg_body;
    static const string default_request_sip_msg_body;
    static const string default_100_trying;
    static const string default_183_header;
    static const string default_183;
    
    bool root;
    map<string, string> vars; // Map of var names and their values
    vector<SipMessage*> messages;
    
    void read_file(string filepath);
    string gen_branch();
    string gen_call_id(CallIDKind kind);
    string gen_tag();
    void gen_random_string(string& str, int min_length = 1, const string* char_set = nullptr);
    CallIDKind string_to_call_id_kind(string str);
    bool is_last_var(string&);
    string get_last_value(string& var, int call_number = -1);
    void print_title(string filepath);
    void print_end_title(string filepath);
    
    
    //==================================================================================================================
    // A map between call-ids and call numbers
    //==================================================================================================================
    class CallsNumMap
    {
    public:
        int get_call_num(string call_id);
        
    private:
        map<string, int> id_num_map; // Map between call-ids and call numbers
        int last_call_num = -1;
        
    } *calls_num_map;
};

