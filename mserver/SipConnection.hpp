//
//  SipConnection.hpp
//  mserver
//
//  Created by Yuval Dinari on 11/5/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef SipConnection_hpp
#define SipConnection_hpp

#include "common_headers.h"
#include "SipMessage.hpp"
#include "Connection.hpp"

//======================================================================================================================
//======================================================================================================================
class SipConnection: public Connection
{
public:
    SipConnection(string ip, int port);
    ~SipConnection() { close(pfd.fd); }
    
    SipMessage* get_message(string kind, int timeout); // Message kind is either method or status code
    bool send_message(SipMessage &message);
    
private:
    string ip;
    int port;
    char buf[CONNECTION_BUFFER_SIZE+1]; // Add a place for null terminating
    pollfd pfd;

    deque<SipMessage*> msg_queue;
    
    void connect();
    void try_poll();
};

#endif /* SipConnection_hpp */