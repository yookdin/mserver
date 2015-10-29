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

//==========================================================================================================
//==========================================================================================================
class MServer
{
public:
    MServer(int argc, char * argv[]);

private:
    string test_dir;
    string scenario_file;
    string ip;
    int port;
    enum CallIDKind {NONE, MIN, MAX} call_id_kind;
    
    void process_args(int argc, char * argv[]);
    void error(string msg);
    
    //======================================================================================================
    //======================================================================================================
    class Option
    {
    public:
        Option(bool mandatory, bool need_arg);
        bool missing();
        
        string val;
        bool mandatory;
        bool need_arg;
        bool found;
    };
};

#endif /* MServer_hpp */
