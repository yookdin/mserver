
/*
 
 <send>
    SIP-MESSAGE
 </send>
 
*/

#include "SendCommand.h"
#include "SipMessage.hpp"


//==========================================================================================================
//==========================================================================================================
const regex SendCommand::end_regex("</send>");


//==========================================================================================================
//
//==========================================================================================================
void SendCommand::interpret(string &line, ifstream &file)
{
    vector<string> msg_lines;
    replcae_vars(file, msg_lines);
    
    if(msg_lines.empty())
    {
        throw string("Send command must have a SIP message specified");
    }
    
    SipMessage message(msg_lines);
}


//==========================================================================================================
// Read lines from file until end_regex is encountered, and replace occurrences of vars with values. Put lines
// in input vector.
//==========================================================================================================
void SendCommand::replcae_vars(ifstream &file, vector<string>& msg_lines)
{
    bool calc_len = false;
    int body_len = 0;
    vector<string*> len_lines; // Lines containins [len]
    
    for(string line; getline(file, line);)
    {
        if(regex_search(line, end_regex))
        {
            break;
        }
        
        trim(line); // Remove leading white spaces and tabs, final newline
        bool contains_len = replace_vars(line); // replace_vars() return true if it sees "[len]"
        msg_lines.push_back(line);
        
        if(contains_len)
        {
            len_lines.push_back(&msg_lines.back());
        }
        
        if(calc_len)
        {
            body_len += line.length() + 2; // The plus 2 is for the \r\n that will be appended after each line
        }
        
        if(line.empty()) //  This is the last line of the header section, after which there's the optional body
        {
            calc_len = true;
        }
    }
    
    // Replace [len] in all the lines that contain it; can it really be more than one line?
    string len_str = to_string(body_len);
    
    for(auto pline: len_lines)
    {
        replace_len(*pline, len_str);
    }
}
