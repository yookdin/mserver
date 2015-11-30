//
//  MServer.hpp
//  mserver
//
//  Created by Yuval Dinari on 10/27/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef MServer_hpp
#define MServer_hpp

#include "common_headers.h"
#include "SipParser.hpp"
#include "SipConnection.hpp"
#include "ControlConnection.hpp"

//==========================================================================================================
//==========================================================================================================
class MServer
{
public:
    static MServer inst;
    
    void run(int argc, char * argv[]);
    void single_run(); // This is temporary, for running a single test w/o use of contol port
    string& get_value(string var);
    SipMessage* get_sip_message(string kind, int timeout); // Message kind is either method or status code
    bool send_sip_message(SipMessage &message);
    void print_vars();
    
private:
    MServer();

    SipConnection *sip_connection = nullptr;
    ControlConnection *ctrl_connection = nullptr;
    map<string, string> vars;
    
    void process_args(int argc, char * argv[]);
    void error(string msg);
    void set_scenario_dir(char *argv_0);
    void process_control_message(string& ctrl_msg, map<string, string>& script_vars);
};

#endif /* MServer_hpp */
