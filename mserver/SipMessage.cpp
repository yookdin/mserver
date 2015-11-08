//
//  SipMessage.cpp
//  mserver
//
//  Created by Yuval Dinari on 11/2/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "SipMessage.hpp"
#include "SipParser.hpp"

//==========================================================================================================
// Construct message from a vector of strings, received from script file.
//==========================================================================================================
SipMessage::SipMessage(vector<string> &_lines): lines(_lines)
{
    parse(true);
}


//==========================================================================================================
// Construct message from char* buffer received from socket.
// The buffer might contain more than one sip message. Try to construct a message from it, and set offset to
// indicate how many bytes where used.
//==========================================================================================================
SipMessage::SipMessage(char *buf, long &offset, long num_bytes)
{
    char *cur_buf = buf;
    long remaining_bytes = num_bytes;
    
    while(remaining_bytes > 0)
    {
        string * p_cur_line = get_sip_line(cur_buf, remaining_bytes);
        
        if(p_cur_line == nullptr)
        {
            throw string("No SIP message found in buffer");
        }
        
        // Check if this line is not a start of a new message. If at least one line added and the current line
        // is a start line it belongs to the next message
        if(!lines.empty() && SipParser::inst.match(START_LINE, *p_cur_line))
        {
            cur_buf -= p_cur_line->length(); // We read one line too many, roll back cur_buf
            delete p_cur_line;
            break;
        }
        
        lines.push_back(*p_cur_line);
    }
    
    offset = cur_buf - buf;
    parse(false);
}


//==========================================================================================================
// Get a line ending with CRLF and adjust parameters accordingly
//==========================================================================================================
string* SipMessage::get_sip_line(char*& cur_buf, long& remaining_bytes)
{
    for(int i = 0; i < remaining_bytes - 1; ++i)
    {
        if(cur_buf[i] == '\r' && cur_buf[i+1] == '\n')
        {
            int len = i+2;
            string* res = new string(cur_buf, len);
            remaining_bytes -= len;
            cur_buf += len;

            return res;
        }
    }
    
    return nullptr; // No SIP line delimiter (CRLF) found
}

//==========================================================================================================
// Parse the message contained in lines and throw error if not valid.
//==========================================================================================================
void SipMessage::parse(bool add_crlf)
{
    for(auto &line: lines)
    {
        if(add_crlf)
        {
            line += CRLF;
        }
        
        size += line.length();
    }
    
    auto parser = SipParser::inst;
    
    if(!parser.match(START_LINE, lines[0]))
    {
        throw string("SIP messaage expected to start with request or status line but received:\n" + lines[0]);
    }
    
    if(!parser.get_sub_match(REQUEST_LINE).empty())
    {
        kind = parser.get_sub_match(METHOD);
    }
    else // STATUS_LINE
    {
        kind = parser.get_sub_match(STATUS_CODE);
    }
    
    bool in_header = true;
    
    for(int i = 1; i < lines.size(); ++i)
    {
        if(lines[i] == CRLF) // empty line
        {
            if(!in_header)
            {
                throw string("SIP message should contain only one empty line, to separate header from body");
            }
            
            in_header = false;
        }
        
        if(in_header)
        {
            if(!parser.match(HEADER_LINE, lines[i]))
            {
                throw string("SIP message contains non header line in header section line " + to_string(i) + ":\n" + lines[i]);
            }
        }
    }
    
    //print();
}


//==========================================================================================================
//==========================================================================================================
string SipMessage::get_kind()
{
    return kind;
}


//==========================================================================================================
//==========================================================================================================
void SipMessage::write_to_buffer(char buf[], long &num_to_write)
{
    if(size > CONNECTION_BUFFER_SIZE)
    {
        throw string("Message size " + to_string(size) + " exceeds buffer size " + to_string(CONNECTION_BUFFER_SIZE));
    }
    
    char *cur_buf = buf;
    
    for(auto& line: lines)
    {
        line.copy(cur_buf, line.length());
        cur_buf += line.length();
    }
    
    buf[size] = '\0'; // For printing; buf is actually bigger by 1 then CONNECTION_BUFFER_SIZE to enable this
    num_to_write = size;
}


//==========================================================================================================
//==========================================================================================================
void SipMessage::print()
{
    cout << "Message kind: " << kind << endl << endl;
    for(auto s: lines)
    {
        cout << s;
    }
}












