//
//  SipMessage.hpp
//  mserver
//
//  Created by Yuval Dinari on 11/2/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef SipMessage_hpp
#define SipMessage_hpp

#include "common_headers.h"

class ScriptReader;

class SipMessage {
public:
    static bool is_message_var(string var);
    
    SipMessage(vector<string>& lines);
    SipMessage(char *buf, long &offset, long num_bytes);
    
    enum Direction {IN, OUT, ANY};
    
    string get_kind()     { return kind; }
    string get_call_id()  { return call_id; }
    int get_call_number() { return call_number; }

    void check_call_params(int call_number, ScriptReader& reader); // Check that received message parameters match those of the call it belongs to
    void write_to_buffer(char buf[], long &num_to_write);
    string get_value(string& var);
    void set_call_number(int call_num);
    void print();
    
private:
    // List of variable names that can be given to the get_value() function
    static const vector<string> message_vars;
    
    const Direction dir;
    vector<string> lines;
    string kind;    // INVITE, 200, etc.
    string call_id;
    string cseq;    // numeric part of CSeq field
    
    int size = 0;   // In bytes of entire message
    
    // Serial number of "call" in the script. By call I mean a group of messages with the same call-id,
    // so it's not the same as SIP dialog, and regsitration messages also considered calls.
    int call_number = -1;
    
    void parse(bool from_script);
    void get_sip_line(char*& cur_buf, long& remaining_bytes, string& line);
    string extract_cseq(string header_value);
    string get_sdp_send_recv();
};

#endif /* SipMessage_hpp */
