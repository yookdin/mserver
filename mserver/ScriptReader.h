#pragma once

#include "Command.h"
#include "SipMessage.hpp"
#include "Call.hpp"
#include "IfStatement.hpp"

//======================================================================================================================
//======================================================================================================================
class ScriptReader
{
public:
	ScriptReader(string filename, map<string, string> vars, ScriptReader* parent = nullptr);
    ~ScriptReader();
    
    static const regex script_var_regex;
    static const regex literal_var_regex;

    void replace_vars(string &line, int call_number = -1);
    string get_replaced_str(string line, int call_number = -1);
    void set_value(string var, string value, bool overwirte);
    string get_value(string var, int call_number = -1, bool try_as_last = false);
    
    void add_message(SipMessage* msg);
    SipMessage* get_last_message(int call_number = -1);
    void print_continue_title();
    
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

    static const regex var_regex;
    static const regex command_start_regex;
    static const regex assignment_regex;
    
    static map<string, Command*> commands;
    static string set_command_name;
    
    static map<string, Command*> init_commands();
    static regex init_command_start_regex();

    static const string default_response_body;
    static const string default_request_body;
    static const string default_video_response_body;
    static const string default_video_request_body;
    static const string default_100_trying;
    static const string default_ack;
    static const string default_183_header;
    static const string default_183;
    
    string filename;
    bool root;
    map<string, string> vars; // Map of var names and their values
    vector<SipMessage*>* messages;

    stack<IfStatement*> if_stack;
    typedef void(ScriptReader::*keyword_func)(string&);
    map<string, keyword_func> keyword_funcs;
    
    void replace_regular_vars(string &line, int call_number);
    void replace_literal_vars(string &line);
    void check_valid_value(string var, string value);
    string get_final_value(string var, int call_number = -1);
    string gen_branch();
    string gen_call_id(CallIDKind kind);
    string gen_tag();
    void gen_random_string(string& str, int min_length = 1, const string* char_set = nullptr);
    CallIDKind string_to_call_id_kind(string str);
    string get_last_value(string& var, int call_number, bool just_value);
    void print_title();
    void print_end_title();
    
    void interpret(string filename);
    void search_command(string& line, ifstream& file);
    bool should_execute();
    keyword_func get_keyword_func(string& line);
    void handle_if(string& line);
    void handle_else(string& line);
    void handle_elseif(string& line);
    void handle_endif(string& line);
    
    
    //==================================================================================================================
    // A map between call-ids and calls
    //==================================================================================================================
    class CallsMap
    {
    public:
        ~CallsMap();
        Call* get_call(SipMessage* msg);
        
    private:
        map<string, Call*> calls_map; // Map between call-ids and call numbers
        int last_call_num = -1;
        
    } *calls_map;
};

