
/*
 
 <send [call_number=NUM]>
    SIP-MESSAGE
 </send>
 
*/

#include "SendCommand.h"
#include "SipMessage.hpp"
#include "MServer.hpp"
#include "ScriptReader.h"
#include "OptionParser.hpp"


//==========================================================================================================
//==========================================================================================================
const regex SendCommand::end_regex("</send>");


//==========================================================================================================
//==========================================================================================================
void SendCommand::interpret(string &line, ifstream &file)
{
    process_args(line);
    vector<string> msg_lines;
    replcae_vars(file, msg_lines);
    
    if(msg_lines.empty())
    {
        throw string("Send command must have a SIP message specified");
    }
    
    SipMessage* message = new SipMessage(msg_lines);
    MServer::inst.send_sip_message(*message);
    reader.add_message(message);
}


//==========================================================================================================
// Read lines from file until end_regex is encountered, and replace occurrences of vars with values. Put lines
// in input vector.
// Calculate [len] field - the lenght of the body section.
//==========================================================================================================
void SendCommand::replcae_vars(ifstream &file, vector<string>& msg_lines)
{
    bool calc_len = false;
    int len = 0;
    vector<long> len_lines_indices; // Lines containins [len]
    
    for(string line; getline(file, line);)
    {
        if(regex_search(line, end_regex))
        {
            break;
        }
        
        trim(line); // Remove leading white spaces and tabs, final newline
        bool contains_len = replace_vars(line, call_number); // replace_vars() return true if it sees "[len]"
        msg_lines.push_back(line);
        
        if(contains_len)
        {
            len_lines_indices.push_back(msg_lines.size() - 1);
        }
        
        if(calc_len)
        {
            len += line.length() + 2; // The plus 2 is for the \r\n that will be appended after each line
        }
        
        if(line.empty()) //  This is the last line of the header section, after which there's the optional body
        {
            calc_len = true;
        }
    }
    
    // Replace [len] in all the lines that contain it; can it really be more than one line?
    string len_str = to_string(len);
    
    for(auto index: len_lines_indices)
    {
        replace_len(msg_lines[index], len_str);
    }
}


//==========================================================================================================
//==========================================================================================================
void SendCommand::process_args(string& line)
{
    string opt = "call_number";
    map<string, Option> options;
    options.emplace(opt, Option(false, true));
    OptionParser parser(line, options, '>');

    if(options.at(opt).was_found())
    {
        call_number = stoi(options.at(opt).get_value());
    }
}
































