////
////  Media.cpp
////  VoxipBBTests
////
////  Created by Yuval Dinari on 11/9/15.
////  Copyright © 2015 Yuval Dinari. All rights reserved.
////
//
//#include "CallTest.h"
//
//
////======================================================================================================================
////======================================================================================================================
//void CallTest::compare_audio_files(string& src_audio_file, string& output_audio_file)
//{
//    //------------------------------------------------------------------------------------------------------------------
//    // Copy the files to the linux machine
//    //------------------------------------------------------------------------------------------------------------------
//    string cmd = "scp " + src_audio_file + " " + output_audio_file + " " + LINUX_HOST + ":" + LINUX_DIR;
//    
//    if(system(cmd.c_str()) != 0)
//    {
//        throw string("Command \"" + cmd + "\" failed");
//    }
//    
//    //------------------------------------------------------------------------------------------------------------------
//    // Run the the comparison script on the linux machine
//    //------------------------------------------------------------------------------------------------------------------
//    cmd = string("ssh ") + LINUX_HOST + " " + LINUX_DIR + "/" + AUDIO_COMPARISON_SCRIPT + " " + basename(src_audio_file) + " " + basename(output_audio_file);
//
//    if(system(cmd.c_str()) != 0)
//    {
//        throw string("Command \"" + cmd + "\" failed");
//    }
//
//    LOG_DEBUG("Audio comparison succeeded");
//}
//
//
////======================================================================================================================
////======================================================================================================================
//TEST_F(CallTest, record_in_audio)
//{
//    VOIPManager::Inst()->SetConfigItem(VXSI_STUN_SERVER_NAME, "");
//    VOIPManager::Inst()->SetConfigItem(VXSI_RECORDING_AUDIO_TO_FILE_MODE_ENABLED, true);
//    string output_audio_file = TestsEnv::inst()->get_test_dir() + '/' + "recorded.wav";
//    VOIPManager::Inst()->SetConfigItem(VXSI_NAME_OF_AUDIO_FILE_FOR_RECORDING, output_audio_file.c_str());
//
//    try
//    {
//        prepare_and_launch_processes(&CallTest::sipp_simple_call, SIPP_PLAY_WAV, true);
//        register_voxip();
//        audio_call();
//        ssleep(10); // To enable recording 10 seconds of incoming audio
//        hangup();
//        unregister_voxip();
//        string src_audio_file = TestsEnv::inst()->get_sipp_scenarios_dir() + '/' + "pcmu_8000.wav";
//        compare_audio_files(src_audio_file, output_audio_file);
//        end_and_check_processes();
//    }
//    catch(string err) // These are exception thrown during normal test execution
//    {
//        KILL_AND_FAIL() << err;
//    }
//    catch(...) // This is an execption thrown after kill_processes(), no need to kill and its value doesn't matter
//    {
//        FAIL() << "Call succeeded but sipp failed";
//    }
//}
//
////======================================================================================================================
////======================================================================================================================
//TEST_F(CallTest, record_out_audio)
//{
//    try
//    {
//        prepare_and_launch_processes(&CallTest::sipp_record_audio_call);
//        register_voxip();
//        audio_call();
//        ssleep(5);
//        hangup();
//        unregister_voxip();
//        end_and_check_processes();
//    }
//    catch(string err) // These are exception thrown during normal test execution
//    {
//        KILL_AND_FAIL() << err;
//    }
//    catch(...) // This is an execption thrown after kill_processes(), no need to kill and its value doesn't matter
//    {
//        FAIL() << "Call succeeded but sipp failed";
//    }
//    
//    // Convert raw audio file to wav using sox
//    string cmd = "/usr/local/bin/sox --channels 1 --type raw --rate 8000 -e u-law " +
//                 TestsEnv::inst()->get_test_dir() + "/sipp_in_audio_rtp.raw " +
//                 TestsEnv::inst()->get_test_dir() + "/sipp_in_audio_rtp.wav";
//    system(cmd.c_str());
//}
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
