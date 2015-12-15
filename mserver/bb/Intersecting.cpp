//
//  Intersecting.cpp
//  VoxipBBTests
//
//  Intersecting call flows.
//  Naming convention of the tests:
//  intersect_<first|Second>_<audio|video|audio_video|video_audio>_<win|lose>
//  o First|Second: did voxip send the first invite
//  o Audio...    : the type of calls; first indicates the call that voixp initiated;
//                  if just one type then both sides initiated that type
//  o Win|Lose    : Only when the calls are of the same type, is voxip expected to win or lose,
//                  (i.e. which invite will receive OK and which will be cancelled)
//
//  Created by Yuval Dinari on 7/28/15.
//  Copyright (c) 2015 Yuval Dinari. All rights reserved.
//

#include "CallTest.h"


/**********************************************************************************************************************
 **********************************************************************************************************************
 *                                                                                                                    *
 *                                      FIRST-(AUDIO/VIDEO)-WIN TESTS                                                 *
 *                                                                                                                    *
 **********************************************************************************************************************
 **********************************************************************************************************************/

//======================================================================================================================
//======================================================================================================================
BB_TEST_F(CallTest, intersect_first_audio_win)
{
    run_mserver_scenario("intersect_first_win.scr", "is_video=false");
    register_voxip();
    audio_call();
    hangup();
}

//======================================================================================================================
//======================================================================================================================
BB_TEST_F(CallTest, intersect_first_video_win)
{
    run_mserver_scenario("intersect_first_win.scr", "is_video=true");
    register_voxip();
    video_call();
    hangup();
}

//======================================================================================================================
// Other side sends the cancel of its invitation before it sends ok to voxip's invitation
//======================================================================================================================
BB_TEST_F(CallTest, intersect_first_audio_win_cancel_before_ok)
{
    run_mserver_scenario("intersect_first_win_cancel_before_ok.scr", "is_video=false");
    register_voxip();
    audio_call();
    hangup();
}

//======================================================================================================================
// Other side sends the cancel of its invitation before it sends ok to voxip's invitation
//======================================================================================================================
BB_TEST_F(CallTest, intersect_first_video_win_cancel_before_ok)
{
    run_mserver_scenario("intersect_first_win_cancel_before_ok.scr", "is_video=true");
    register_voxip();
    video_call();
    hangup();
}



/**********************************************************************************************************************
 **********************************************************************************************************************
 *                                                                                                                    *
 *                                      FIRST-(AUDIO/VIDEO)-LOSE TESTS                                                *
 *                                                                                                                    *
 **********************************************************************************************************************
 **********************************************************************************************************************/


//======================================================================================================================
//======================================================================================================================
BB_TEST_F(CallTest, intersect_first_audio_lose)
{
    test_intersect_first_lose(false);
}


//======================================================================================================================
//======================================================================================================================
BB_TEST_F(CallTest, intersect_first_video_lose)
{
    test_intersect_first_lose(true);
}


//======================================================================================================================
// Helper function for intersect_first_(audio|video)_lose test
//======================================================================================================================
void CallTest::test_intersect_first_lose(bool is_video)
{
    run_mserver_scenario("intersect_first_lose.scr", "is_video=" + toStringLog(is_video));
    register_voxip();
    
    string data, call_id;
    VOIPManager::Inst()->MakeCall(CALL_DEST_1, CALL_UUID_1, is_video);
    
    //--------------------------------------------------------------------------------------------------
    // Wait for the hangup of the outgoing call
    //--------------------------------------------------------------------------------------------------
    if(!TestsEnv::inst()->wait_for_event(CallState_Hangup, 5, &data)) // Outgoing call should be canecelled
    {
        throw string("Expected outgoing call to fail");
    }
    
    call_id = get_json_value(data, "CID");
    
    if(call_id != CALL_UUID_1)
    {
        throw string("Expected hangup for call id ") + CALL_UUID_1 + " but received hangup for call id " + call_id;
    }
    
    //--------------------------------------------------------------------------------------------------
    // Wait for the connection of the incoming call
    //--------------------------------------------------------------------------------------------------
    if(!TestsEnv::inst()->wait_for_event(CallState_Connected, 5, &data))
    {
        throw string("Expected incoming call to be connected");
    }
    
    call_id = get_json_value(data, "CID");
    
    if(call_id != MAX_CALL_ID)
    {
        throw string("Expected connect for call id ") + MAX_CALL_ID + " but received connect for call id " + call_id;
    }
} // test_intersect_first_lose()



/**********************************************************************************************************************
 **********************************************************************************************************************
 *                                                                                                                    *
 *                                      SECOND-(AUDIO/VIDEO)-WIN TESTS                                                *
 *                                                                                                                    *
 **********************************************************************************************************************
 **********************************************************************************************************************/


//======================================================================================================================
//======================================================================================================================
BB_TEST_F(CallTest, intersect_second_audio_win)
{
    test_intersect_second_win(false);
}


//======================================================================================================================
//======================================================================================================================
BB_TEST_F(CallTest, intersect_second_video_win)
{
    test_intersect_second_win(true);
}


//======================================================================================================================
// Other side sends the cancel of its invitation before it sends ok to voxip's invitation
//======================================================================================================================
BB_TEST_F(CallTest, intersect_second_audio_win_cancel_before_ok)
{
    test_intersect_second_win(false, true);
}


//======================================================================================================================
// Other side sends the cancel of its invitation before it sends ok to voxip's invitation
//======================================================================================================================
BB_TEST_F(CallTest, intersect_second_video_win_cancel_before_ok)
{
    test_intersect_second_win(true, true);
}


//======================================================================================================================
//======================================================================================================================
void CallTest::test_intersect_second_win(bool is_video, bool cancel_before_ok)
{
    string scenario = (cancel_before_ok ? "intersect_second_win_cancel_before_ok.scr" : "intersect_second_win.scr");
    run_mserver_scenario(scenario, "is_video=" + toStringLog(is_video));
    register_voxip();
    EVoXIPUIEvent alert_event = (is_video ? CallStateVideo_AlertIncomingVideo : CallState_AlertIncoming);
    
    if(!TestsEnv::inst()->wait_for_event(alert_event, 5))
    {
        throw string("Expected incoming call");
    }
    
    if(is_video)
    {
        video_call();
    }
    else
    {
        audio_call();
    }
}


/**********************************************************************************************************************
 **********************************************************************************************************************
 *                                                                                                                    *
 *                                      SECOND-(AUDIO/VIDEO)-LOSE TESTS                                               *
 *                                                                                                                    *
 **********************************************************************************************************************
 **********************************************************************************************************************/


//======================================================================================================================
//======================================================================================================================
BB_TEST_F(CallTest, intersect_second_audio_lose)
{
    test_intersect_second_lose(false);
}


//======================================================================================================================
//======================================================================================================================
BB_TEST_F(CallTest, intersect_second_video_lose)
{
    test_intersect_second_lose(true);
}


//======================================================================================================================
//======================================================================================================================
void CallTest::test_intersect_second_lose(bool is_video)
{
    run_mserver_scenario("intersect_second_lose.scr", "is_video=" + toStringLog(is_video));
    register_voxip();
    EVoXIPUIEvent alert_event = (is_video ? CallStateVideo_AlertIncomingVideo : CallState_AlertIncoming);

    if(!TestsEnv::inst()->wait_for_event(alert_event, 5))
    {
        throw string("Expected incoming call");
    }

    string data, call_id;
    VOIPManager::Inst()->MakeCall(CALL_DEST_1, CALL_UUID_1, is_video);

    //--------------------------------------------------------------------------------------------------
    // Wait for the hangup of the outgoing call
    //--------------------------------------------------------------------------------------------------
    if(!TestsEnv::inst()->wait_for_event(CallState_Hangup, 5, &data)) // Outgoing call should be canecelled
    {
        throw string("Expected outgoing call to fail");
    }

    call_id = get_json_value(data, "CID");

    if(call_id != CALL_UUID_1)
    {
        throw string("Expected hangup for call id ") + CALL_UUID_1 + " but received hangup for call id " + call_id;
    }

    //--------------------------------------------------------------------------------------------------
    // Wait for the connection of the incoming call
    //--------------------------------------------------------------------------------------------------
    if(!TestsEnv::inst()->wait_for_event(CallState_Connected, 5, &data))
    {
        throw string("Expected incoming call to be connected");
    }

    call_id = get_json_value(data, "CID");

    if(call_id != MAX_CALL_ID)
    {
        throw string("Expected connect for call id ") + MAX_CALL_ID + " but received connect for call id " + call_id;
    }
}



/**********************************************************************************************************************
 **********************************************************************************************************************
 *                                                                                                                    *
 *                                          AUDIO-VIDEO TESTS                                                         *
 *                                                                                                                    *
 **********************************************************************************************************************
 **********************************************************************************************************************/


//======================================================================================================================
//======================================================================================================================
BB_TEST_F(CallTest, intersect_first_audio_video)
{
    run_mserver_scenario("intersect_first_audio_video.scr");
    register_voxip();
    string data, call_id;
    VOIPManager::Inst()->MakeCall(CALL_DEST_1, CALL_UUID_1, false);
    
    //--------------------------------------------------------------------------------------------------
    // Wait for the hangup of the outgoing call
    //--------------------------------------------------------------------------------------------------
    if(!TestsEnv::inst()->wait_for_event(CallState_Hangup, 5, &data)) // Outgoing call should be canecelled
    {
        throw string("Expected outgoing call to fail");
    }
    
    call_id = get_json_value(data, "CID");
    
    if(call_id != CALL_UUID_1)
    {
        throw string("Expected hangup for call id ") + CALL_UUID_1 + " but received hangup for call id " + call_id;
    }
    
    //--------------------------------------------------------------------------------------------------
    // Wait for the connection of the incoming call
    //--------------------------------------------------------------------------------------------------
    if(!TestsEnv::inst()->wait_for_event(CallState_Connected, 5, &data))
    {
        throw string("Expected incoming call to be connected");
    }
}


//==================================================================================================
//==================================================================================================
BB_TEST_F(CallTest, intersect_first_video_audio)
{
    run_mserver_scenario("intersect_first_video_audio.scr");
    register_voxip();
    video_call();
}


//==================================================================================================
//==================================================================================================
BB_TEST_F(CallTest, intersect_first_video_audio_cancel_before_ok)
{
    run_mserver_scenario("intersect_first_video_audio_cancel_before_ok.scr");
    register_voxip();
    video_call();
}


//==================================================================================================
//==================================================================================================
BB_TEST_F(CallTest, intersect_second_audio_video)
{
    run_mserver_scenario("intersect_second_audio_video.scr");
    register_voxip();

    if(!TestsEnv::inst()->wait_for_event(CallStateVideo_AlertIncomingVideo, 5))
    {
        throw string("Expected incoming call");
    }
    
    string data, call_id;
    VOIPManager::Inst()->MakeCall(CALL_DEST_1, CALL_UUID_1, false);
    
    //--------------------------------------------------------------------------------------------------
    // Wait for the hangup of the outgoing call
    //--------------------------------------------------------------------------------------------------
    if(!TestsEnv::inst()->wait_for_event(CallState_Hangup, 5, &data)) // Outgoing call should be canecelled
    {
        throw string("Expected outgoing call to fail");
    }
    
    call_id = get_json_value(data, "CID");
    
    if(call_id != CALL_UUID_1)
    {
        throw string("Expected hangup for call id ") + CALL_UUID_1 + " but received hangup for call id " + call_id;
    }
    
    //--------------------------------------------------------------------------------------------------
    // Wait for the connection of the incoming call
    //--------------------------------------------------------------------------------------------------
    if(!TestsEnv::inst()->wait_for_event(CallState_Connected, 5, &data))
    {
        throw string("Expected incoming call to be connected");
    }
}


//==================================================================================================
//==================================================================================================
BB_TEST_F(CallTest, intersect_second_video_audio)
{
    run_mserver_scenario("intersect_second_video_audio.scr");
    register_voxip();

    if(!TestsEnv::inst()->wait_for_event(CallState_AlertIncoming, 5))
    {
        throw string("Expected incoming call");
    }
    
    video_call();
}


//==================================================================================================
//==================================================================================================
BB_TEST_F(CallTest, intersect_second_video_audio_cancel_before_ok)
{
    run_mserver_scenario("intersect_second_video_audio_cancel_before_ok.scr");
    register_voxip();
    
    if(!TestsEnv::inst()->wait_for_event(CallState_AlertIncoming, 5))
    {
        throw string("Expected incoming call");
    }
    
    video_call();
}















