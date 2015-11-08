//
//  Connection.hpp
//  mserver
//
//  Created by Yuval Dinari on 11/5/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef Connection_hpp
#define Connection_hpp

#include "common_headers.h"
#include "SipMessage.hpp"

class Connection
{
public:
    void setup(string ip, int port);
    SipMessage* get_message(string kind, int timeout); // Message kind is either method or status code
    bool send_message(SipMessage &message);
    
private:
    string ip;
    int port;
    int traffic_socket = -1;
    queue<SipMessage*> msg_queue;
    char buf[CONNECTION_BUFFER_SIZE+1]; // Add a place for null terminating
    pollfd pfd;
    
    int connect();
    int setup_socket_for_listening();
    void try_poll();
};

#endif /* Connection_hpp */
