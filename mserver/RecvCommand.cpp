/*
 
<recv message=NAME[ optional][ timeout=NUM]>
</recv>
 
Default values:
 optional = false
 timeout = 1 second
 
TODO:
 o Add regex in message name?
 o Add checks in command body (on expected values of headers, etc.)?
 
*/


#include "RecvCommand.h"
#include "MServer.hpp"
#include "OptionParser.hpp"


//==========================================================================================================
//==========================================================================================================
void RecvCommand::interpret(string &line, ifstream &file)
{
    string message_kind; // Either a METHOD name of status code
    bool optional = false;
    int timeout = 2; // In seconds
    process_args(line, message_kind, optional, timeout);
    
    SipMessage* msg = MServer::inst.get_message(message_kind, timeout);
    
    if(msg == nullptr && !optional)
    {
        string plural = (timeout > 1 ? "s" : "");
        throw string("Expected message " + message_kind + " didn't arrive (timed out after " + to_string(timeout) + " second" + plural + ")");
    }
}


//==========================================================================================================
//==========================================================================================================
void RecvCommand::process_args(string& line, string& message_kind, bool& optional, int& timeout)
{
    string msg_opt = "message", optional_opt = "optional", timeout_opt = "timeout";
    
    map<string, Option> options;
    options.emplace(msg_opt, Option(true, true));
    options.emplace(optional_opt, Option(false, false));
    options.emplace(timeout_opt, Option(false, true));
    OptionParser parser(line, options);
    
    message_kind = options.at(msg_opt).val;
    optional = options.at(optional_opt).found;
    
    if(options.at(timeout_opt).found)
    {
        timeout = stoi(options.at(timeout_opt).val);
    }
}







