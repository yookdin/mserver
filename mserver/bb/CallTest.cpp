//
//  CallTest.cpp
//  VoxipCallTests
//
//  Created by Yuval Dinari on 7/8/15.
//  Copyright (c) 2015 Yuval Dinari. All rights reserved.
//

#include "CallTest.h"


//======================================================================================================================
// Called after each test
//======================================================================================================================
void CallTest::TearDown()
{
    in_teardown = true;
    for(auto &iter: calls)
    {
        delete iter.second;
    }
    calls.clear();
    BBTest::TearDown();
}

//======================================================================================================================
//======================================================================================================================
void CallTest::voxip_ui_callback(EVoXIPUIEvent event, const char* data)
{
    if(in_teardown) // Teardown process begun
    {
        return;
    }
    
    if(event_handlers.count(event) == 0) // No handler for this event
    {
        return;
    }
    
    Call *call = nullptr;
    string call_id = get_json_value(data, "CID");

    if(!call_id.empty()) // Not all events data have call-id field
    {
        call = get_call(call_id); // Might be null
    }

    // Find the right function and invoke it
    (this->*event_handlers[event])(call_id, call, data);
}


//======================================================================================================================
// Return call-state for given call-id
//======================================================================================================================
Call *CallTest::get_call(string call_id)
{
    if(calls.count(call_id) != 0)
    {
        return calls[call_id];
    }
    else
    {
        return nullptr;
    }
}


//======================================================================================================================
//======================================================================================================================
Call* CallTest::get_call_by_sip_id(string sip_id)
{
    // Find a pair in the map for which the lambda function returns true, i.e. the sip_id field equals the given sip_id.
    // To use the sip_id in the lambda it is needed to specify it in the capture list (the square brackets before the
    // lambda definition). It is specified with & to pass it be reference.
    auto iter = find_if(calls.begin(), calls.end(),
                        [&sip_id](string_call_pair val){ return val.second->sip_id == sip_id; });
    
    if(iter != calls.end())
    {
        return iter->second;
    }
    else
    {
        return nullptr;
    }
}

//======================================================================================================================
//======================================================================================================================
Call * CallTest::check_call_exists(string call_id)
{
    Call *call = get_call(call_id);
    
    if(call == nullptr)
    {
        throw string("Call " + quote(call_id) + " doesn't exist");
    }
    
    return call;
}


//======================================================================================================================
// Check that call exist and connected, and return it, o/w issue an error.
// TODO: if called from voxip_ui_callback(), there's no one to catch the exception, perhaps change
//======================================================================================================================
Call * CallTest::check_call_connected(string call_id)
{
    Call *call = check_call_exists(call_id);
    
    if(!call->connected)
    {
        throw string("Call " + quote(call_id) + " isn't connected");
    }
    
    return call;
}


//======================================================================================================================
// Return true if any not ended call exists 
//======================================================================================================================
bool CallTest::are_active_calls()
{
    // The weird third param is a lambda function, i.e. an unamed function whose definition is given inline, that acts
    // as the predicate for the count_if function.
    return count_if(calls.begin(),
                    calls.end(),
                    [](string_call_pair val){ return !val.second->ended; }) > 0;
}


//======================================================================================================================
// These are general checks, that can be doen every test if we choose so
//======================================================================================================================
void CallTest::check_telemetries()
{
    for(auto val: calls)
    {
        val.second->check_telemetries();
    }
}


//======================================================================================================================
//======================================================================================================================
void CallTest::post_internal_test_body()
{
    check_telemetries();
}


//======================================================================================================================
// Tell voxip to make call , and if successful hangup. If it fails, create error message, kill external processes and
// return false, o/w return true
// Will make Audio Call if False and Video Call if True
// If hasTraceId = false (default value) nothing is done, if it is true the JSON value wil be checked to make sure it is present.
//======================================================================================================================
void CallTest::call_and_hangup(bool is_video_call, bool hasTraceId)
{
    if(is_video_call)
    {
        video_call();
    }
    else
    {
        audio_call(hasTraceId);
    }
    
    hangup(CALL_UUID_1, hasTraceId);
    
}

//======================================================================================================================
// Tell voxip to receive call, and if successful hangup. If it fails, create error message, kill external processes and
// return false, o/w return true
//======================================================================================================================
void CallTest::answer_call_and_hangup(bool hasTraceId)
{
    string call_id = wait_for_incoming_call(hasTraceId);
    answer_call(call_id, hasTraceId);
    hangup(call_id, hasTraceId);
}

//======================================================================================================================
// Tell voxip to receive call, and if successful hangup.
// local_hold = true is local hold during call vs remote hold when false.
//======================================================================================================================
void CallTest::answer_call_and_hangup_with_hold(bool local_hold)
{
    string call_id = wait_for_incoming_call();
    answer_call(call_id);
    
    if(local_hold)
    {
        gsm_hold(true);
        gsm_unhold(true, true, call_id);
    }
    else
    {
        expect_remote_hold();
        expect_remote_unhold(call_id);
    }
    
    hangup(call_id);
}

//======================================================================================================================
// Tell voxip to receive call, and hangup while on Hold. If it fails, create error message, kill external processes and
// return false, o/w return true
// local_hold = true is local hold during call vs remote hold when false.
//======================================================================================================================
void CallTest::answer_call_and_hangup_while_on_hold(bool local_hold)
{
    string call_id = wait_for_incoming_call();
    answer_call(call_id);
    
    if(local_hold)
    {
        gsm_hold(true);
    }
    else
    {
        expect_remote_hold();
        expect_remote_unhold(call_id);
    }
    
    hangup(call_id);
}



//======================================================================================================================
// Tell voxip to receive call, and Remote side hangup while on Hold. If it fails, create error message, kill external processes and
// return false, o/w return true
// local_hold = true is local hold during call vs remote hold when false.
//======================================================================================================================
void CallTest::answer_call_and_remote_hangup_while_on_hold(bool local_hold)
{
    string call_id = wait_for_incoming_call();
    answer_call(call_id);
    
    if(local_hold)
    {
        gsm_hold(true);
    }
    else
    {
        expect_remote_hold();
    }
    
    confirm_remote_hangup();
}


//======================================================================================================================
//======================================================================================================================
void CallTest::call(string dest, string uuid, bool is_video)
{
    VOIPManager::Inst()->MakeCall(dest.c_str(), uuid.c_str(), is_video);
    
    if(!TestsEnv::inst()->wait_for_event(CallState_Connected, 20))
    {
        throw string("Call failed to connect");
    }
}


//======================================================================================================================
// Tell voxip to make audio call. If it fails, create error message, kill external processes and return false, o/w return true
// If hasTraceId = false (default value) nothing is done, if it is true the JSON value wil be checked to make sure it is present.
//======================================================================================================================
void CallTest::audio_call(bool hasTraceId)
{
    VOIPManager::Inst()->MakeCall(CALL_DEST_1, CALL_UUID_1, false);
    VOIPManager::Inst()->MoveCallToUiFront(CALL_UUID_1, true); //todo: what is this?
    
    string data;
    if(!TestsEnv::inst()->wait_for_event(CallState_Connected, 20, &data))
    {
        throw string("Audio Call Failed to Connect");
    }

    if(hasTraceId)
    {
        string traceId1 = TRACE_ID;
        string traceId2 = get_json_value(data, "headers", "X-HDAP-TraceId");
      
        if(traceId1!= traceId2)
        {
            string err = "Expected X-HDAP TraceID : " + traceId1 + " in event CallState_Connected data but received X-HDAP TraceID : " + traceId2;
            throw err ;
        }
    }
}

//======================================================================================================================
// Tell voxip to make audio call. Check all states of the call not just connected.
//  If it fails, create error message, kill external processes and return false, o/w return true
//======================================================================================================================
void CallTest::audio_call_all_states()
{
    VOIPManager::Inst()->MakeCall(CALL_DEST_1, CALL_UUID_1, false);
    VOIPManager::Inst()->MoveCallToUiFront(CALL_UUID_1, true); //todo: what is this?
    
    if(!TestsEnv::inst()->wait_for_event(CallState_Calling, 20))
    {
        throw string("Calling State not Reached");
    }
    
    if(!TestsEnv::inst()->wait_for_event(CallState_First_Active_Calls, 20))
    {
        throw string("First Call State Not Received");
    }
    
    if(!TestsEnv::inst()->wait_for_event(CallState_Ringing, 20))
    {
        throw string("Audio Call Failed to Connect");
    }
    
    if(!TestsEnv::inst()->wait_for_event(CallState_RingtoneChange, 20))
    {
        throw string("Ringtone Change did not occur.");
    }
    
    if(!TestsEnv::inst()->wait_for_event(CallState_Media_Stream_Change, 20))
    {
        throw string("Media State Change Not Reached");
    }
    
    if(!TestsEnv::inst()->wait_for_event(CallState_Connected, 20))
    {
        throw string("Audio Call Failed to Connect");
    }
}

//======================================================================================================================
// Tell voxip to make audio call that will not be answered. If it fails, create error message, kill external processes and return false, o/w return true
//======================================================================================================================
void CallTest::audio_call_without_answer()
{
    VOIPManager::Inst()->MakeCall(CALL_DEST_1, CALL_UUID_1, false);
    VOIPManager::Inst()->MoveCallToUiFront(CALL_UUID_1, true); //todo: what is this?
    
    if(!TestsEnv::inst()->wait_for_event(CallState_Ringing, 20))
    {
        throw string("Other side did not ring on invite.");
    }
}

//======================================================================================================================
//======================================================================================================================
void CallTest::reject_call()
{
    string data;

    // Wait for incoming call
    if(!TestsEnv::inst()->wait_for_event(CallState_AlertIncoming, 10, &data))
    {
        throw string("Incoming Call Not Received");
    }
    
    // Extract call-id and reject the call
    string call_id1 = get_json_value(data, "CID");
    VOIPManager::Inst()->Hangup(call_id1.c_str());
    
    // When rejecting a call that was never connected, the idle event will be emitted and not the hangup event
    if(!TestsEnv::inst()->wait_for_event(CallState_Idle, 10, &data))
    {
        throw string("Idle event didn't happen");
    }

    string call_id2 = get_json_value(data, "CID");
    
    if(call_id2 != call_id1)
    {
        string msg = "Expected idle event for call-id " + call_id1 + " but received idle event for call-id " + call_id2;
        throw string(msg);
    }
}


//======================================================================================================================
// Tell voxip to video make call. If it fails, create error message, kill external processes and return false, o/w return true
//======================================================================================================================
void CallTest::video_call()
{
    VOIPManager::Inst()->MakeCall(CALL_DEST_1, CALL_UUID_1, true);
    VOIPManager::Inst()->MoveCallToUiFront(CALL_UUID_1, true); //todo: what is this?
    
    if(!TestsEnv::inst()->wait_for_event(CallState_Connected, 20))
    {
        throw string("Video call failed to Connect");
    }
}

//======================================================================================================================
// Tell voxip to hangup call
// If hasTraceId = false (default value) nothing is done, if it is true the JSON value wil be checked to make sure it is present.
//======================================================================================================================
void CallTest::hangup(string call_id, bool hasTraceId)
{
    VOIPManager::Inst()->Hangup(call_id.c_str());
    
    string data;
    
    if(!TestsEnv::inst()->wait_for_event(CallState_Hangup, 10, &data))
    {
        throw string("Call failed to disconnect");
    }
    
    if(hasTraceId)
    {
        string traceId1 = TRACE_ID;
        string traceId2 = get_json_value(data, "headers", "X-HDAP-TraceId");
        
        if(traceId1 != traceId2)
        {
            string err = "Expected X-HDAP TraceID : " + traceId1 + " in event CallState_Hangup data but received X-HDAP TraceID : " + traceId2;
            throw err ;
        }
    }
    
    if(!are_active_calls())
    {
        if(!TestsEnv::inst()->wait_for_event(CallState_No_Active_Calls, 10))
        {
            throw string("Call still active");
        }
    }
}

//======================================================================================================================
// When VoXIP received by from SIPP This will Confirm Hang Up Occured.
//If it fails, create error message, kill external processes and return false, o/w return true
//======================================================================================================================
void CallTest::confirm_remote_hangup(bool hasTraceId)
{
    if(!TestsEnv::inst()->wait_for_event(CallState_Idle, 10))
    {
        throw string("Call did not return to Idle State");
    }
    
    string data;
    
    if(!TestsEnv::inst()->wait_for_event(CallState_Hangup, 10, &data))
    {
        throw string("Call failed to hang up");
    }
    
    if(hasTraceId)
    {
        string traceId1 = TRACE_ID;
        string traceId2 = get_json_value(data, "headers", "X-HDAP-TraceId");
        
        if(traceId1 != traceId2)
        {
            string err = "Expected X-HDAP TraceID : " + traceId1 + " in event CallState_Hangup data but received X-HDAP TraceID : " + traceId2;
            throw err ;
        }
    }
    
    if(!TestsEnv::inst()->wait_for_event(CallState_Media_Stream_Closed, 10))
    {
        throw string("Call failed to end media stream");
    }
}

//======================================================================================================================
// Test all Invite Error Code Response Scenarios where the app will tear down the call and not perform any additional
// actions.
//======================================================================================================================
void CallTest::test_invite_fail(string error_code, EVoXIPUIEvent event)
{
    run_mserver_scenario("invite_fail.scr", "error_code=" + quote(error_code));
    register_voxip();
    VOIPManager::Inst()->MakeCall(CALL_DEST_1, CALL_UUID_1, false);
    
    if(!TestsEnv::inst()->wait_for_event(event, 5))
    {
        throw string("Event " + VoXIPUIEvents::GetUICallbackEventString(event) + " didn't occur after " + error_code);
    }
    
    if(!TestsEnv::inst()->wait_for_event(CallState_Hangup, 5))
    {
        throw string("Event CallState_Hangup didn't occur after " + error_code);
    }
}

//======================================================================================================================
// Enter Local Hold State - User Initiated
//======================================================================================================================
void CallTest::local_hold(string call_id)
{
    check_call_connected(call_id);
    VOIPManager::Inst()->Hold(call_id.c_str(), true, true);
    
    if(!TestsEnv::inst()->wait_for_event(CallState_OnHold, 10))
    {
        throw string("Call was not placed on hold");
    }
}

//======================================================================================================================
// End Local Hold State - User Initiated
//======================================================================================================================
void CallTest::local_unhold(string call_id)
{
    check_call_connected(call_id);
    VOIPManager::Inst()->Hold(call_id.c_str(), false, true);
    
    // Check that the event occurs and that its data contain the value "unhold" for field "Param"
    string data;

    if(!TestsEnv::inst()->wait_for_event(CallState_Connected, 10, &data))
    {
        throw string("Call was not taken off hold");
    }
    
    check_json_value(data, "Param", "unhold", "CallState_Connected");
}

//======================================================================================================================
// Expect remote hold
//======================================================================================================================
void CallTest::expect_remote_hold()
{
    if(!TestsEnv::inst()->wait_for_event(CallState_RemoteHold, 10))
    {
        throw string("Remote hold not reached");
    }
}

//======================================================================================================================
// Expect remote unhold. Check that the event occurs and that its data contain the value "unhold" for field "Param"
//======================================================================================================================
void CallTest::expect_remote_unhold(string call_id)
{
    Call *call = check_call_connected(call_id);
    
    if(!call->remote_hold)
    {
        throw string("Can't expect remote unhold if call is not on remote hold!");
    }
    
    string data;

    if(!TestsEnv::inst()->wait_for_event(CallState_Connected, 10, &data))
    {
        throw string("Call was not taken off remote hold");
    }
    
    check_json_value(data, "Param", "unhold", "CallState_Connected");
}

//======================================================================================================================
// Expect remote unhold while on local hold, indicated by receiving the OnHold event again, with Param = remoteUnhold.
//======================================================================================================================
void CallTest::expect_remote_unhold_during_local_hold(string call_id)
{
    Call *call = check_call_connected(call_id);
    
    if(!call->remote_hold || (!call->local_hold && !call->gsm_hold))
    {
        throw string("Can't expect remote unhold during local hold if both holds aren't on!");
    }
    
    string data;
    
    if(!TestsEnv::inst()->wait_for_event(CallState_OnHold, 10, &data))
    {
        throw string("Call was not taken off remote hold");
    }
    
    check_json_value(data, "Param", "remoteUnhold", "CallState_OnHold");
}


//======================================================================================================================
// Enter Local Hold State - GSM or Siri Initiated, GSM = True otherwise uses Siri
// If Call Active = True Vonage Call is Active otherwise it is not.
//======================================================================================================================
void CallTest::gsm_hold(bool GSM)
{
    if(GSM == true)
    {
        VxConnectivity::Inst()->NativeCall()->setValue(1);
    }
    else
    {
        VxDeviceState::Inst()->AudioSession()->setValue(1);
    }
    
    if(are_active_calls())
    {
        if(!TestsEnv::inst()->wait_for_event(CallState_OnHold_GSM, 10))
        {
            throw string("Call was not placed on gsm hold");
        }
    }
}


//======================================================================================================================
// End Local Hold State - GSM or Siri Initiated, GSM = True otherwise uses Siri
//======================================================================================================================
void CallTest::gsm_unhold(bool GSM, bool expect_connected, string call_id)
{
    if(GSM == true)
    {
        VxConnectivity::Inst()->NativeCall()->setValue(0);
    }
    else
    {
        VxDeviceState::Inst()->AudioSession()->setValue(0);
    }
    
    if(! are_active_calls())
    {
        return;
    }
    
    if(!TestsEnv::inst()->wait_for_event(CallState_GSM_End, 10))
    {
        throw string("Call was not taken off gsm hold");
    }
    
    VOIPManager::Inst()->Hold(call_id.c_str(), false, true); // Unhold
    
    if(expect_connected)
    {
        // Check that the event occurs and that its data contain the value "unhold" for field "Param"
        string data;
        
        if(!TestsEnv::inst()->wait_for_event(CallState_Connected, 10, &data))
        {
            throw string("Call was not taken off hold after gsm ends");
        }
        
        check_json_value(data, "Param", "unhold", "CallState_Connected");
    }
}


//======================================================================================================================
// Tell voxip to make call, places call on hold, takes it off hold and if successful hangup.
// If Hold Type = 1 it is manual hold/unhold
// If Hold Type = 2 it is GSM hold/unhold
// If Hold Type = 3 it is Siri hold/unhold
// If it fails, create error message, kill external processes and return false, o/w return true
// Will make Audio Call if False and Video Call if True
//======================================================================================================================
void CallTest::call_and_hangup_with_local_hold_unhold(bool do_video_call, int hold_type)
{
    if(do_video_call)
    {
        video_call();
    }
    else
    {
        audio_call();
    }
    
    //run local hold initiated by user
    if (hold_type == 1)
    {
        local_hold();
        local_unhold();
    }
    else if (hold_type == 2)  //run local hold initiated by GSM interrupt
    {
        gsm_hold(true);
        gsm_unhold(true);
    }
    else  //run local hold initiated by GSM interrupt
    {
        gsm_hold(false);
        gsm_unhold(false);
    }
        
    hangup();
}

//==============================================================================================
//This function will try to place a call while there is a GSM Call active
//The call should not succeed
//==============================================================================================
void CallTest::attempt_call_on_hold ()
{
    gsm_hold(true);
    VOIPManager::Inst()->MakeCall(CALL_DEST_1, CALL_UUID_1, false);
    
    if(!TestsEnv::inst()->wait_for_event(CallState_Idle_GSM, 10))
    {
        throw string("GSM interrupt not seen");
    }
    
    if(!TestsEnv::inst()->wait_for_event(CallState_Hangup, 10))
    {
        throw string("Call failed to hang up");
    }
    
    gsm_unhold(true, false);
}

//==============================================================================================
// This function will try to receive a call while there is a GSM Call active
// The call will be rejected automatically and then hold will be ended.
//==============================================================================================
void CallTest::receive_call_on_hold()
{
    gsm_hold(true);
    
    if(!TestsEnv::inst()->wait_for_event(CallState_No_Active_Calls, 10))
    {
        throw string("Expected event CallState_No_Active_Calls after incoming call during GSM hold");
    }
    
    gsm_unhold(true, false);
}

//======================================================================================================================
// Tell voxip to make call, places call on hold, and end call while on hold.
// If Hold Type = 1 it is manual hold/unhold
// If Hold Type = 2 it is GSM hold/unhold
// If Hold Type = 3 it is Siri hold/unhold
// If it fails, create error message, kill external processes and return false, o/w return true
// Will make Audio Call if False and Video Call if True
//======================================================================================================================
void CallTest::call_and_hangup_while_on_local_hold(bool do_video_call, int hold_type)
{
    if(do_video_call)
    {
        video_call();
    }
    else
    {
        audio_call();
    }
    
    if (hold_type == 1)   //run local hold initiated by user
    {
        local_hold();
    }
    else if (hold_type == 2)   //run local hold initiated by GSM interrupt
    {
        gsm_hold(true);
    }
    else  //run local hold initiated by SIRI interrupt
    {
        gsm_hold(false);
    }
    
    hangup();
}

//======================================================================================================================
// Tell voxip to make call, is placed on hold, is taken off hold and if successful hangup.
// If it fails, create error message, kill external processes and return false, o/w return true
// Will make Audio Call if False and Video Call if True
//======================================================================================================================
void CallTest::call_and_hangup_with_remote_hold(bool do_video_call, bool unhold)
{
    if(do_video_call)
    {
        video_call();
    }
    else
    {
        audio_call();
    }
    
    expect_remote_hold();
    
    if(unhold == true)
    {
        expect_remote_unhold();
    }
    
    hangup();
}

//======================================================================================================================
// Tell voxip to make call, place call on hold, take call off hold
// call is placed on hold, is taken off hold and if successful hangup.
// If it fails, create error message, kill external processes and return false, o/w return true
// Will make Audio Call if False and Video Call if True
// If Local First is true it performs local hold first, otherwise remote.
//======================================================================================================================
void CallTest::call_and_hangup_with_sequential_hold(bool do_video_call, bool local_first)
{
    if(do_video_call)
    {
        video_call();
    }
    else
    {
        audio_call();
    }
    
    if(local_first == true)
    {
        gsm_hold(true);
        gsm_unhold(true);
        expect_remote_hold();
        expect_remote_unhold();
        hangup();
    }
    else
    {
        expect_remote_hold();
        expect_remote_unhold();
        gsm_hold(true);
        gsm_unhold(true);
        hangup();
    }
}

//======================================================================================================================
// Tell voxip to make call, call will then enter remote and local hold at same time depending on enter_local_first value.
// Call will then have holds end based on end_local_first value.
// Call is disconnected following holds being ended successfully.
//======================================================================================================================
void CallTest::call_and_hangup_with_parallel_hold(bool enter_local_first, bool end_local_first)
{
    audio_call();
    
    //Enter Local Hold First Followed by Remote
    if(enter_local_first)
    {
        gsm_hold(true);
        expect_remote_hold();
    }
    else //Enter Remote Hold First Followed by Remote
    {
        expect_remote_hold();
        gsm_hold(true);
    }
    
    //End Local Hold First
    if(end_local_first)
    {
        gsm_unhold(true, false);
        
        //Confirm Call Remains in Remote Hold
        if(!TestsEnv::inst()->wait_for_event(CallState_RemoteHold, 10))
        {
            throw string("Expected remote hold event after GSM unhold during remote hold");
        }
        
        expect_remote_unhold();
    }
    else // End Remote Hold First
    {
        expect_remote_unhold_during_local_hold();
        gsm_unhold(true);
    }
    
    hangup();
}


//======================================================================================================================
// Toggle current mute state for given call-id, with the expected action indicated by the 'on' param:
// true: enable mute
// false: disable mute
// An error is issued if the operation can't be executed as expected
//======================================================================================================================
void CallTest::toggle_mute(string call_id, bool on)
{
    Call *call = check_call_connected(call_id);
    string action = (on ? "enabled" : "disabled");
    
    if(call == nullptr)
    {
        throw string("Trying to mute with an invalid call_id " + call_id);
    }
    else if(call->mutted == on)
    {
        throw string("Mute is already " + action + " for call " + call_id);
    }

    string data;
    EVoXIPUIEvent event = (on ? CallState_MuteOn : CallState_MuteOff);
    
    VOIPManager::Inst()->MuteToggle(call_id.c_str());
    
    if(!TestsEnv::inst()->wait_for_event(event, 10, &data))
    {
        throw string("Mute not " + action);
    }
    
    if(get_json_value(data, "CID") != call_id)
    {
        throw string("Mute " + action + " for the wrong call");
    }
    
    call->toggle_mute();
    
} // toggle_mute()


//======================================================================================================================
// Tell VoXIP to Toggle Mute On for a Specific Call ID
//======================================================================================================================
void CallTest::enable_mute(string call_id)
{
    toggle_mute(call_id, true);
}


//======================================================================================================================
// Tell VoXIP to Toggle Mute Off for a Specific Call ID
//======================================================================================================================
void CallTest::disable_mute(string call_id)
{
    toggle_mute(call_id, false);
}



//======================================================================================================================
//======================================================================================================================
string CallTest::wait_for_incoming_call(int timeout, bool has_trace_id)
{
    string data;
    
    if(!TestsEnv::inst()->wait_for_event(CallState_AlertIncoming, timeout, &data))
    {
        throw string("Call Not received");
    }
    
    if(has_trace_id)
    {
        check_trace_id(data);
    }
    
    return get_json_value(data, "CID");
}


//======================================================================================================================
// Wait for incoming call and return its call id
// If hasTraceId = false (default value) nothing is done, if it is true the JSON value wil be checked to make sure it is present.
//======================================================================================================================
string CallTest::wait_for_incoming_call(bool has_trace_id)
{
    string data;
    
    if(!TestsEnv::inst()->wait_for_event(CallState_AlertIncoming, 20, &data))
    {
        throw string("Call Not received");
    }

    if(has_trace_id)
    {
        check_trace_id(data);
    }
    
    return get_json_value(data, "CID");
}


//======================================================================================================================
//======================================================================================================================
void CallTest::check_trace_id(string& data)
{
    string traceId1 = TRACE_ID;
    string traceId2 = get_json_value(data, "headers", "X-HDAP-TraceId");
    
    if(traceId1 != traceId2)
    {
        throw string("Expected X-HDAP TraceID : " + traceId1 + " in event CallState_AlertIncoming data but received X-HDAP TraceID : " + traceId2);
    }
}


//======================================================================================================================
// Tell voxip to answer audio call for a specific call ID. If it fails, create error message, kill external processes and return false, o/w return true
// If hasTraceId = false (default value) nothing is done, if it is true the JSON value wil be checked to make sure it is present.
//======================================================================================================================
void CallTest::answer_call(string call_id, bool hasTraceId)
{
    if(call_id.empty())
    {
        throw string("Call ID is Empty");
    }
    
    VOIPManager::Inst()->Answer(call_id.c_str());
    VOIPManager::Inst()->MoveCallToUiFront(call_id.c_str(), true);
    
    string data;
    
    if(!TestsEnv::inst()->wait_for_event(CallState_Connected, 20, &data))
    {
        throw string("Call Failed to be Answered");
    }
    
    if(hasTraceId)
    {
        string traceId1 = TRACE_ID;
        string traceId2 = get_json_value(data, "headers", "X-HDAP-TraceId");
        
        if(traceId1 != traceId2)
        {
            string err = "Expected X-HDAP TraceID : " + traceId1 + " in event CallState_Connected data but received X-HDAP TraceID : " + traceId2;
            throw err ;
        }
    }
}


//======================================================================================================================
// Expect the an ars-message event, with the given termination reason
//======================================================================================================================
void CallTest::expect_termination_reason(AppRejectReason reason, int timeout)
{
    string data;
    
    if(!TestsEnv::inst()->wait_for_event(MessageState_SendArsMessage, timeout, &data))
    {
        throw string("Event MessageState_SendArsMessage didn't happen");
    }
    
    AppRejectReason recv_reason;
    string termination_reason;
    
    try
    {
        termination_reason = get_json_value(data, "TERMINATIONREASON");
        recv_reason = (AppRejectReason) stoi(termination_reason);
    }
    catch(invalid_argument& ia) // This exception might be thrown by stoi()
    {
        throw string("stoi() failed for argument: \"" + termination_reason + "\""); // Will be caught in TestBody()
    }
    
    if(reason != recv_reason)
    {
        throw string("Expected termination reason to be " +
                     toStringLog<AppRejectReason>(reason) + "(=" + to_string(reason) + ") but received " +
                     toStringLog<AppRejectReason>(recv_reason)) + "(=" + to_string(recv_reason) + ")";
    }
}









