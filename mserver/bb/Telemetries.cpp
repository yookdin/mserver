//
//  Telemetries.cpp
//  VoxipBBTests
//
//  Created by Yuval Dinari on 10/14/15.
//  Copyright © 2015 Yuval Dinari. All rights reserved.
//

#include "CallTest.h"
#include "VoXIPDeviceLayer.h"
#include "BBSimulatedDeviceLayer.h"

//======================================================================================================================
// Check that bettery strength is reported correctly
//======================================================================================================================
BB_TEST_F(CallTest, BatteryStrength)
{
    int battery_strength = 55;
    VxDeviceState::Inst()->Battery()->setValue(battery_strength);
    
    run_mserver_scenario("complete_audio_call.scr");
    register_voxip();
    call_and_hangup(false);
    
    Call *call = get_call(CALL_UUID_1); // The only call in this test, with the default call id
    
    if(stoi(get_json_value(call->telemetries, "BATTERYSTRENGTH")) != battery_strength)
    {
        throw string("Expected battery strength to be: " + to_string(battery_strength));
    }
}


//======================================================================================================================
// Check that the correct paylod type is reported
//======================================================================================================================
BB_TEST_F(CallTest, PayloadType)
{
    run_mserver_scenario("audio_call_ilbc.scr");
    register_voxip();
    call_and_hangup(false);
    
    Call *call = get_call(CALL_UUID_1); // The only call in this test, with the default call id
    
    if(stoi(get_json_value(call->telemetries, "PAYLOADTYPE")) != 114) // ILBC
    {
        throw string("Expected payload type to be: " + to_string(114));
    }
}


//======================================================================================================================
// Test that the connection type is wifi
//======================================================================================================================
BB_TEST_F(CallTest, ConnectionTypeWIFI)
{
    test_connection_type(CONNECT_TYPE_WIFI);
}


//======================================================================================================================
// Test that the connection type is 3G
//======================================================================================================================
BB_TEST_F(CallTest, ConnectionType3G)
{
    test_connection_type(CONNECT_TYPE_3G);
}


//======================================================================================================================
// Test that the correct connection type is reported
//======================================================================================================================
void CallTest::test_connection_type(string connection_type)
{
    if(connection_type == CONNECT_TYPE_WIFI)
    {
        VxConnectivity::Inst()->Network()->setValue(2);
    }
    else if(connection_type == CONNECT_TYPE_3G)
    {
        VxConnectivity::Inst()->Network()->setValue(1);
    }
    else
    {
        throw string("Unsupported connection type " + connection_type);
    }
    
    run_mserver_scenario("complete_audio_call.scr");
    register_voxip();
    call_and_hangup(false);
    
    Call *call = get_call(CALL_UUID_1); // The only call in this test, with the default call id
    string received_connection_type = get_json_value(call->telemetries, "MOBILECONNECTIONTYPE");
    
    if(received_connection_type != connection_type)
    {
        throw string("Expected connection type to be: " + connection_type + " but received: " + received_connection_type);
    }
}


//======================================================================================================================
// Test signal-stength report
//======================================================================================================================
BB_TEST_F(CallTest, SignalStrength)
{
    int signal_strength = 100;
    
    run_mserver_scenario("complete_audio_call.scr");
    register_voxip();
    
    // Set the signal strength here and not in the beginning of the test because at the beginning voxip init process
    // might be still running and connection type not set yet
    if(string(VOIPSettings::Inst()->GetConnectType()) == CONNECT_TYPE_3G)
    {
        VxConnectivity::Inst()->GSMSignal()->setValue(signal_strength);
    }
    else
    {
        VxConnectivity::Inst()->WiFiSignal()->setValue(signal_strength);
    }
    
    call_and_hangup(false);
    
    Call *call = get_call(CALL_UUID_1); // The only call in this test, with the default call id
    int received_sig_strength = stoi(get_json_value(call->telemetries, "SIGNALSTRENGTH"));
    
    if(received_sig_strength != signal_strength)
    {
        throw string("Expected signal strength to be: ") + to_string(signal_strength) + " but received: " + to_string(received_sig_strength);
    }
}


//======================================================================================================================
// Incoming call is rejected, check that termination reason is app_reason_call_rejected_by_user
//======================================================================================================================
BB_TEST_F(CallTest, RejectCall)
{
    run_mserver_scenario("complete_send_invite_rejected.scr");
    register_voxip();
    reject_call();
    expect_termination_reason(app_reason_call_rejected_by_user);
}


//======================================================================================================================
// Outgoing call is not answered, check that app_reason_tsx_transport_error_ringing is reported
//======================================================================================================================
BB_TEST_F(CallTest, CallNotAnswered)
{
    run_mserver_scenario("call_not_answered.scr");
    register_voxip();
    audio_call_without_answer();
    expect_termination_reason(app_reason_tsx_transport_error_ringing);
}


//======================================================================================================================
// Lose connection in mid call, check that app_reason_no_audio_timeout is reported
//======================================================================================================================
BB_TEST_F(CallTest, NoAudioTimeout)
{
    run_mserver_scenario("no_audio_timeout.scr");
    register_voxip();
    audio_call();
    expect_termination_reason(app_reason_no_audio_timeout, 22);
}


//======================================================================================================================
// TODO: how to create the media update failure?
//======================================================================================================================
BB_TEST_F(CallTest, DISABLED_PJMediaUpdateFail)
{
    prepare_and_launch_processes(&CallTest::sipp_simple_call, SIPP_INVITE_AND_CANCEL, false);
    register_voxip();
    audio_call_without_answer();
    expect_termination_reason(app_reason_pjsip_failed_in_pjsua_call_on_media_update);
}











































