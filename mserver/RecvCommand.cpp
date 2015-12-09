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


//==========================================================================================================
// The recv command is actually a one liner, and could have been written like this:
// <recv message=NAME ... />
// But its written between begin and end tags to enable nesting expect commands in it:
// <recv ...>
//     <expect> ... </exepct>
//     <expect> ... </exepct>
//     ...
// </recv>
//
// There is no difference between nesting the expects and writing them after the recv command, it just seems
// more clear in the nested syntax.
//==========================================================================================================
void RecvCommand::interpret(string &line, ifstream &file, ScriptReader &reader)
{
    process_args(line);
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
        // INVITE starts a new call. Otherwise, the message belongs to a previous call and should check that its parameters (such as call-id)
        // match that call.
        if(message_kind != "INVITE")
        {
            msg->check_call_params(call_number, reader); // Must call this before adding the messagae, o/w it, the message, will become the last of its call
        }
        
        reader.add_message(msg);
    }
}


//==========================================================================================================
//==========================================================================================================
void RecvCommand::process_args(string& line)
{
    message_kind.clear();
    optional = false;
    timeout = 2;
    call_number =  -1;

    string msg_opt = "message", optional_opt = "optional", timeout_opt = "timeout", call_number_opt = "call_number";
    
    map<string, Option> options;
    options.emplace(msg_opt, Option(true, true));
    options.emplace(optional_opt, Option(false, false));
    options.emplace(timeout_opt, Option(false, true));
    options.emplace(call_number_opt, Option(false, true));
    OptionParser parser(line, options, '>');
    
    message_kind = options.at(msg_opt).get_value();
    optional = options.at(optional_opt).was_found();
    
    if(options.at(timeout_opt).was_found())
    {
        timeout = stoi(options.at(timeout_opt).get_value());
    }
    
    if(options.at(call_number_opt).was_found())
    {
        call_number = stoi(options.at(call_number_opt).get_value());
    }
}


//==========================================================================================================
//==========================================================================================================
string RecvCommand::get_start_regex_str()
{
    return "<(recv)";
}




