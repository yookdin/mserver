//
//  ControlConnection.hpp
//  VoxipBBTests
//
//  Created by Yuval Dinari on 12/2/15.
//  Copyright © 2015 Yuval Dinari. All rights reserved.
//

#ifndef ControlConnection_hpp
#define ControlConnection_hpp

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>

using namespace std;

//==================================================================================================
//==================================================================================================
class ControlConnection
{
public:
    ControlConnection(string ip, int port);
    
    string get_message();
    void send_message(string str);
    
private:
    char buf[500];
    int my_socket;
    sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);
};

#endif /* ControlConnection_hpp */
