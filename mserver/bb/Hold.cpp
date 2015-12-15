//
//  Hold.cpp
//
//  Created by Brian Pante on 7/28/15.
//
//

#include "CallTest.h"

//==================================================================================================
// Audio Call With Local Hold:
// register, make call, wait for connection, enter local hold, end local hold, hangup
//==================================================================================================
BB_TEST_F(CallTest, AudioCallWithLocalHold)
{
    run_mserver_scenario("audio_call_with_local_hold.scr");
    register_voxip();
    call_and_hangup_with_local_hold_unhold(false, 1);
    unregister_voxip();
}

//==================================================================================================
// Audio Call With GSM Local Hold:
// register, make call, wait for connection, enter local hold due to GSM, end local hold, hangup
//==================================================================================================
BB_TEST_F(CallTest, AudioCallWithGSMLocalHold)
{
    run_mserver_scenario("audio_call_with_local_hold.scr");
    register_voxip();
    call_and_hangup_with_local_hold_unhold(false, 2);
    unregister_voxip();
}


//==================================================================================================
// Audio Call With Siri Local Hold:
// register, make call, wait for connection, enter local hold due to GSM, end local hold, hangup
//==================================================================================================
BB_TEST_F(CallTest, AudioCallWithSiriLocalHold)
{
    run_mserver_scenario("audio_call_with_local_hold.scr");
    register_voxip();
    call_and_hangup_with_local_hold_unhold(false, 3);
    unregister_voxip();
}


//==================================================================================================
//Attempt Call With GSM Active:
//Register, Receive GSM Event, Attempt Call and have it Fail, End GSM Event, Attempt new call and it succeeds
//==================================================================================================
BB_TEST_F(CallTest, AttemptCallWithGSMActive)
{
    run_mserver_scenario("complete_audio_call.scr");
    register_voxip();
    attempt_call_on_hold();
    call_and_hangup(false);
    unregister_voxip();    
}


//==================================================================================================
// Audio Call Ended During GSM Local Hold:
// register, make call, wait for connection, enter local hold due to GSM, hangup
//==================================================================================================
BB_TEST_F(CallTest, AudioCallEndedDuringGSMLocalHold)
{
    run_mserver_scenario("audio_call_end_during_hold.scr");
    register_voxip();
    call_and_hangup_while_on_local_hold(false, 2);
    unregister_voxip();
}


//==================================================================================================
// Audio Call With Remote Hold:
// register, make call, wait for connection, enter remote hold, end remote hold, hangup
//==================================================================================================
BB_TEST_F(CallTest, AudioCallWithRemoteHold)
{
    run_mserver_scenario("audio_call_with_remote_hold.scr");
    register_voxip();
    call_and_hangup_with_remote_hold(false, true);
    unregister_voxip();
}


//==================================================================================================
// AudioCallEndDuringRemoteHold:
// register, make call, wait for connection, enter remote hold, hangup
//==================================================================================================
BB_TEST_F(CallTest, AudioCallEndDuringRemoteHold)
{
    run_mserver_scenario("audio_call_end_during_remote_hold.scr");
    register_voxip();
    call_and_hangup_with_remote_hold(false, false);
    unregister_voxip();
}


//==================================================================================================
// Recieve Call With GSM Active:
// Register, Receive GSM Event, Recive Call and Reject, End GSM Event, Receive new call, Ansswer and Hang Up
//==================================================================================================
BB_TEST_F(CallTest, ReceiveCallWithGSMActive)
{
    run_mserver_scenario("receive_call_with_gsm_active.scr");
    register_voxip();
    receive_call_on_hold();
    expect_termination_reason(app_reason_call_rejected_since_now_in_gsm_call);
    answer_call_and_hangup();
}


//==================================================================================================
// Audio Call With Local then Remote Hold:
// register, make call, wait for connection, enter local hold, end local hold,
// enter remote hold, end remote hold, hangup
//==================================================================================================
BB_TEST_F(CallTest, AudioCallWithLocalThenRemoteHold)
{
    run_mserver_scenario("audio_call_with_local_then_remote_hold.scr");
    register_voxip();
    call_and_hangup_with_sequential_hold(false, true);
    unregister_voxip();
}


//==================================================================================================
// Audio Call With Remote then Local Hold:
// register, make call, wait for connection, enter remote hold, end remote hold,
// enter local hold, end local hold, hangup
//==================================================================================================
BB_TEST_F(CallTest, AudioCallWithRemoteThenLocalHold)
{
    run_mserver_scenario("audio_call_with_remote_then_local_hold.scr");
    register_voxip();
    call_and_hangup_with_sequential_hold(false, false);
    unregister_voxip();
}


//==================================================================================================
// Audio Call With Local Hold Remote Hold At Same Time End Local First:
// register, make call, wait for connection, enter local hold, enter remote hold,
// end local hold, end remote hold, hang up.
//==================================================================================================
BB_TEST_F(CallTest, AudioCallWithLocalHoldRemoteHoldAtSameTimeEndLocalFirst)
{
    run_mserver_scenario("audio_call_with_local_hold_remote_hold_at_same_time_end_local_first.scr");
    register_voxip();
    call_and_hangup_with_parallel_hold(true, true);
    unregister_voxip();
}


//==================================================================================================
// Audio Call With Local Hold Remote Hold At Same Time End Remote First:
// register, make call, wait for connection, enter local hold, enter remote hold,
// end remote hold, end local hold, hang up.
//==================================================================================================
BB_TEST_F(CallTest, AudioCallWithLocalHoldRemoteHoldAtSameTimeEndRemoteFirst)
{
    run_mserver_scenario("audio_call_with_local_hold_remote_hold_at_same_time_end_remote_first.scr");
    register_voxip();
    call_and_hangup_with_parallel_hold(true, false);
    unregister_voxip();
}


//==================================================================================================
// Audio Call With Remote Hold Local Hold At Same Time End Local First:
// register, make call, wait for connection, enter remote hold, enter local hold,
// end local hold, end remote hold, hang up.
//==================================================================================================
BB_TEST_F(CallTest, AudioCallWithRemoteHoldLocalHoldAtSameTimeEndLocalFirst)
{
    run_mserver_scenario("audio_call_with_remote_hold_local_hold_at_same_time_end_local_first.scr");
    register_voxip();
    call_and_hangup_with_parallel_hold(false, true);
    unregister_voxip();
}


//==================================================================================================
// Audio Call With Remote Hold Local Hold At Same Time End Local First:
// register, make call, wait for connection, enter remote hold, enter local hold,
// end remote hold, end local hold, hang up.
//==================================================================================================
BB_TEST_F(CallTest, AudioCallWithRemoteHoldLocalHoldAtSameTimeEndRemoteFirst)
{
    run_mserver_scenario("audio_call_with_remote_hold_local_hold_at_same_time_end_remote_first.scr");
    register_voxip();
    call_and_hangup_with_parallel_hold(false, false);
    unregister_voxip();
}


//==================================================================================================
//  Audio Call End During Local Hold From Remote Bye:
//  register, make call, wait for connection, enter local hold, receive Bye
//==================================================================================================
BB_TEST_F(CallTest, AudioCallEndDuringLocalHoldRemoteBye)
{
    run_mserver_scenario("audio_call_end_during_local_hold_remote_bye.scr");
    register_voxip();
    audio_call();
    gsm_hold(true);
    confirm_remote_hangup();
    unregister_voxip();
}


//==================================================================================================
//  Audio Call End During Local Hold From Remote Bye:
//  register, make call, wait for connection, enter remote hold, receive Bye
//==================================================================================================
BB_TEST_F(CallTest, AudioCallEndDuringRemoteHoldRemoteBye)
{
    run_mserver_scenario("audio_call_end_during_remote_hold_remote_bye.scr");
    register_voxip();
    audio_call();
    expect_remote_hold();
    confirm_remote_hangup();
    unregister_voxip();
}


//==================================================================================================
// Inbound Audio Call With Local Hold:
// register, receive call, wait for connection, enter local hold, end local hold, hangup
//==================================================================================================
BB_TEST_F(CallTest, InboundAudioCallWithLocalHold)
{
    run_mserver_scenario("inbound_audio_call_with_local_hold.scr");
    register_voxip();
    answer_call_and_hangup_with_hold(true);
}


//==================================================================================================
// Inbound Audio Call Ends In Local Hold:
// register, receive call, wait for connection, enter local hold, hangup
//==================================================================================================
BB_TEST_F(CallTest, InboundAudioCallLocalByeWhileInLocalHold)
{
    run_mserver_scenario("inbound_audio_call_local_bye_while_in_local_hold.scr");
    register_voxip();
    answer_call_and_hangup_while_on_hold(true);
    unregister_voxip();
}


//==================================================================================================
// Inbound Audio Call Remote Ends In Local Hold:
// register, receive call, wait for connection, enter local hold, hangup
//==================================================================================================
BB_TEST_F(CallTest, InboundAudioCallRemoteByeWhileInLocalHold)
{
    run_mserver_scenario("inbound_audio_call_remote_bye_while_in_local_hold.scr");
    register_voxip();
    answer_call_and_remote_hangup_while_on_hold(true);
}


//==================================================================================================
// Inbound Audio Call With Local Hold:
// register, receive call, wait for connection, enter remote hold, end remote hold, hangup
//==================================================================================================
BB_TEST_F(CallTest, InboundAudioCallWithRemoteHold)
{
    run_mserver_scenario("inbound_audio_call_with_remote_hold.scr");
    register_voxip();
    answer_call_and_hangup_with_hold(false);
}


//==================================================================================================
// Audio Call With Local Hold Fails 408:
// register, make call, wait for connection, attempt to enter local hold, receive 408 timeout, tear down call
//==================================================================================================
BB_TEST_F(CallTest, AudioCallWithLocalHoldFails408)
{
    run_mserver_scenario("audio_call_with_local_hold_fails_408.scr");
    register_voxip();
    audio_call();
    gsm_hold(true);
    
    if(!TestsEnv::inst()->wait_for_event(CallState_Hangup, 10))
    {
        throw string("Call was not diconnected after hold failure");
    }
    
    unregister_voxip();
}


//==================================================================================================
// Audio Call With Local Unhold Fails 408:
// register, make call, wait for connection, enter local hold, attempt to end local hold,
// receive 408 timeout, tear down call
//==================================================================================================
BB_TEST_F(CallTest, AudioCallWithLocalUnholdFails408)
{
    run_mserver_scenario("audio_call_with_local_unhold_fails_408.scr");
    register_voxip();
    audio_call();
    gsm_hold(true);
    gsm_unhold(true, false);
    
    if(!TestsEnv::inst()->wait_for_event(CallState_Hangup, 10))
    {
        throw string("Call was not diconnected after unhold failure");
    }
    
    unregister_voxip();
}





