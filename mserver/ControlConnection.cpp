//
//  ControlConnection.cpp
//  mserver
//
//  Created by Yuval Dinari on 11/28/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "ControlConnection.hpp"


//======================================================================================================================
//======================================================================================================================
ControlConnection::ControlConnection(string ip, int port)
{
    bind_socket(ip, port, SOCK_DGRAM);
}

//======================================================================================================================
//======================================================================================================================
string ControlConnection::get_message()
{
    long num_bytes = recvfrom(bounded_socket, buf, CONTROL_BUFFER_SIZE, 0, (sockaddr *)&client_addr, &addr_len); // This is blocking
    
    if(num_bytes < 0)
    {
        throw string("recvfrom() failed");
    }

    return string(buf, num_bytes);
}


//======================================================================================================================
//======================================================================================================================
void ControlConnection::send_message(string str)
{
    str.copy(buf, str.length());
    long num_bytes = sendto(bounded_socket, buf, str.length(), 0, (sockaddr *)&client_addr, addr_len);

    if(num_bytes != str.length())
    {
        throw string("sendto() failed");
    }
}





















