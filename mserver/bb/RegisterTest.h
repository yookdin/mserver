//
//  RegisterTest.h
//  VoxipBBTests
//
//  Created by Yuval Dinari on 7/8/15.
//  Copyright (c) 2015 Yuval Dinari. All rights reserved.
//

#ifndef __VoxipBBTests__RegisterTest__
#define __VoxipBBTests__RegisterTest__

#include "BBTest.h"

class RegisterTest: public BBTest
{
protected:

    // General register fail test
    void test_register_fail(string error_code);
};


#endif /* defined(__VoxipBBTests__RegisterTest__) */
