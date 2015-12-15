//
//  CallFails.cpp
//  VoxipBBTests
//
//  Contains all test where the invite fails due to error code or no response.
//
//  Created by Yuval Dinari on 7/28/15.
//  Copyright (c) 2015 Yuval Dinari. All rights reserved.
//

#include "CallTest.h"

//==================================================================================================
// Invite Fail 407:  Register, send invite, receive 407 twice, register again
//==================================================================================================
BB_TEST_F(CallTest, InviteFail407)
{
    run_mserver_scenario("invite_fail_407.scr");
    register_voxip();
    VOIPManager::Inst()->MakeCall(CALL_DEST_1, CALL_UUID_1, false);
    
    if(!TestsEnv::inst()->wait_for_event(CallState_Idle_NotAuth, 5))
    {
        throw string("Event CallState_Idle_NotAuth didn't occur after multiple 407 responses");
    }

    if(!TestsEnv::inst()->wait_for_event(RegState_Registered, 5, false))
    {
        throw string("Second register failed");
    }
}


//==================================================================================================
// Invite Fail 403:  Register, send invite, receive 403, register again
//==================================================================================================
BB_TEST_F(CallTest, InviteFail403)
{
    run_mserver_scenario("invite_fail_403.scr");
    register_voxip();
    VOIPManager::Inst()->MakeCall(CALL_DEST_1, CALL_UUID_1, false);
    
    if(!TestsEnv::inst()->wait_for_event(CallState_Idle_NotAuth, 5))
    {
        throw string("Event CallState_Idle_NotAuth didn't occur after 403");
    }
    
    if(!TestsEnv::inst()->wait_for_event(RegState_Registered, 5, false))
    {
        throw string("Second register failed");
    }
}


//======================================================================================================================
// INVITE FAIL - Callstate_Idle_WrongDest for 400, 404, 410
//======================================================================================================================
BB_TEST_F(CallTest, InviteFail404)
{
    test_invite_fail("404 Not Found", CallState_Idle_WrongDest);
}

BB_TEST_F(CallTest, InviteFail400)
{
    test_invite_fail("400 Bad Request", CallState_Idle_WrongDest);
}

BB_TEST_F(CallTest, InviteFail410)
{
    test_invite_fail("410 Gone", CallState_Idle_WrongDest);
}// INVITE FAIL - Callstate_Idle_WrongDest

//======================================================================================================================
// INVITE FAIL - Callstate_Idle_Busy for 486, 600
//======================================================================================================================
BB_TEST_F(CallTest, InviteFail486)
{
    test_invite_fail("486 Busy Here", CallState_Idle_Busy);
}

BB_TEST_F(CallTest, InviteFail600)
{
    test_invite_fail("600 Busy Everywhere", CallState_Idle_Busy);
}// INVITE FAIL - Callstate_Idle_Busy

//======================================================================================================================
// INVITE FAIL - Callstate_Idle_NoAnswer for 603, 604
//======================================================================================================================
BB_TEST_F(CallTest, InviteFail603)
{
    test_invite_fail("603 Decline", CallState_Idle_NoAnswer);
}

BB_TEST_F(CallTest, InviteFail604)
{
    test_invite_fail("604 Does not exist anywhere", CallState_Idle_NoAnswer);
}// INVITE FAIL - Callstate_Idle_NoAnswer

//======================================================================================================================
// INVITE FAIL - Callstate_Idle_General for
// 402, 405, 406, 408, 413, 414, 415, 416, 420, 421, 480, 481, 482, 483,
// 484, 485, 487, 488, 493, 500, 501, 502, 503, 504, 505, 513, 606
//======================================================================================================================
BB_TEST_F(CallTest, InviteFail402)
{
    test_invite_fail("402 Payment Required", CallState_Idle_General);
}
BB_TEST_F(CallTest, InviteFail405)
{
    test_invite_fail("405 Method Not Allowed", CallState_Idle_General);
}
BB_TEST_F(CallTest, InviteFail406)
{
    test_invite_fail("406 Not Acceptable", CallState_Idle_General);
}
BB_TEST_F(CallTest, InviteFail408)
{
    test_invite_fail("408 Request Timeout", CallState_Idle_General);
}
BB_TEST_F(CallTest, InviteFail413)
{
    test_invite_fail("413 Request Entity Too Large", CallState_Idle_General);
}
BB_TEST_F(CallTest, InviteFail414)
{
    test_invite_fail("414 Request-URI Too Large", CallState_Idle_General);
}
BB_TEST_F(CallTest, InviteFail415)
{
    test_invite_fail("415 Unsupported Media Type", CallState_Idle_General);
}
BB_TEST_F(CallTest, InviteFail416)
{
    test_invite_fail("416 Unsupported URI Scheme", CallState_Idle_General);
}
BB_TEST_F(CallTest, InviteFail420)
{
    test_invite_fail("420 Bad Extension", CallState_Idle_General);
}
BB_TEST_F(CallTest, InviteFail421)
{
    test_invite_fail("421 Extension Required", CallState_Idle_General);
}
BB_TEST_F(CallTest, InviteFail480)
{
    test_invite_fail("480 Temporarily not available", CallState_Idle_General);
}
BB_TEST_F(CallTest, InviteFail481)
{
    test_invite_fail("481 Call Leg/Transaction Does Not Exist", CallState_Idle_General);
}
BB_TEST_F(CallTest, InviteFail482)
{
    test_invite_fail("482 Loop Detected", CallState_Idle_General);
}
BB_TEST_F(CallTest, InviteFail483)
{
    test_invite_fail("483 Too Many Hops", CallState_Idle_General);
}
BB_TEST_F(CallTest, InviteFail484)
{
    test_invite_fail("484 Address Incomplete", CallState_Idle_General);
}
BB_TEST_F(CallTest, InviteFail485)
{
    test_invite_fail("485 Ambiguous", CallState_Idle_General);
}
BB_TEST_F(CallTest, InviteFail487)
{
    test_invite_fail("487 Request Terminated", CallState_Idle_General);
}
BB_TEST_F(CallTest, InviteFail488)
{
    test_invite_fail("488 Not Acceptable Here", CallState_Idle_General);
}
BB_TEST_F(CallTest, InviteFail493)
{
    test_invite_fail("493 Undecipherable", CallState_Idle_General);
}
BB_TEST_F(CallTest, InviteFail500)
{
    test_invite_fail("500 Internal Server Error", CallState_Idle_General);
}
BB_TEST_F(CallTest, InviteFail501)
{
    test_invite_fail("501 Not Implemented", CallState_Idle_General);
}
BB_TEST_F(CallTest, InviteFail502)
{
    test_invite_fail("502 Bad Gateway", CallState_Idle_General);
}
BB_TEST_F(CallTest, InviteFail503)
{
    test_invite_fail("503 Service Unavailable", CallState_Idle_General);
}
BB_TEST_F(CallTest, InviteFail504)
{
    test_invite_fail("504 Server Time-out", CallState_Idle_General);
}
BB_TEST_F(CallTest, InviteFail505)
{
    test_invite_fail("505 SIP Version not supported", CallState_Idle_General);
}
BB_TEST_F(CallTest, InviteFail513)
{
    test_invite_fail("513 Message Too Large", CallState_Idle_General);
}
BB_TEST_F(CallTest, InviteFail606)
{
    test_invite_fail("606 Not Acceptable", CallState_Idle_General);
}//INVITE FAIL - Callstate_Idle_General


//==================================================================================================
// Invite Fail No Response:  Register, send invite, receive no response and confirm error is sent to UI
//==================================================================================================
BB_TEST_F(CallTest, InviteFailNoResponse)
{
    run_mserver_scenario("invite_fail_no_response.scr");
    register_voxip();
    VOIPManager::Inst()->MakeCall(CALL_DEST_1, CALL_UUID_1, false);
    
    //Check that call times out and error is sent to UI
    if(!TestsEnv::inst()->wait_for_event(CallState_Idle_General, 30))
    {
        throw string("Event CallState_Idle_General Did Not Occur After No Response");
    }
    
    if(!TestsEnv::inst()->wait_for_event(CallState_Hangup, 30))
    {
        throw string("Event CallState_Hangup Did Not Occur After No Response");
    }
}


//==================================================================================================
// Invite Fail No Response Without Reg:
// Make call without app registering, receive no response and confirm error is sent to UI
//==================================================================================================
BB_TEST_F(CallTest, InviteFailNoResponseWithoutReg)
{
    VOIPManager::Inst()->MakeCall(CALL_DEST_1, CALL_UUID_1, false);
    
    //Check that call times out and error is sent to UI
    if(!TestsEnv::inst()->wait_for_event(CallState_Idle_NotReg, 30))
    {
        throw string("Event CallState_Idle_General Did Not Occur After No Response");
    }
    
    if(!TestsEnv::inst()->wait_for_event(CallState_Hangup, 30))
    {
        throw string("Event CallState_Hangup Did Not Occur After No Response");
    }
    
    // Because there was no mserver in this test, need to tell default TestBody() not to check mserver status
    dont_check_mserver = true;
}


//==================================================================================================
// InboundAudioCallWithoutSupportedCodec: No codec supported, VoXIP reject call and hangup
//==================================================================================================
BB_TEST_F(CallTest, InboundAudioCallWithoutSupportedCodec)
{
    run_mserver_scenario("inbound_without_supported_codec.scr");
    register_voxip();
}


