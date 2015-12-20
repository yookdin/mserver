
/*
 
 <send [call_number=NUM]>
    SIP-MESSAGE
 </send>
 
*/

#include "SendCommand.h"
#include "SipMessage.hpp"
#include "MServer.hpp"
#include "ScriptReader.h"
#include "mserver_utils.hpp"
#include "AssignmentEvaluator.hpp"

//==========================================================================================================
//==========================================================================================================
SendCommand::SendCommand(): Command("send"), end_regex("\\bendsend\\b") {}


//==========================================================================================================
// Note: the rest of the line after the closing > of the command start is ignored
//==========================================================================================================
void SendCommand::interpret(string &line, ifstream &file, ScriptReader &reader)
{
    process_args(line, reader);
    vector<string> msg_lines;
    replcae_vars(file, msg_lines, reader);
    
    if(msg_lines.empty())
    {
        throw string("Send command must have a SIP message specified");
    }
    
    SipMessage* message = new SipMessage(msg_lines);
    
    if(!MServer::inst.send_sip_message(*message))
    {
        throw string("Send of message " + message->get_kind() + " failed");
    }
    
    reader.add_message(message);
}


//==========================================================================================================
// Read lines from file until end_regex is encountered, and replace occurrences of vars with values. Put lines
// in input vector.
// Calculate [len] field - the lenght of the body section.
//==========================================================================================================
void SendCommand::replcae_vars(ifstream &file, vector<string>& msg_lines, ScriptReader &reader)
{
    bool calc_len = false;
    int len = 0; // Value of Content-Length header
    long content_length_index = -1; // Index of the line of the Content-Length header
    
    for(string line; getline(file, line);)
    {
        if(regex_search(line, end_regex))
        {
            break;
        }
        
        trim(line); // Remove leading white spaces and tabs, final newline
        reader.replace_vars(line, call_number);

        if(line.empty()) // This is the last line of the header section, after which there's the optional body
        {
            calc_len = true;
            msg_lines.push_back(line);
        }
        else
        {
            //----------------------------------------------------------------------------------------------
            // Line may contain newline chars after replacement. Split to single lines and add to list.
            //----------------------------------------------------------------------------------------------
            stringstream ss(line);
            string single_line;
            
            while(getline(ss, single_line))
            {
                if(calc_len)
                {
                    len += single_line.length() + 2; // The plus 2 is for the \r\n that will be appended after each line
                }
                
                if(single_line.empty()) // This is the last line of the header section, after which there's the optional body
                {
                    calc_len = true;
                }
                
                if(single_line.find("[len]") != string::npos)
                {
                    if(content_length_index != -1)
                    {
                        throw string("Expected only one [len] occurrence");
                    }
                    
                    content_length_index = msg_lines.size();
                }
                
                msg_lines.push_back(single_line);
            }
        }
    }
    
    // Replace [len] with its value
    if(content_length_index > -1)
    {
        string len_str = to_string(len);
        replace_len(msg_lines[content_length_index], len_str);
    }
}


//==========================================================================================================
//==========================================================================================================
void SendCommand::process_args(string& line, ScriptReader &reader)
{
    call_number = -1;
    string call_number_opt = "call_number";
    map<string, string> vars;
    AssignmentEvaluator::inst()->eval(line, vars, reader);
    
    if(vars.count(call_number_opt))
    {
        call_number = stoi(vars[call_number_opt]);
    }
}

//==========================================================================================================
// Replace occurences of [len] with the given string
//==========================================================================================================
void SendCommand::replace_len(string &line, string& len)
{
    string line2 = line;
    regex var_re("\\[len\\]");
    sregex_iterator iter(line.begin(), line.end(), var_re);
    sregex_iterator end;
    long offset = 0;
    
    while(iter != end) {
        line2.replace(iter->position() + offset, iter->length(), len);
        offset += len.length() - iter->length();
        iter++;
    }
    
    line = line2;
}

