
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
    
    for(string line; getline(file, line);) // Skip current line containing <send>
    {
        if(regex_search(line, end_regex))
        {
            break;
        }
        
        trim(line);
        replace_vars(line);
        msg_lines.push_back(line);
    }
    
    if(msg_lines.empty())
    {
        throw string("Send command must have a SIP message specified");
    }
    
    SipMessage message(msg_lines);
}