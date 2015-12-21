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

#define VOXIP_LOG "voxip.log"

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
#define RUN_DIR_OPT "run_dir"
#define SCENARIO_DIR_OPT "scenario_dir"
#define REAL_SERVER_OPT "real_server"
#define MSERVER_OPT "mserver"

//--------------------------------------------------------------------------------------------------
// Linux machine stuff. This is needed to run audio comparisons using aqua, which runs only on linux
//--------------------------------------------------------------------------------------------------
#define LINUX_HOST "jenkins@10.116.0.35"
#define LINUX_DIR "/slave/tmp_files_bb"
#define AUDIO_COMPARISON_SCRIPT "run_aqua.sh"

#endif /* defines_h */

