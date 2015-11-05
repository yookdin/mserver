//
//  Connection.cpp
//  mserver
//
//  Created by Yuval Dinari on 11/5/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include "Connection.hpp"


//======================================================================================================================
// Open a connection and listen for incoming traffic
//======================================================================================================================
Connection::Connection(string _ip, int _port): ip(_ip), port(_port)
{
    traffic_socket = connect();
    pfd.fd = traffic_socket;
    pfd.events = POLLIN | POLLOUT;
    pfd.revents = 0;
}


//======================================================================================================================
// Try until timeout expires to get a message. If one found and is of right kind return it, o/w return null.
//======================================================================================================================
SipMessage* Connection::get_message(string kind, int timeout)
{
    time_t start_time = time(nullptr);
    
    while(msg_queue.empty() && time(nullptr) <= start_time + timeout)
    {
        try_poll(pfd); // Check that there are no errors on the socket
        
        long num_bytes = 0;
        
        if(pfd.revents & POLLIN) // Socket is ready
        {
            num_bytes = ::read(pfd.fd, buf, CONNECTION_BUFFER_SIZE);
            
            if(num_bytes < 0)
            {
                throw string("read() error");
            }
            else if(num_bytes == 0) // Other side issued a close of the TCP connection
            {
                return nullptr;
            }
            else // Successful read
            {
                buf[num_bytes] = '\0'; // Make buf a null-terminated string to enable printing etc.
                
                cout << "---------------------------START----------------------------------" << endl;
                cout << buf;
                cout << "----------------------------END-----------------------------------" << endl << endl;
            
                // The buffer might contain more than one sip message, extract all of them into the messages queue
                for(long offset = 0; num_bytes > 0; num_bytes -= offset)
                {
                    msg_queue.push(new SipMessage(buf + offset, offset, num_bytes));
                }

                break;
            }
        }
        
        usleep(10000);
    }
    
    if(!msg_queue.empty())
    {
        SipMessage* front = msg_queue.front();
        if(front->get_kind() == kind)
        {
            msg_queue.pop();
            return front;
        }
    }
    
    return nullptr;

} // get_message()


//======================================================================================================================
// Send message through the socket, return true if successful, false o/w
//======================================================================================================================
bool Connection::send_message(SipMessage &message)
{
    time_t start_time = time(nullptr);

    while(!(pfd.revents & POLLOUT) && time(nullptr) <= start_time + 1)
    {
        usleep(10000);
    }
    
    if(pfd.revents & POLLOUT)
    {
        long bytes_to_write;
        message.write_to_buffer(buf, bytes_to_write);
        long bytes_writen = ::write(pfd.fd, buf, bytes_to_write);
        
        if(bytes_writen != bytes_to_write)
        {
            throw string("write() error");
        }
    
        return true;
    }

    return false;
}


//======================================================================================================================
// poll() the file descriptors (which are sockets) and exit if error
//======================================================================================================================
void Connection::try_poll(pollfd pfd)
{
    long res = poll(&pfd, 1, 0);
    
    if(res < 0)
    {
        throw string("poll() error");
    }
    
    if(pfd.revents & POLLERR)
    {
        throw string("Error on voxip socket");
    }
}


//======================================================================================================================
// Connect to client, return the traffic socket
//======================================================================================================================
int Connection::connect()
{
    int listen_socket = setup_socket_for_listening();
    
    int traffic_socket = -1;
    sockaddr_in client_addr;
    socklen_t client_length = sizeof(client_addr);
    
    traffic_socket = accept(listen_socket, (sockaddr *) &client_addr, &client_length);
    cout << "Connected to client on port " << port << endl;
    return traffic_socket;
}

//======================================================================================================================
// Setup the socket on which to listen for incoming traffic from the client
//======================================================================================================================
int Connection::setup_socket_for_listening()
{
    sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    int listen_socket;
    
    if((server_addr.sin_addr.s_addr = inet_addr(ip.c_str())) == (unsigned long)INADDR_NONE)
    {
        throw string("inet_addr() error");
    }
    
    if((listen_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        throw string("socket() error when trying to setup socket for listening");
    }
    
    // Set this option to enable immediate reuse of the port after program finishes
    int opt_val = 1;
    socklen_t opt_len = sizeof(opt_val);
    setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, (void*)&opt_val, opt_len);
    
    if(::bind(listen_socket, (sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        throw string("bind() error");
    }
    
    if(listen(listen_socket, 1) != 0) // 1 is the max size of incoming connections queue
    {
        throw string("listen() error");
    }
    
    // Set listen_socket to be non-blocking, so accept() won't block
//    int flags = fcntl(listen_socket, F_GETFL);
//    flags |= O_NONBLOCK;
//    
//    if(fcntl(listen_socket, F_SETFL, flags) == -1)
//    {
//        throw string("fcntl() error");
//    }
    
    return listen_socket;
}








