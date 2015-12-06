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
    string& get_value(string var);
    SipMessage* get_sip_message(string kind, int timeout); // Message kind is either method or status code
    bool send_sip_message(SipMessage &message);
    void advance_ip();
    void reset_ip();
    void print_vars();
    
private:
    MServer();

    bool debug = false;
    bool log_file_set = false;
    SipConnection *sip_connection = nullptr;
    ControlConnection *ctrl_connection = nullptr;
    map<string, string> vars;
    vector<string> ips;
    int cur_ip_index = 0;
    
    void process_args(int argc, char * argv[]);
    void set_scenario_dir(char *argv_0);
    void process_control_message(string& ctrl_msg, map<string, string>& script_vars);
    void set_log_file(string filepath);
    void extract_ips(string ip_list);
};

#endif /* MServer_hpp */
