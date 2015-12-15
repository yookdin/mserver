//
//  CallTransfer.cpp
//  VoxipBBTests
//
//  Contains all test containing a Call Transfer Scenario.
//
//  Created by Yuval Dinari on 7/28/15.
//  Copyright (c) 2015 Yuval Dinari. All rights reserved.
//

#include "CallTest.h"

//==================================================================================================
//==================================================================================================
BB_TEST_F(CallTest, BasicCallTransfer)
{
    run_mserver_scenario("call_transfer.scr");
    register_voxip();
    audio_call();
    VOIPManager::Inst()->TransferCall(CALL_UUID_1, "1234"); // Number to call is dummy
    
    if(!TestsEnv::inst()->wait_for_event(CallState_Hangup, 10))
    {
        throw string("Expected hangup event after call transfer");
    }
}
