//
//  Call.hpp
//  mserver
//
//  Created by Yuval Dinari on 12/11/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef Call_hpp
#define Call_hpp

#include "common.h"

class SipMessage;

//==========================================================================================================
//==========================================================================================================
class Call
{
public:
    Call(int _number, SipMessage* first_msg);
    
    const int number;
    const Direction dir;
    
    string get_me() { return me; }
    string get_other() { return other; }
    void update(SipMessage* msg);
    
private:
    bool to_completed = false;
    string me;
    string other;
};

#endif /* Call_hpp */
