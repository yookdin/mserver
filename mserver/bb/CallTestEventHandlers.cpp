//
//  CallTestEventHandlers.cpp
//  VoxipBBTests
//
//  Created by Yuval Dinari on 10/15/15.
//  Copyright © 2015 Yuval Dinari. All rights reserved.
//

#include "CallTest.h"


//======================================================================================================================
//======================================================================================================================
CallTest::CallTest()
{
    event_handlers[CallState_Calling]                   = &CallTest::handle_calling;
    event_handlers[CallState_AlertIncoming]             = &CallTest::handle_alertincoming;
    event_handlers[CallStateVideo_AlertIncomingVideo]   = &CallTest::handle_alertincomingvideo;
    event_handlers[CallState_Connected]                 = &CallTest::handle_connected;
    event_handlers[CallState_OnHold]                    = &CallTest::handle_onhold;
    event_handlers[CallState_RemoteHold]                = &CallTest::handle_remotehold;
    event_handlers[CallState_OnHold_GSM]                = &CallTest::handle_onhold_gsm;
    event_handlers[CallState_GSM_End]                   = &CallTest::handle_gsm_end;
    event_handlers[CallState_Hangup]                    = &CallTest::handle_hangup;
    event_handlers[MessageState_SendArsMessage]         = &CallTest::handle_sendarsmessage;
    event_handlers[CallState_SipCallId]                 = &CallTest::handle_sipcallid;
}

//======================================================================================================================
//======================================================================================================================
void CallTest::handle_calling(string call_id, Call *call, const char *data)
{
    if(call != nullptr)
    {
        throw string("Expected call to be null for event CallState_Calling, call-id = " + call_id);
    }
    
    calls[call_id] = new Call(Call::OUT);
}


//======================================================================================================================
//======================================================================================================================
void CallTest::handle_alertincoming(string call_id, Call *call, const char *data)
{
    if(call != nullptr)
    {
        throw string("Expected call to be null for event CallState_AlertIncoming, call-id = " + call_id);
    }
    
    calls[call_id] = new Call(Call::IN);
}


//======================================================================================================================
//======================================================================================================================
void CallTest::handle_alertincomingvideo(string call_id, Call *call, const char *data)
{
    if(call != nullptr)
    {
        throw string("Expected call to be null for event CallStateVideo_AlertIncomingVideo, call-id = " + call_id);
    }
    
    calls[call_id] = new Call(Call::IN);
}


//======================================================================================================================
//======================================================================================================================
void CallTest::handle_connected(string call_id, Call *call, const char *data)
{
    if(call == nullptr) // Can happen in intersecting scenario
    {
        calls[call_id] = new Call(get_json_value(data, "Param"));
    }
    else
    {
        string param_val = get_json_value(data, "Param");
        
        if(param_val == "in" || param_val == "out")
        {
            call->check_direction(get_json_value(data, "Param"));
        }
        
        call->connect();
    }
}


//======================================================================================================================
//======================================================================================================================
void CallTest::handle_onhold(string call_id, Call *call, const char *data)
{
    check_call_connected(call_id);
    call->local_hold_on();
}


//======================================================================================================================
//======================================================================================================================
void CallTest::handle_remotehold(string call_id, Call *call, const char *data)
{
    check_call_connected(call_id);
    call->remote_hold_on();
}


//======================================================================================================================
//======================================================================================================================
void CallTest::handle_onhold_gsm(string call_id, Call *call, const char *data)
{
    if(call != nullptr)
    {
        call->gsm_hold_on();
    }
}


//======================================================================================================================
//======================================================================================================================
void CallTest::handle_gsm_end(string call_id, Call *call, const char *data)
{
    if(call != nullptr)
    {
        call->gsm_hold_off();
    }
}


//======================================================================================================================
//======================================================================================================================
void CallTest::handle_hangup(string call_id, Call *call, const char *data)
{
    if(call != nullptr)
    {
        call->end();
    }
    
}


//======================================================================================================================
//======================================================================================================================
void CallTest::handle_sipcallid(string call_id, Call *call, const char *data)
{
    if(call != nullptr)
    {
        call->sip_id = get_json_value(data, "SIP_ID");
    }
}


//======================================================================================================================
//======================================================================================================================
void CallTest::handle_sendarsmessage(string call_id, Call *call, const char *data)
{
    string sip_id = get_json_value(data, "CALLID");
    
    if(sip_id.empty())
    {
        return; // Can happen when the call failed, e.g. due to no registration
    }
    
    call = get_call_by_sip_id(sip_id); // The input call is always null for this event

    if(call == nullptr) // Call can be null, for example when rejecting an incoming call
    {
        return;
    }
    
    call->telemetries = data;
}




























