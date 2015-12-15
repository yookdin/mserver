//
//  defines.h
//  VoxipBBTests
//
//  Created by Yuval Dinari on 10/22/15.
//  Copyright © 2015 Yuval Dinari. All rights reserved.
//
//  Contains constants definitions

#ifndef defines_h
#define defines_h

//--------------------------------------------------------------------------------------------------
// Network stuff
//--------------------------------------------------------------------------------------------------
#define LOOPBACK_IP "127.0.0.1"
#define LOOPBACK_INTERFACE "lo0"
#define MSERVER_SIP_PORT "10000"
#define MSERVER_CONTROL_PORT "10001"
#define PROXY_VOXIP_SERVER_PORT "8999"  // proxy port used for incoming connection from voxip
#define PROXY_SIPP_SERVER_PORT "9000"   // proxy port used for incoming connection from sipp (when sipp is in client mode)
#define SIPP_SERVER_PORT "9001"         // sipp port when it is in server mode

//--------------------------------------------------------------------------------------------------
// Log file names
//--------------------------------------------------------------------------------------------------
#define SIPP_LOG "sipp.log"
#define SIPP_ERR_LOG "sipp_err.log"
#define SIPP_MSG_LOG "sipp_messages.log"
#define PROXY_LOG "proxy.log"
#define VOXIP_LOG "voxip.log"

//--------------------------------------------------------------------------------------------------
// SIPP script names
//--------------------------------------------------------------------------------------------------
#define SIPP_REG_SCRIPT "uas_reg.xml"

#define SIPP_UNREG_SCRIPT "uas_unreg.xml"

#define SIPP_REG_UNREG_SCRIPT "uas_reg_unreg.xml"

#define SIPP_REG_423_SCRIPT "uas_reg_423.xml"

#define SIPP_REG_FAIL_SCRIPT_TEMPLATE "uas_reg_fail_template.xml"

#define SIPP_REG_FAIL_401_BAD_CRED_SCRIPT "uas_reg_fail_401_bad_cred.xml"

#define SIPP_INVITE_FAIL_SCRIPT_TEMPLATE "uas_invite_fail_template.xml"

#define SIPP_COMPLETE_CALL_SCRIPT "uas_complete_audio_call.xml"

#define SIPP_COMPLETE_CALL_LOCAL_THEN_REMOTE_HOLD_SCRIPT "uas_complete_audio_call_local_then_remote_hold.xml"

#define SIPP_COMPLETE_CALL_REMOTE_THEN_LOCAL_HOLD_SCRIPT "uas_complete_audio_call_remote_then_local_hold.xml"

#define SIPP_COMPLETE_CALL_LOCAL_HOLD_END_REMOTE_SCRIPT "uas_complete_audio_call_local_hold_end_remote.xml"

#define SIPP_COMPLETE_CALL_REMOTE_HOLD_END_REMOTE_SCRIPT "uas_complete_audio_call_remote_hold_end_remote.xml"

#define SIPP_COMPLETE_CALL_LOCAL_THEN_REMOTE_HOLD_END_LOCAL_SCRIPT "uas_audio_local_then_remote_hold_end_local.xml"

#define SIPP_INBOUND_CALL_NO_SUPPORTED_CODEC "uac_inbound_without_supported_codec.xml"

#define SIPP_COMPLETE_CALL_LOCAL_THEN_REMOTE_HOLD_END_REMOTE_SCRIPT "uas_audio_local_then_remote_hold_end_remote.xml"

#define SIPP_COMPLETE_CALL_REMOTE_THEN_LOCAL_HOLD_END_LOCAL_SCRIPT "uas_audio_remote_then_local_hold_end_local.xml"

#define SIPP_COMPLETE_CALL_REMOTE_THEN_LOCAL_HOLD_END_REMOTE_SCRIPT "uas_audio_remote_then_local_hold_end_remote.xml"

#define SIPP_COMPLETE_CALL_LOCAL_HOLD_FAILS_SCRIPT "uas_complete_audio_call_local_hold_fails.xml"

#define SIPP_COMPLETE_CALL_LOCAL_UNHOLD_FAILS_SCRIPT "uas_complete_audio_call_local_unhold_fails.xml"

#define SIPP_COMPLETE_CALL_REMOTE_BYE_SCRIPT "uas_complete_audio_call_remote_bye.xml"

#define SIPP_COMPLETE_INBOUND_CALL_SCRIPT "uac_complete_inbound_audio_call.xml"

#define SIPP_COMPLETE_INBOUND_CALL_LOCAL_HOLD_SCRIPT "uac_complete_inbound_audio_call_with_local_hold.xml"

#define SIPP_COMPLETE_INBOUND_CALL_REMOTE_HOLD_SCRIPT "uac_complete_inbound_audio_call_with_remote_hold.xml"

#define SIPP_COMPLETE_INBOUND_CALL_REMOTE_BYE_SCRIPT "uac_complete_inbound_audio_call_remote_bye.xml"

#define SIPP_COMPLETE_INBOUND_CALL_WHILE_ON_HOLD_SCRIPT "uac_complete_inbound_audio_call_while_on_hold.xml"

#define SIPP_COMPLETE_CALL_LOCAL_HOLD_SCRIPT "uas_complete_audio_call_local_hold.xml"

#define SIPP_COMPLETE_CALL_END_DURING_LOCAL_HOLD_SCRIPT "uas_complete_audio_call_end_during_local_hold.xml"

#define SIPP_COMPLETE_INBOUND_CALL_END_DURING_LOCAL_HOLD_SCRIPT "uac_complete_inbound_audio_call_with_local_hold_end_local_bye.xml"

#define SIPP_COMPLETE_INBOUND_CALL_REMOTE_END_DURING_LOCAL_HOLD_SCRIPT "uac_complete_inbound_audio_call_remote_side_end_during_local_hold.xml"

#define SIPP_COMPLETE_CALL_REMOTE_HOLD_SCRIPT "uas_complete_audio_call_remote_hold.xml"

#define SIPP_COMPLETE_CALL_END_DURING_REMOTE_HOLD_SCRIPT "uas_complete_audio_call_end_during_remote_hold.xml"

#define SIPP_COMPLETE_VIDEO_CALL_SCRIPT "uas_complete_video_call.xml"

#define SIPP_PARTIAL_CALL_SCRIPT "uas_partial_audio_call.xml"

#define SIPP_CALL_HANDOFF_SCRIPT "uas_call_handoff.xml"

#define SIPP_INVITE_FAIL_403 "uas_invite_fail_403.xml"

#define SIPP_INVITE_407 "uas_invite_407.xml"

#define SIPP_INVITE_FAIL_407 "uas_invite_fail_407.xml"

#define SIPP_OUTBOUND_CALL_WITH_TRACEID "uas_complete_audio_call_with_trace_id.xml"
#define SIPP_INBOUND_CALL_WITH_TRACEID "uac_complete_inbound_audio_call_with_traceid.xml"
#define SIPP_INBOUND_CALL_REMOTE_BYE_WITH_TRACEID "uac_complete_inbound_audio_call_remote_bye_with_traceid.xml"
#define SIPP_OUTBOUND_CALL_REMOTE_BYE_WITH_TRACEID "uas_complete_audio_call_remote_bye_with_traceid.xml"

#define SIPP_RECEIVE_ACK "uas_receive_ack.xml"
#define SIPP_REJECTED_INBOUND_CALL "uac_rejected_inbound_call.xml"
#define SIPP_CANCEL_INBOUND_CALL "uac_cancel_inbound_call.xml"
#define SIPP_PARTIAL_NOT_ANSWERED_CALL_SCRIPT "uas_partial_audio_call_not_answered.xml"

#define SIPP_AUDIO_CALL_ILBC "uas_audio_call_ilbc.xml"
#define SIPP_INVITE_AND_CANCEL "uas_invite_and_cancel.xml"

#define SIPP_PLAY_WAV "uas_play_wav.xml"

//--------------------------------------------------------------------------------------------------
// Call and user parameters
//--------------------------------------------------------------------------------------------------
#define CALL_DEST_1 "12345678900"  // Number to call
#define CALL_UUID_1 "call_uuid_1" // Call identifier
#define CALL_DEST_2 "12345678902"  // Number to call for second outbound call
#define CALL_UUID_2 "call_uuid_2" // Call identifier for second outbound call
#define MAX_CALL_ID "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" // Actually could be longer
#define TRACE_ID "17209172011445449877235"

//--------------------------------------------------------------------------------------------------
// Command line option
//--------------------------------------------------------------------------------------------------
#define SIPP_OPT "-sipp"
#define PROXY_OPT "-proxy"
#define MSERVER_OPT "-mserver"
#define RUN_DIR_OPT "-run_dir"
#define SCENARIO_DIR_OPT "-scenario_dir"

//--------------------------------------------------------------------------------------------------
// Linux machine stuff. This is needed to run audio comparisons using aqua, which runs only on linux
//--------------------------------------------------------------------------------------------------
#define LINUX_HOST "jenkins@10.116.0.35"
#define LINUX_DIR "/slave/tmp_files_bb"
#define AUDIO_COMPARISON_SCRIPT "run_aqua.sh"

#endif /* defines_h */

