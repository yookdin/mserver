//
//  Call.cpp
//  mserver
//
//  Created by Yuval Dinari on 12/11/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "Call.hpp"
#include "SipMessage.hpp"


//==========================================================================================================
// First message has a complete From header, but an incomplete To header.
//==========================================================================================================
Call::Call(int _number, SipMessage* first_msg): number(_number), dir(first_msg->dir)
{
    string from = first_msg->get_value("From", true);
    string to = first_msg->get_value("To", true);
    
    if(dir == OUT)
    {
        me = from;
        other = to;
    }
    else
    {
        other = from;
        me = to;
    }
}


//==========================================================================================================
// If message has a complete To header - update the value of either 'me' or 'other' (according to call direction)
//==========================================================================================================
void Call::update(SipMessage* msg)
{
    if(to_completed)
    {
        return; // already updated
    }

    regex tag_regex(";tag=");
    string to = msg->get_value("To", true);
    
    if(!regex_search(to, tag_regex))
    {
        return; // To header has no tag yet (e.g. a "100 Trying" message)
    }
    else // To header has a tag! update
    {
        if(dir == OUT)
        {
            other = to;
        }
        else
        {
            me = to;
        }
        
        to_completed = true;
    }
}



















