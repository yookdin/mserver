//
//  SipConnection.hpp
//  mserver
//
//  Created by Yuval Dinari on 11/5/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef SipConnection_hpp
#define SipConnection_hpp

#include "common.h"
#include "SipMessage.hpp"
#include "Connection.hpp"

//======================================================================================================================
//======================================================================================================================
class SipConnection: public Connection
{
public:
    SipConnection(string ip, int port);
    ~SipConnection() { close(pfd.fd); }
    
    SipMessage* get_message(string kind, bool optional, int timeout, bool should_reconnect = true); // Message kind is either method or status code
    bool send_message(SipMessage &message);
    void clear();
    void start();
    void stop();
    void start(string in_ip, bool force_restart = false);
    void check_no_pending_messages();
    
private:
    string ip;
    int port;
    char buf[CONNECTION_BUFFER_SIZE+1]; // Add a place for null terminating
    pollfd pfd;

    deque<SipMessage*> msg_queue;
    
    void listen();
    void reconnect();
    void connect();
    void try_poll();
    string get_pending_messages_str();
};

#endif /* SipConnection_hpp */
