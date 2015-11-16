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
    enum CallIDKind {NONE, MIN, MAX};
    
    void run(int argc, char * argv[]);
    string& get_value(string var);
    SipMessage* get_message(string kind, int timeout); // Message kind is either method or status code
    bool send_message(SipMessage &message);
    CallIDKind get_call_id_kind();
    
private:
    MServer();

    Connection connection;
    map<string, string> vars;
    CallIDKind call_id_kind;
    
    void process_args(int argc, char * argv[]);
    void error(string msg);
};

#endif /* MServer_hpp */
