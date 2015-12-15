//
//  Call.h
//  VoxipBBTests
//
//  Created by Yuval Dinari on 10/8/15.
//  Copyright © 2015 Yuval Dinari. All rights reserved.
//

#ifndef Call_hpp
#define Call_hpp

#include "common_headers.h"

class Call
{
public:
    enum Direction {IN, OUT} dir;

    Call(Direction dir);
    Call(string dir);

    static Direction string_to_dir(string dir_str);
    
    void end();
    void toggle_mute();
    void gsm_hold_on();
    void gsm_hold_off();
    void local_hold_on();
    void remote_hold_on();
    void connect();
    void check_direction(string direction);
    void check_telemetries();
    
    bool connected;
    bool mutted = false;
    bool local_hold = false;
    bool gsm_hold = false;
    bool remote_hold = false;
    bool ended = false;
    
    bool was_on_hold = false;
    string sip_id;
    string telemetries;
    
};


#endif /* Call_h */
