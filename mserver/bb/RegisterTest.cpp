//
//  RegisterTest.cpp
//  VoxipBBTests
//
//  Created by Yuval Dinari on 7/8/15.
//  Copyright (c) 2015 Yuval Dinari. All rights reserved.
//

#include "RegisterTest.h"


//======================================================================================================================
//======================================================================================================================
void RegisterTest::SetUp()
{
    BBTest::SetUp();
    config(SIPP_SERVER_PORT);
}


//======================================================================================================================
// Run a generic register fail error with the given error code. After the failure it is expected
// that voxip will try the next ip and will succeed to register.
//======================================================================================================================
void RegisterTest::test_register_fail(string error_code)
{
    if(TestsEnv::inst()->resolver->get_num_ips() < 2) // We need at least 2 ips for these tests
    {
        throw string("Can't run register fail test without at least one \"real\" local ip");
    }
    
    // Run the script with parameter named 'error_code' having the value of input string error_code. Put error_code in
    // quotes because it may contain spaces (e.g. "400 Bad Request")
    run_mserver_scenario("reg_fail.scr", "error_code=" + quote(error_code));
    register_voxip();
}

