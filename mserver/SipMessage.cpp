//
//  SipMessage.cpp
//  mserver
//
//  Created by Yuval Dinari on 11/2/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "SipMessage.hpp"
#include "SipParser.hpp"
#include "ScriptReader.h"


//==========================================================================================================
// Construct message from a vector of strings, received from script file.
//==========================================================================================================
SipMessage::SipMessage(vector<string>& _lines): lines(_lines), dir(OUT)
{
    parse(true);
}


//==========================================================================================================
// Construct message from char* buffer received from socket.
// The buffer might contain more than one sip message. Try to construct a message from it, and set offset to
// indicate how many bytes where used.
//==========================================================================================================
SipMessage::SipMessage(char *buf, long &offset, long num_bytes): dir(IN)
{
    char *cur_buf = buf;
    long remaining_bytes = num_bytes;
    
    while(remaining_bytes > 0)
    {
        string cur_line;
        get_sip_line(cur_buf, remaining_bytes, cur_line);
        
        if(cur_line.empty())
        {
            throw string("No SIP message found in buffer");
        }
        
        // Check if this line is not a start of a new message. If at least one line added and the current line
        // is a start line it belongs to the next message
        if(!lines.empty() && SipParser::inst().match(START_LINE, cur_line))
        {
            cur_buf -= cur_line.length(); // We read one line too many, roll back cur_buf
            break;
        }
        
        lines.push_back(cur_line); // cur_line copied to lines
    }
    
    offset = cur_buf - buf;
    parse(false);
}


//==========================================================================================================
// Get a line ending with CRLF and adjust parameters accordingly
//==========================================================================================================
void SipMessage::get_sip_line(char*& cur_buf, long& remaining_bytes, string& line)
{
    for(int i = 0; i < remaining_bytes - 1; ++i)
    {
        if(cur_buf[i] == '\r' && cur_buf[i+1] == '\n')
        {
            int len = i+2;
            line = string(cur_buf, len);
            remaining_bytes -= len;
            cur_buf += len;

            return;
        }
    }
}

//==========================================================================================================
// Parse the message contained in lines and throw error if not valid.
//==========================================================================================================
void SipMessage::parse(bool from_script)
{
    for(auto &line: lines)
    {
        if(from_script)
        {
            line += CRLF;
        }
        
        size += line.length();
    }
    
    auto parser = SipParser::inst();
    
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
    long header_size = lines[0].length();
    int found_len = -1;
    
    for(int i = 1; i < lines.size(); ++i)
    {
        header_size += lines[i].length();

        if(lines[i] == CRLF) // empty line
        {
            if(!in_header)
            {
                throw string("SIP message should contain only one empty line, to separate header from body");
            }
            
            in_header = false;
            break;
        }
        
        if(in_header)
        {
            if(!parser.match(HEADER_LINE, lines[i]))
            {
                string dir = (from_script ? "Outgoing" : "Incoming");
                throw string(dir + " " + kind + " SIP message contains non header line in header section line " + to_string(i) + ":\n" + lines[i]);
            }
            
            string header_name = parser.get_sub_match(HEADER_NAME);
            
            if(header_name == "CSeq")
            {
                cseq = extract_cseq(parser.get_sub_match(HEADER_VALUE));
            }
            else if(header_name == "Call-ID")
            {
                call_id = parser.get_sub_match(HEADER_VALUE);
            }
            
            // Need to check the Content-Length header only for incoming messages, not ones read from script
            if(!from_script && header_name == "Content-Length")
            {
                found_len = stoi(parser.get_sub_match(HEADER_VALUE));
            }
            
        }
    }
    
    // in_header can remian true only if user didn't write an empty line at the end of the header section. So instead of failing just add it
    if(in_header)
    {
        lines.push_back(CRLF);
        size += 2;
        header_size += 2;
    }
    
    if(found_len != -1 && !from_script)
    {
        long len = size - header_size;
        
        if(len != found_len)
        {
            throw string("Content-Length has wrong value: " + to_string(found_len) + " instead of " + to_string(len));
        }
    }
    
    if(cseq.empty())
    {
        throw string("No CSeq header in message!");
    }
    
    if(call_id.empty())
    {
        throw string("No Call-ID header in message");
    }
} // parse()


//==========================================================================================================
// Check that the received message parameters match those of the call it belongs to
//==========================================================================================================
void SipMessage::check_call_params(int call_number, ScriptReader& reader)
{
    SipMessage* last = reader.get_last_message(call_number);
    
    if(last == nullptr)
    {
        return;
    }
    
    if(call_id != last->get_call_id())
    {
        throw string("Received message " + kind + " call-id is different than the call call-id");
    }
    
    // TODO: add checks on other headers?
}


//==========================================================================================================
//==========================================================================================================
string SipMessage::extract_cseq(string header_value)
{
    if(!SipParser::inst().match(CSEQ_VALUE, header_value))
    {
        throw string("Wrong CSeq header value: " + header_value);
    }
    
    return SipParser::inst().get_sub_match(NUM);
}


//==========================================================================================================
// Write this message to given buffer and set num_to_write to the num of bytes written
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
// Get a value from the current message according to the var string. The var can represent an entire header,
// just the header value, or other data (if var starts with "last_", that part is ignored).
// var = <header-name>          :   the entire header line will be returned (not including CRLF)
// var = <header-name>:value    :   only the value part of the header line
// Special vars:
// var = cseq                   :   the numerical part of CSeq header
// var = To_no_tag              :   "To" header without the tag part
//==========================================================================================================
string SipMessage::get_value(string& var)
{
    smatch match;
    
    if(!regex_match(var, match, ScriptReader::query_regex))
    {
        throw string("SipMessage::get_value(): wrong format of var: " + var);
    }
    
    string name = match[2];
    
    if(name == CSEQ)
    {
        return cseq;
    }
    
    bool to_no_tag = false;
    
    if(name == "To_no_tag")
    {
        name = "To";
        to_no_tag = true;
    }
    
    bool entire_hdr = match[3].str().empty(); // No ":value" in query string
    string result;
    
    for(int i = 1; i < lines.size() && !lines[i].empty(); ++i)
    {
        SipParser::inst().match(HEADER_LINE, lines[i]);

        if(name == SipParser::inst().get_sub_match(HEADER_NAME))
        {
            result = (entire_hdr ? (name + ": ") : "") + SipParser::inst().get_sub_match(HEADER_VALUE);
            break;
        }
    }
    
    if(!result.empty())
    {
        if(to_no_tag)
        {
            // Remove the to-tag from the header
            result = regex_replace(result, SipParser::inst().to_tag_regex, "");
        }
        
        return result;
    }
    
    throw string("SipMessage::get_value(): header " + name + " not found");
}


//==========================================================================================================
//==========================================================================================================
void SipMessage::set_call_number(int call_num)
{
    if(call_number != -1)
    {
        throw string("Call number already set!");
    }
    
    call_number = call_num;
}


//==========================================================================================================
//==========================================================================================================
void SipMessage::print()
{
    string dir_str = (dir == IN ? "Incoming" : "Outgoing");
    cout << endl << dir_str << " message";
    cout << endl << "----------------" << endl;

    for(auto s: lines)
    {
        cout << s;
    }
}












