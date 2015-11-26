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
#include "Connection.hpp"

//==========================================================================================================
//==========================================================================================================
class MServer
{
public:
    static MServer inst;
    
    void run(int argc, char * argv[]);
    string& get_value(string var);
    SipMessage* get_message(string kind, int timeout); // Message kind is either method or status code
    bool send_message(SipMessage &message);
    void print_vars();
    
private:
    MServer();

    Connection connection;
    map<string, string> vars;
    
    void process_args(int argc, char * argv[]);
    void error(string msg);
    void set_scenario_dir(char *argv_0);
};

#endif /* MServer_hpp */
