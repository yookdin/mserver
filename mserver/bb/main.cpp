//
//  main.cpp
//  VoxipBBTests
//
//  Created by Yuval Dinari on 5/18/15.
//  Copyright (c) 2015 Yuval Dinari. All rights reserved.
//
// TODO:
// o Paramters for proxy run


#include "common_headers.h"

#undef __CLASS__
#define __CLASS__ "VOXIP_BB_TESTER"

// Global boolean indicating a signal was received
bool sig_received = false;

//==================================================================================================
//==================================================================================================
int main(int argc, char * argv[])
{
    signal(SIGUSR1, handle_sig); // Catch signal from child processes notifying that they're running
    testing::InitGoogleTest(&argc, argv); // gtest will strip argv of options belonging to it
    bool run_tests = ! testing::FLAGS_gtest_list_tests;
    
    if(run_tests)
    {
        TestsEnv::inst()->init(argc, argv);
    }

    int status = RUN_ALL_TESTS(); // If not really running this will just print the test names;
    
    if(run_tests)
    {
        delete TestsEnv::inst();
    }
    
    return status;
}
