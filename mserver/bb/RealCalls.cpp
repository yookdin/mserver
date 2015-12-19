//
//  RealCalls.cpp
//  VoxipBBTests
//
//  Created by Yuval Dinari on 12/16/15.
//  Copyright © 2015 Yuval Dinari. All rights reserved.
//


#include "CallTest.h"


#define ENV_NAME "qa6"
#define QUALA_URL "sip-10001.accounts.koala.vocal-dev.com"
#define QA6_URL "sip-213349.accounts.qa6.vocal-qa.com"

//==================================================================================================
//==================================================================================================
BB_TEST_F(CallTest, DISABLED_transfer_real_call)
{
    VOIPManager::Inst()->SetConfigItem(VXSI_SHOULD_RESOLVE_DNS, false);
    
    string url, user, num1, num2;
    
    if(ENV_NAME == "quala")
    {
        url = QUALA_URL;
        user = "VH2160"; // 448
        num1 = "449";
        num2 = "450";
    }
    else if(ENV_NAME == "qa6")
    {
        url = QA6_URL;
        user = "VH436363"; // 484
        num1 = "485";
        num2 = "486";
    }
    
    VOIPManager::Inst()->SetAccountData(url.c_str(), user.c_str(), "Vocal123", "10000", "Elvin Jones");
    
    register_voxip();
    call(num1, CALL_UUID_1, false);
    
    VOIPManager::Inst()->TransferCall(CALL_UUID_1, num2.c_str());
    
    if(!TestsEnv::inst()->wait_for_event(CallState_Hangup, 10))
    {
        throw string("Expected hangup event after call transfer");
    }
    
    unregister_voxip();
}


//==================================================================================================
//==================================================================================================
BB_TEST_F(CallTest, DISABLED_receive_real_call)
{
    VOIPManager::Inst()->SetConfigItem(VXSI_SHOULD_RESOLVE_DNS, false);
    
    string url, user;
    
    if(ENV_NAME == "quala")
    {
        url = QUALA_URL;
        user = "VH2166";
    }
    else if(ENV_NAME == "qa6")
    {
        url = QA6_URL;
        user = "VH436364";
    }
    
    VOIPManager::Inst()->SetAccountData(url.c_str(), user.c_str(), "Vocal123", "10000", "Chick Webb");

    register_voxip();
    string uuid = wait_for_incoming_call();
    answer_call(uuid);
    
    // Wait for other side to hangup
    if(!TestsEnv::inst()->wait_for_event(CallState_Hangup, 50))
    {
        throw string("Didn't receive hangup");
    }
    
    expect_termination_reason(app_reason_ok, 30);
    
    unregister_voxip();
}


//==================================================================================================
//==================================================================================================
BB_TEST_F(CallTest, DISABLED_receive_transferred_call)
{
    VOIPManager::Inst()->SetConfigItem(VXSI_SHOULD_RESOLVE_DNS, false);
    
    string url, user;
    
    if(ENV_NAME == "quala")
    {
        url = QUALA_URL;
        user = "VH2179";
    }
    else if(ENV_NAME == "qa6")
    {
        url = QA6_URL;
        user = "VH439885";
    }

    VOIPManager::Inst()->SetAccountData(url.c_str(), user.c_str(), "Vocal123", "10000", "Jojo Mayer");
    
    register_voxip();
    string uuid = wait_for_incoming_call(60);
    answer_call(uuid);
    ssleep(21);
    hangup(uuid);
    
    expect_termination_reason(app_reason_ok, 30);
    
    unregister_voxip();
}
