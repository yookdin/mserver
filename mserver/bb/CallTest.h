//
//  CallTest.h
//  VoxipBBTests
//
//  Created by Yuval Dinari on 7/8/15.
//  Copyright (c) 2015 Yuval Dinari. All rights reserved.
//

#ifndef __VoxipBBTests__CallTest__
#define __VoxipBBTests__CallTest__

#include "BBTest.h"
#include "Call.h"


class CallTest: public BBTest
{
public:
    CallTest();
    
    virtual void SetUp();
    virtual void TearDown();
    virtual void voxip_ui_callback(EVoXIPUIEvent event, const char* data);


    //------------------------------------------------------------------------------------------------------------------
    // SIPP launchers for different scenarions
    //------------------------------------------------------------------------------------------------------------------
    void sipp_audio_call_local_and_remote_hold_sequential(VoidLaunchInfo *launch_info, bool local_first);
    void sipp_audio_call_local_then_remote_hold_parallel(VoidLaunchInfo *launch_info, bool end_local_first);
    void sipp_audio_call_remote_then_local_hold_parallel(VoidLaunchInfo *launch_info, bool end_local_first);

    void sipp_simple_call(VoidLaunchInfo *launch_info, string script, bool unregister = true);
    void sipp_simple_inbound_call(VoidLaunchInfo *launch_info, string script);
    
    void sipp_audio_call(VoidLaunchInfo *launch_info, bool local_bye);
    void sipp_record_audio_call(VoidLaunchInfo *launch_info);
    
    void sipp_video_call(VoidLaunchInfo *launch_info);
    void sipp_audio_call_local_hold(VoidLaunchInfo *launch_info, bool unhold);
    void sipp_audio_call_remote_hold(VoidLaunchInfo *launch_info, bool unhold);
    void sipp_audio_call_hold_end_remote_bye(VoidLaunchInfo *launch_info, bool is_local);
    void sipp_call_handoff(VoidLaunchInfo *launch_info);
    void sipp_invite_fail(VoidLaunchInfo *launch_info, string error_code);

    void sipp_inbound_audio_call(VoidLaunchInfo *launch_info, bool local_bye);
    void sipp_inbound_audio_call_with_hold(VoidLaunchInfo *launch_info, bool local_hold);
    void sipp_inbound_audio_call_end_during_local_hold(VoidLaunchInfo *launch_info);
    void sipp_inbound_audio_call_remote_end_during_local_hold(VoidLaunchInfo *launch_info);
    void sipp_inbound_audio_call_while_on_hold(VoidLaunchInfo *launch_info);
    
    void sipp_invite_fail403(VoidLaunchInfo *launch_info);
    void sipp_invite407(VoidLaunchInfo *launch_info);
    void sipp_invite_fail407(VoidLaunchInfo *launch_info);
    void sipp_audio_call_local_hold_fails(VoidLaunchInfo *launch_info, bool on_hold);
    void sipp_inbound_audio_call_without_supported_codec(VoidLaunchInfo *launch_info);
    
    void sipp_call_then_reg_fail(VoidLaunchInfo *launch_info);
    
protected:
    //------------------------------------------------------------------------------------------------------------------
    // Do common preparations and launch proxy and sipp. Has to be template to enable launching different sipp functions
    // with different parameters list. Must be defined here for some C++ reason.
    //------------------------------------------------------------------------------------------------------------------
    template <class FunctionType, class... Args>
    void prepare_and_launch_processes(FunctionType&& func_pointer, Args&&... args)
    {
        launch_infos.push_back(new VoidLaunchInfo("proxy"));
        launch_infos.push_back(new VoidLaunchInfo("sipp"));
        int timeout = 5;
        
        //--------------------------------------------------------------------------------------------------------------
        // Launch proxy
        //--------------------------------------------------------------------------------------------------------------
        launch_infos[0]->future_res = async(launch::async, &BBTest::launch_proxy, this, launch_infos[0]);
        wait_for_process_to_run(launch_infos[0], timeout);

        //--------------------------------------------------------------------------------------------------------------
        // Launch SIPP
        //--------------------------------------------------------------------------------------------------------------
        // The weird last parameter is the way to pass the variable arguments list 'args' to another function
        launch_infos[1]->future_res = async(launch::async, func_pointer, this, launch_infos[1], _VSTD::forward<Args>(args)...);
        wait_for_process_to_run(launch_infos[1], timeout);
    }

    //------------------------------------------------------------------------------------------------------------------
    // Parametrized tests
    //------------------------------------------------------------------------------------------------------------------
    void test_intersect_first_lose(bool is_video);
    void test_intersect_second_win(bool is_video, bool cancel_before_ok = false);
    void test_intersect_second_lose(bool is_video);
    
    void test_connection_type(string connection_type);

    //------------------------------------------------------------------------------------------------------------------
    // VOXIP API activators
    //------------------------------------------------------------------------------------------------------------------
    void audio_call(bool hasTraceId = false);
    void audio_call_all_states();
    void audio_call_without_answer();
    void video_call();
    void reject_call();
    void hangup(string call_id = CALL_UUID_1, bool hasTraceId = false);
    void confirm_remote_hangup(bool hasTraceId = false);
    void call_and_hangup(bool call_type, bool hasTraceID = false);
    void answer_call_and_hangup(bool hasTraceId = false);
    void answer_call_and_hangup_with_hold(bool local_hold);
    void answer_call_and_hangup_while_on_hold(bool local_hold);
    void answer_call_and_remote_hangup_while_on_hold(bool local_hold);
    void call_and_hangup_with_local_hold_unhold(bool do_video_call, int hold_type);
    void test_invite_fail(string error_code, EVoXIPUIEvent event);
   
    void local_hold(string call_id = CALL_UUID_1);
    void local_unhold(string call_id = CALL_UUID_1);
    void expect_remote_hold();
    void expect_remote_unhold(string call_id = CALL_UUID_1);
    void expect_remote_unhold_during_local_hold(string call_id = CALL_UUID_1);
    void gsm_hold(bool GSM);
    void gsm_unhold(bool GSM, bool expect_connected = true, string call_id = CALL_UUID_1);
    
    void local_siri_hold();
    void local_siri_unhold();
    void attempt_call_on_hold();
    void receive_call_on_hold ();
    void call_and_hangup_while_on_local_hold(bool do_video_call, int hold_type);
    void call_and_hangup_with_remote_hold(bool do_video_call, bool unhold);
    void call_and_hangup_with_sequential_hold(bool do_video_call, bool local_first);
    void call_and_hangup_with_parallel_hold(bool enter_local_first, bool end_local_first);

    void toggle_mute(string call_id, bool on);
    void enable_mute(string call_id);
    void disable_mute(string call_id);
    
    string wait_for_incoming_call(bool hasTraceId = false);
    void answer_call(string call_id, bool hasTraceId = false);
    void call_waiting_answer_and_hold_out_in(int swaps);
    void hangup_active(string call_id = CALL_UUID_1);
    void add_call(string active_call_id);
    void end_and_answer(string active_call, string incoming_call);
    void hold_and_answer(string active_call, string incoming_call);
    void swap(string active_call, string hold_call);
    void hangup_both(string active_call, string hold_call);
    void merge(string active_call, string hold_call);
    
    void expect_termination_reason(AppRejectReason reason, int timeout = 10);
    
    //------------------------------------------------------------------------------------------------------------------
    // Methods dealing with calls state
    //------------------------------------------------------------------------------------------------------------------
    Call* get_call(string call_id);
    Call *get_call_by_sip_id(string sip_id);
    Call* check_call_exists(string call_id);
    Call* check_call_connected(string call_id);

    bool are_active_calls();
    void check_telemetries();
    virtual void post_internal_test_body();
    void compare_audio_files(string& src_audio_file, string& output_audio_file);

private:
    
    // Calls, identified by their non-SIP call id
    map<string, Call*> calls;
    
    bool in_teardown = false; // Indicates tear down has begun, ignore incoming voxip events
    
    //------------------------------------------------------------------------------------------------------------------
    // Event handlers
    //------------------------------------------------------------------------------------------------------------------

    // A map of functions for handling ui events
    map<EVoXIPUIEvent, void(CallTest::*)(string, Call *, const char *)> event_handlers;

    // This is the value type stored in the calls map, used by various queries
    typedef pair<string, Call*> string_call_pair;

    void handle_calling(string call_id, Call *call, const char *data);
    void handle_alertincoming(string call_id, Call *call, const char *data);
    void handle_alertincomingvideo(string call_id, Call *call, const char *data);
    void handle_connected(string call_id, Call *call, const char *data);
    void handle_onhold(string call_id, Call *call, const char *data);
    void handle_remotehold(string call_id, Call *call, const char *data);
    void handle_onhold_gsm(string call_id, Call *call, const char *data);
    void handle_gsm_end(string call_id, Call *call, const char *data);
    void handle_hangup(string call_id, Call *call, const char *data);
    void handle_sendarsmessage(string call_id, Call *call, const char *data);
    void handle_sipcallid(string call_id, Call *call, const char *data);

}; // class CallTest
 
#endif /* defined(__VoxipBBTests__CallTest__) */


































