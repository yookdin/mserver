//
//  CallWaiting.cpp
//  VoxipBBTests
//
//  Contains all test containing a Call Waiting Scenario
//  Naming convention of the tests:
//  cw_<out|in>_<out|in>_<Answer_End|Answer_Hold>
//  o cw:  call waiting
//  o out|in: was call outbound or inbound in respect to VoXIP
//  o Answer_End|Answer_Hold:  did user select Answer+End or Answer+Hold
//  Created by Yuval Dinari on 7/28/15.
//  Copyright (c) 2015 Yuval Dinari. All rights reserved.

#include "CallTest.h"

//All Call Waiting Specific API Call Functions

//======================================================================================================================
// Tell voxip to hangup active call, while second call is on hold
//======================================================================================================================
void CallTest::hangup_active(string call_id)
{
    VOIPManager::Inst()->Hangup(call_id.c_str());
    
    if(!TestsEnv::inst()->wait_for_event(CallState_Hangup, 10))
    {
        throw string("Primary Call failed to hang up");
    }
}


//======================================================================================================================
// VoXIP Received Inbound call.  Puts Active Call On Hold and then Answers the Inbound Call.
// Calls will be swapped back and forth depending on number of swaps specified, 0-2 are supported.
//======================================================================================================================
void CallTest::call_waiting_answer_and_hold_out_in(int swaps)
{
    string call_id = wait_for_incoming_call();
    hold_and_answer(CALL_UUID_1, call_id);
    
    if(swaps == 0)
    {
        hangup_both(call_id, CALL_UUID_1);
    }
    else if (swaps == 1)
    {
        swap(call_id, CALL_UUID_1);
        hangup_both(CALL_UUID_1, call_id);
    }
    else if (swaps == 2)
    {
        swap(call_id, CALL_UUID_1);
        swap(CALL_UUID_1, call_id);
        hangup_both(call_id, CALL_UUID_1);
    }
}


//======================================================================================================================
//Function to Mirror Answer + End Behavior.
//Has two strings as arguements, active_call will be Ended, incoming_call will be answered
//======================================================================================================================
void CallTest::end_and_answer(string active_call, string incoming_call)
{
    hangup(active_call);
    answer_call(incoming_call);
}

//======================================================================================================================
//Function to Mirror Answer + Hold Behavior.
//Has two strings as arguements:  active_call will be put on Hold, incoming_call will be answered
//======================================================================================================================
void CallTest::hold_and_answer(string active_call, string incoming_call)
{
    local_hold(active_call);
    answer_call(incoming_call);
}

//======================================================================================================================
// Function to Mirror Swap Behavior.
// Has two strings as arguements, active_call will be put on Hold, hold_call will be made active
//======================================================================================================================
void CallTest::swap(string active_call, string hold_call)
{
    local_hold(active_call);
    local_unhold(hold_call);
}

//======================================================================================================================
//Function to Hang Up both Active and On Hold Call
//Arguements:  active_call will be put on ended, hold_call will be made active then ended
//======================================================================================================================
void CallTest::hangup_both(string active_call, string hold_call)
{
    hangup(active_call);
    local_unhold(hold_call);
    hangup(hold_call);
}

//======================================================================================================================
// While in call, receive another call and reject it.
//======================================================================================================================
BB_TEST_F(CallTest, CW_out_in_Decline_Second_Call)
{
    run_mserver_scenario("cw_out_in_decline_second_call.scr");
    register_voxip();
    audio_call();
    reject_call();
}


//======================================================================================================================
// While in call, receive another call and have it be canceled before you answer.
//======================================================================================================================
BB_TEST_F(CallTest, CW_out_in_Second_Call_Canceled)
{
    run_mserver_scenario("cw_out_in_second_call_canceled.scr");
    register_voxip();
    audio_call();
    confirm_remote_hangup();
}


//======================================================================================================================
//Receive a incoming call prior to the initial outbound call being connected.  VoXIP should return Busy Reply.
//======================================================================================================================
BB_TEST_F(CallTest, CW_out_in_Second_Call_Before_First_Answered)
{
    run_mserver_scenario("cw_out_in_second_call_before_first_answered.scr");
    register_voxip();
    audio_call_without_answer();
    expect_termination_reason(app_reason_already_in_a_call);
}


//======================================================================================================================
//Call Waiting Test - Starts with outbound call followed by Inbound Call.  User Selects Answer + End.
//======================================================================================================================
BB_TEST_F(CallTest, CW_out_in_Answer_End)
{
    run_mserver_scenario("cw_out_in_answer_end.scr");
    register_voxip();
    audio_call();
    
    // VoXIP Received Inbound call. Ends the active call and then Answers the Inbound Call.
    string call_id = wait_for_incoming_call();
    end_and_answer(CALL_UUID_1, call_id);
    hangup(call_id);
}


//======================================================================================================================
//Call Waiting Test - Starts with outbound call followed by Inbound Call.  User Selects Answer + Hold.
//User Ends Call Waiting Call, then returns to Original Call and Ends It.
//======================================================================================================================
BB_TEST_F(CallTest, CW_out_in_Hold_Answer_No_Swap)
{
    run_mserver_scenario("cw_out_in_hold_answer_no_swap.scr");
    register_voxip();
    audio_call();
    call_waiting_answer_and_hold_out_in(0);
}


//======================================================================================================================
//Call Waiting Test - Starts with outbound call followed by Inbound Call.  User Selects Answer + Hold.
//User swaps to original call and ends it and returns to second call and ends it.
//======================================================================================================================
BB_TEST_F(CallTest, CW_out_in_Hold_Answer_One_Swap)
{
    run_mserver_scenario("cw_out_in_hold_answer_one_swap.scr");
    register_voxip();
    audio_call();
    call_waiting_answer_and_hold_out_in(1);
}


//======================================================================================================================
//Call Waiting Test - Starts with outbound call followed by Inbound Call.  User Selects Answer + Hold.
//User swaps to original call, swaps back to the call waiting call and ends it and returns to primary call and ends it.
//======================================================================================================================
BB_TEST_F(CallTest, CW_out_in_Hold_Answer_Two_Swaps)
{
    run_mserver_scenario("cw_out_in_hold_answer_two_swaps.scr");
    register_voxip();
    audio_call();
    call_waiting_answer_and_hold_out_in(2);
}


//======================================================================================================================
//Call Waiting Test - Starts with outbound call followed by Inbound Call.  User Selects Answer + Hold.
//Have Mute enabled on Primary call leg first and then on secondary call leg.
//User swaps to original call, swaps back to the call waiting call and ends it and returns to primary call and ends it.
//======================================================================================================================
BB_TEST_F(CallTest, CW_out_in_Hold_Answer_Mute_Outbound_First)
{
    run_mserver_scenario("cw_out_in_hold_answer_two_swaps.scr");
    register_voxip();
    audio_call();
    enable_mute(CALL_UUID_1);
    string call_id = wait_for_incoming_call();
    hold_and_answer(CALL_UUID_1, call_id);  // Place first call on hold, answer second call
    enable_mute(call_id);
    swap(call_id, CALL_UUID_1);             // Place second on hold, unhold first call
    disable_mute(CALL_UUID_1);
    swap(CALL_UUID_1, call_id);             // Place first call on hold, unhold second call
    disable_mute(call_id);
    
    hangup_both(call_id,CALL_UUID_1);
}


//======================================================================================================================
//Call Waiting Test - Starts with outbound call followed by Inbound Call.  User Selects Answer + Hold.
//Have Mute enabled on Secondary call leg first and then on primary call leg.
//User swaps to original call, swaps back to the call waiting call and ends it and returns to primary call and ends it.
//======================================================================================================================
BB_TEST_F(CallTest, CW_out_in_Hold_Answer_Mute_Inbound_First)
{
    run_mserver_scenario("cw_out_in_hold_answer_two_swaps.scr");
    register_voxip();
    audio_call();
    string call_id = wait_for_incoming_call();
    hold_and_answer(CALL_UUID_1, call_id);  // Place first call on hold, answer second call
    enable_mute(call_id);
    swap(call_id, CALL_UUID_1);     // Place second on hold, unhold first call
    enable_mute(CALL_UUID_1);
    swap(CALL_UUID_1, call_id);     // Place first call on hold, unhold second call
    disable_mute(call_id);
    hangup(call_id);                // End second call
    local_unhold();                 // Take first call off hold
    disable_mute(CALL_UUID_1);
    hangup();                       // End first call
}


//======================================================================================================================
// While in Inbound call, receive another call and reject it.
//======================================================================================================================
BB_TEST_F(CallTest, CW_In_In_Decline_Second_Call)
{
    run_mserver_scenario("cw_in_in_decline_second_call.scr");
    register_voxip();
    string call1 = wait_for_incoming_call();
    answer_call(call1);
    reject_call();
    expect_termination_reason(app_reason_call_rejected_by_user);
}


//======================================================================================================================
// While in Inbound call, receive another call, end the current call and answer the new call
//======================================================================================================================
BB_TEST_F(CallTest, CW_In_In_Answer_End)
{
    run_mserver_scenario("cw_in_in_answer_end.scr");
    register_voxip();
    string call1 = wait_for_incoming_call();
    answer_call(call1);
    string call2 = wait_for_incoming_call();
    end_and_answer(call1, call2);
    hangup(call2);
}


//======================================================================================================================
// While in Inbound call, receive another call, put first call on hold, answer new call
// end new call, return to first call and end it.
//======================================================================================================================
BB_TEST_F(CallTest, CW_In_In_Hold_Answer_No_Swap)
{
    run_mserver_scenario("cw_in_in_hold_answer_no_swap.scr");
    register_voxip();
    string call1 = wait_for_incoming_call();
    answer_call(call1);
    string call2 = wait_for_incoming_call();
    hold_and_answer(call1, call2);
    hangup_both(call2, call1);
}


//======================================================================================================================
//======================================================================================================================
BB_TEST_F(CallTest, CW_In_In_Hold_Answer_One_Swap)
{
    run_mserver_scenario("cw_in_in_hold_answer_one_swap.scr");
    register_voxip();
    string call1 = wait_for_incoming_call();
    answer_call(call1);
    string call2 = wait_for_incoming_call();
    hold_and_answer(call1, call2);
    swap(call2, call1);
    hangup_both(call1, call2);
}


//======================================================================================================================
//======================================================================================================================
BB_TEST_F(CallTest, CW_In_In_Hold_Answer_Two_Swaps)
{
    run_mserver_scenario("cw_in_in_hold_answer_two_swaps.scr");
    register_voxip();
    string call1 = wait_for_incoming_call();
    answer_call(call1);
    string call2 = wait_for_incoming_call();
    hold_and_answer(call1, call2);
    swap(call2, call1);
    swap(call1, call2);
    hangup_both(call2, call1);
}


//======================================================================================================================
//Call Waiting Test - Starts with outbound call followed by Inbound Call.  User Selects Answer + Hold.
//A third call comes in and is rejected by App with 486.  End all Calls.
//======================================================================================================================
BB_TEST_F(CallTest, CW_out_in_Third_Call_Rejected)
{
    run_mserver_scenario("cw_out_in_third_call_rejected.scr");
    register_voxip();
    audio_call();
    string call_id;
    call_id = wait_for_incoming_call();
    hold_and_answer(CALL_UUID_1, call_id);
    local_hold(call_id);
    local_unhold();
    hangup_both(CALL_UUID_1, call_id);
}




/*TO DO CALL WAITING TEST
 1.  One side video/one side audio - with swap
 2.  Initial call on hold when call waiting call is received
 3.  GSM Hold during Call Waiting Scenario
 4.  Receive Remote Bye for Active Call --> Return to On Hold Call --> Confirm a new call waiting call can be received
 5.  Receive Remote Bye for Hold Call --> Confirm a new call waiting call can be received
 
 */


