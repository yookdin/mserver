//
//  3WayCall.cpp
//  VoxipBBTests
//
//  Contains all test containing a 3 Way Call Scenario
//
//  Created by Brian Pante on 7/28/15.
//  Copyright (c) 2015 Yuval Dinari. All rights reserved.
//

#include "CallTest.h"

//Functions for 3 Way Call Scenarios

//==================================================================================================
//Places active call on Hold and then makes a new call.
//==================================================================================================
void CallTest::add_call(string active_call_id)
{
    local_hold(active_call_id);
    VOIPManager::Inst()->MakeCall(CALL_DEST_2, CALL_UUID_2, false);
    VOIPManager::Inst()->MoveCallToUiFront(CALL_UUID_2, true); //todo: what is this?
    
    string data;
    
    if(!TestsEnv::inst()->wait_for_event(CallState_Connected, 10, &data))
    {
        throw string("2nd Audio Call Failed to Connect");
    }

    if(get_json_value(data, "CID") != CALL_UUID_2)
    {
        throw string("Wrong Call ID for Second Call Connected");
    }
}

//==================================================================================================
//Places active call on Hold and then makes a new call.
//==================================================================================================
void CallTest::merge(string active_call_id, string hold_call_id)
{
    VOIPManager::Inst()->SetMergedCallEnabled(active_call_id.c_str(), true);
    VOIPManager::Inst()->SetMergedCallEnabled(hold_call_id.c_str(), true);
    local_unhold(hold_call_id);
}

//==================================================================================================
//Basic 3 Way Call - Be on Outbound Call.  Place on Hold.  Place Second Outbound Call.
//End Second Call, Return to First, End First Call
//==================================================================================================
BB_TEST_F(CallTest, ThreeWay_Out_Out_No_Swap_or_Merge)
{
    run_mserver_scenario("threeway_out_out.scr");
    register_voxip();
    audio_call();
    add_call(CALL_UUID_1);
    hangup_both(CALL_UUID_2, CALL_UUID_1);
}

//==================================================================================================
//Basic 3 Way Call - Be on Inbound Call.  Place on Hold. Place Second Outbound Call.
//End Second Call, Return to First, End First Call
//==================================================================================================
BB_TEST_F(CallTest, ThreeWay_In_Out_No_Swap_or_Merge)
{
    run_mserver_scenario("threeway_in_out.scr");
    register_voxip();
    string incoming_call_id = wait_for_incoming_call();
    answer_call(incoming_call_id);
    add_call(incoming_call_id);
    //ssleep(2);
    hangup_both(CALL_UUID_2, incoming_call_id);
}


