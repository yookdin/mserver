//
//  ControlConnection.hpp
//  mserver
//
//  Created by Yuval Dinari on 11/28/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef ControlConnection_hpp
#define ControlConnection_hpp

#include "common_headers.h"
#include "Connection.hpp"

//======================================================================================================================
//======================================================================================================================
class ControlConnection: public Connection
{
public:
    ControlConnection(string ip, int port);
    
    string get_message();
    void send_message(string str);
    
private:
    char buf[CONTROL_BUFFER_SIZE];
    sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
};

#endif /* ControlConnection_hpp */
