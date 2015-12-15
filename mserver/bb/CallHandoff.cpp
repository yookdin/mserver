//
//  CallTestsCallHandoff.cpp
//  VoxipBBTests
//
//  Contains all test containing a Call Handoff Scenario
//
//  Created by Yuval Dinari on 7/28/15.
//  Copyright (c) 2015 Yuval Dinari. All rights reserved.
//

#include "CallTest.h"

//==================================================================================================
// Basic Call Handoff:  Make call, change networks during call, confirm call moved to new network, end call.
//==================================================================================================
BB_TEST_F(CallTest, BasicCallHandoff)
{
    vector<VxIpAddress> ips = TestsEnv::inst()->resolver->get_ips();

    if(ips.size() < 3) // The list contains the loopback ip so we need two more "real" ips
    {
        // The ready-to-register event indicates that voxip finished init process. This is needed
        // due to voxip bug: if trying to stop before init finished, voxip crashes
        TestsEnv::inst()->wait_for_event(RegState_ReadyToRegister, 5);
        throw string("Can't run call-handoff test without at least two local ips");
    }

    // Prepate the next ip to simulate network change
    VxIpAddress cur_local_ip = VxConnectivity::Inst()->getCurrentIP(), next_local_ip;

    for(VxIpAddress ip : ips)
    {
        if(ip != cur_local_ip && !ip.is_loopback())
        {
            next_local_ip = ip;
            break;
        }
    }

    if(next_local_ip.is_unspecified())
    {
        throw string("Something is wrong, next_local_ip shouldn't be empty");
    }

    run_mserver_scenario("call_handoff.scr");
    register_voxip();
    audio_call();

    //--------------------------------------------------------------------------------------------------------
    // To create the call handoff scenario make a network change.
    //--------------------------------------------------------------------------------------------------------
    TestsEnv::inst()->clear_events(); // So waiting for events won't succeed because of old events still in the list
    voxip_shared_ptr<BBSimulatedConnectivity> connectivity = static_pointer_cast<BBSimulatedConnectivity>(VxConnectivity::Inst());
    connectivity->setCurrentIP(next_local_ip.to_string());
    connectivity->Network()->setValue(2); // Will cause network change in voxip

    if(!TestsEnv::inst()->wait_for_event(RegState_Registered, 10))
    {
        throw string("Second register failed");
    }

    // The media-stream-change event indicates (indirectly) that the re-invite succeeded.
    if(!TestsEnv::inst()->wait_for_event(CallState_Media_Stream_Change, 10))
    {
        throw string("Reinvite failed");
    }

    hangup();

} // BasicCallHandoff()
