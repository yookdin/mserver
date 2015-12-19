/*
 
<recv message=NAME [call_number=NUM] [optional] [timeout=NUM]>
</recv>
 
Default values:
 optional = false
 timeout = 1 second
 
TODO:
 o Add regex in message name?
 
*/


#include "RecvCommand.h"
#include "MServer.hpp"
#include "mserver_utils.hpp"
#include "ScriptReader.h"
#include "AssignmentEvaluator.hpp"

//==========================================================================================================
//==========================================================================================================
void RecvCommand::interpret(string &line, ifstream &file, ScriptReader &reader)
{
    process_args(line, reader);
    SipMessage* msg = MServer::inst.get_sip_message(message_kind, optional, timeout);
    
    if(!optional)
    {
        if(msg == nullptr)
        {
            string plural = (timeout > 1 ? "s" : "");
            throw string("Expected message " + message_kind + " didn't arrive (timed out after " + to_string(timeout) + " second" + plural + ")");
        }

        if(msg->get_kind() != message_kind)
        {
            throw string("Received message " + msg->get_kind() + " while expecting message " + message_kind);
        }
    }
    
    if(msg != nullptr)
    {
        // Check call params only if call number explicitly specified. Automatic detection of call number is tricky, and need
        // some work
        if(call_number != -1)
        {
            msg->check_call_params(call_number, reader); // Must call this before adding the messagae, o/w it, the message, will become the last of its call
        }
        
        reader.add_message(msg);
    }
}


//==========================================================================================================
//==========================================================================================================
void RecvCommand::process_args(string& line, ScriptReader &reader)
{
    message_kind.clear();
    optional = false;
    timeout = 2;
    call_number = -1;

    map<string, string> vars;
    AssignmentEvaluator::inst()->eval(line, vars, reader);
    string msg_opt = "message", optional_opt = "optional", timeout_opt = "timeout", call_number_opt = "call_number";
    
    if(vars.count(msg_opt) == 0)
    {
        throw string("No 'message' option to recv command");
    }
    else
    {
        message_kind = vars[msg_opt];
    }
    
    if(vars.count(call_number_opt))
    {
        call_number = stoi(vars[call_number_opt]);
    }
    
    if(vars.count(optional_opt) > 0)
    {
        optional = stob(vars[optional_opt]);
    }
    
    if(vars.count(timeout_opt) > 0)
    {
        timeout = stoi(vars[timeout_opt]);
    }
}




