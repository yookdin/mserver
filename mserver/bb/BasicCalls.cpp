//
//  BasicCalls.cpp
//  VoxipBBTests
//
//  Contains all basic inbound and outbound call flow scenarios.
//
//  Created by Yuval Dinari on 6/7/15.
//  Copyright (c) 2015 Yuval Dinari. All rights reserved.
//

#include "CallTest.h"


//==================================================================================================
// Basic call: register, make call, wait for connection, hangup
//==================================================================================================
BB_TEST_F(CallTest, BasicAudioCall)
{
    run_mserver_scenario("complete_audio_call.scr", "unregister = true");
    register_voxip();
    call_and_hangup(false);
    expect_termination_reason(app_reason_ok);
    unregister_voxip();
}


//==================================================================================================
// Basic call: register, make call, confirm all intermediate call states, wait for connection, hangup
//==================================================================================================
BB_TEST_F(CallTest, OutboundAudioCallConfirmAllStates)
{
    run_mserver_scenario("complete_audio_call.scr");
    register_voxip();
    audio_call_all_states();
    hangup();
}


//==================================================================================================
// Basic Video call: register, make call, wait for connection, hangup
//==================================================================================================
BB_TEST_F(CallTest, BasicVideoCall)
{
    run_mserver_scenario("complete_video_call.scr", "unregister = true");
    register_voxip();
    call_and_hangup(true);
    unregister_voxip();
}


//==================================================================================================
// Send an invite, get a 407, send an invite again with authorization and get an ok.
//==================================================================================================
BB_TEST_F(CallTest, Invite407)
{
    run_mserver_scenario("invite_407.scr");
    register_voxip();
    audio_call();
}


//==================================================================================================
// Basic Inbound call: register, receive call, wait for connection, hangup
//==================================================================================================
BB_TEST_F(CallTest, BasicInboundAudioCall)
{
    run_mserver_scenario("complete_inbound_audio_call.scr");
    register_voxip();
    answer_call_and_hangup();
}


//==================================================================================================
//Inbound call that is answered and then ended by the remote party.
//==================================================================================================
BB_TEST_F(CallTest, BasicInboundAudioCallRemoteBye)
{
    run_mserver_scenario("complete_inbound_audio_call_remote_bye.scr");
    register_voxip();
    string call_id = wait_for_incoming_call();
    answer_call(call_id);
    confirm_remote_hangup();
}


//==================================================================================================
// Basic call: register, make call, wait for connection, receive Bye
//==================================================================================================
BB_TEST_F(CallTest, BasicAudioCallRemoteBye)
{
    run_mserver_scenario("complete_audio_call_remote_bye.scr");
    register_voxip();
    audio_call();
    confirm_remote_hangup();
}


//==================================================================================================
// Basic call: register, make call, wait for connection, enable and disable mute, hangup
//==================================================================================================
BB_TEST_F(CallTest, OutboundAudioCallWithMute)
{
    run_mserver_scenario("complete_audio_call.scr");
    register_voxip();
    audio_call();
    enable_mute(CALL_UUID_1);
    disable_mute(CALL_UUID_1);
    hangup();
}


//==================================================================================================
// Basic Inbound Call: register, make call, wait for connection, enable and disable mute, hangup
//==================================================================================================
BB_TEST_F(CallTest, InboundAudioCallWithMute)
{
    run_mserver_scenario("complete_inbound_audio_call.scr");
    register_voxip();
    string call_id = wait_for_incoming_call();
    answer_call(call_id);
    enable_mute(call_id);
    disable_mute(call_id);
    hangup(call_id);
}


//==================================================================================================
// Basic call: register, make call, wait for connection, hangup.  Check that the HDAP Trace ID field
// received in SIP 183 and 200 OK are passed along in the UI Callbacks
//==================================================================================================
BB_TEST_F(CallTest, OutboundAudioCallWithTraceID)
{
    run_mserver_scenario("complete_audio_call_with_trace_id.scr");
    register_voxip();
    call_and_hangup(false, true);
}


//==================================================================================================
// Basic call: register, make call, wait for connection, receive Bye Check that the HDAP Trace ID field
// received in SIP 183 and 200 OK are passed along in the UI Callbacks
//==================================================================================================
BB_TEST_F(CallTest, OutboundAudioCallRemoteByeWithTraceID)
{
    run_mserver_scenario("complete_audio_call_remote_bye_trace_id.scr");
    register_voxip();
    audio_call(true);
    confirm_remote_hangup(true);
}


//==================================================================================================
// Basic Inbound call: register, receive call, wait for connection, hangup
// Check that the HDAP Trace ID field received in Invite are passed along in the UI Callbacks
//==================================================================================================
BB_TEST_F(CallTest, InboundAudioCallWithTraceID)
{
    run_mserver_scenario("complete_inbound_audio_call_trace_id.scr");
    register_voxip();
    answer_call_and_hangup(true);
}


//==================================================================================================
//Inbound call that is answered and then ended by the remote party.
//Check that the HDAP Trace ID field received in Invite are passed along in the UI Callbacks
//==================================================================================================
BB_TEST_F(CallTest, InboundAudioCallRemoteByeWithTraceID)
{
    run_mserver_scenario("complete_inbound_audio_call_remote_bye_trace_id.scr");
    register_voxip();
    string call_id = wait_for_incoming_call(true);
    answer_call(call_id, true);
    confirm_remote_hangup(true);
}

