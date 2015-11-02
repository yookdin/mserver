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
    static MServer inst;

    void run(int argc, char * argv[]);
    string& get_value(string var);
    
private:
    MServer();

    map<string, string> vars;
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
