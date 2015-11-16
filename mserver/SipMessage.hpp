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

class SipMessage {
public:
    SipMessage(vector<string> &lines);
    SipMessage(char *buf, long &offset, long num_bytes);
    
    enum Direction {IN, OUT, ANY};
    
    string get_kind();
    void write_to_buffer(char buf[], long &num_to_write);
    string get_value(string& var);
    bool match(Direction dir, string kind);
    void print();
    
private:
    static const regex value_regex;
    
    const Direction dir;
    vector<string> lines;
    string kind;
    string cseq;
    int size = 0;
    
    void parse(bool add_crlf);
    string* get_sip_line(char*& cur_buf, long& remaining_bytes);
    string get_cseq(string header_value);
};

#endif /* SipMessage_hpp */
