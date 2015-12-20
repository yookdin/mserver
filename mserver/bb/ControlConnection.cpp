//
//  ControlConnection.cpp
//  VoxipBBTests
//
//  Created by Yuval Dinari on 12/2/15.
//  Copyright © 2015 Yuval Dinari. All rights reserved.
//

#include <sys/errno.h>
#include "VxLog.h"
#include "bbutils.h"
#include "ControlConnection.hpp"


//==================================================================================================
// Open a UDP socket, and prepare the server address struct.
//==================================================================================================
ControlConnection::ControlConnection(string ip, int port)
{
    if((my_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        throw string("socket() error");
    }
    
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if((server_addr.sin_addr.s_addr = inet_addr(ip.c_str())) == (unsigned long)INADDR_NONE)
    {
        throw string("inet_addr() error");
    }
}


//======================================================================================================================
//======================================================================================================================
void ControlConnection::send_message(string str)
{
    LOG_DEBUG("Sending mserver control message: %s", str.c_str());
    str.copy(buf, str.length());
    long num_bytes = sendto(my_socket, buf, str.length(), 0, (sockaddr *)&server_addr, addr_len);
    
    if(num_bytes != str.length())
    {
        throw string("sendto() failed");
    }
}

//======================================================================================================================
//======================================================================================================================
string ControlConnection::get_message()
{
    LOG_DEBUG("Waiting for mserver control response");
    long num_bytes = 0;
    
    num_bytes = recvfrom(my_socket, buf, sizeof(buf), 0, (sockaddr *)&server_addr, &addr_len);
    
    if(num_bytes <= 0)
    {
        throw string("Getting mserver status message failed");
    }
    
    buf[num_bytes] = '\0'; // To enable printing
    LOG_DEBUG("Got mserver status message: %s", buf);
    return string(buf, num_bytes);
}































