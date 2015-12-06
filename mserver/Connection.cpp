//
//  Connection.cpp
//  mserver
//
//  Created by Yuval Dinari on 11/30/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "Connection.hpp"


//======================================================================================================================
//======================================================================================================================
Connection::Connection(string ip, int port, int type)
{
    bind_socket(ip, port, type);
}


//======================================================================================================================
//======================================================================================================================
void Connection::bind_socket(string ip, int port, int type)
{
    if((bounded_socket = socket(AF_INET, type, 0)) < 0)
    {
        throw string("socket() error");
    }
    
    sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if((server_addr.sin_addr.s_addr = inet_addr(ip.c_str())) == (unsigned long)INADDR_NONE)
    {
        throw string("inet_addr() error");
    }
    
    // Set this option to enable immediate reuse of the port after program finishes
    int opt_val = 1;
    socklen_t opt_len = sizeof(opt_val);
    setsockopt(bounded_socket, SOL_SOCKET, SO_REUSEADDR, (void*)&opt_val, opt_len);
    
    if(::bind(bounded_socket, (sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        throw string("bind() error");
    }
}
