//
//  SipConnection.cpp
//  mserver
//
//  Created by Yuval Dinari on 11/5/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "SipConnection.hpp"

#define CONNECT_TIMEOUT 10


//======================================================================================================================
//======================================================================================================================
SipConnection::SipConnection(string _ip, int _port): ip(_ip), port(_port), Connection(_ip, _port, SOCK_STREAM)
{
    pfd.fd = -1;
    pfd.events = POLLIN | POLLOUT;
    pfd.revents = 0;
    
    //------------------------------------------------------------------------------------------------------------------
    // Set up the bounded socket for listening, but don't accept any connections yet. This will be initiated by the first
    // get/send_message() call.
    //------------------------------------------------------------------------------------------------------------------
    if(listen(bounded_socket, 1) != 0) // 1 is the max size of incoming connections queue
    {
        throw string("listen() error");
    }
    
    // Set the listen socket to be non-blocking, so accept() won't block
    int flags = fcntl(bounded_socket, F_GETFL);
    flags |= O_NONBLOCK;
    
    if(fcntl(bounded_socket, F_SETFL, flags) == -1)
    {
        throw string("fcntl() error");
    }
}


//======================================================================================================================
// Try until timeout expires to get a message. If one found and is of right kind return it, o/w return null.
//======================================================================================================================
SipMessage* SipConnection::get_message(string kind, int timeout)
{
    if(pfd.fd == -1)
    {
        connect();
    }
        
    time_t start_time = time(nullptr);
    
    long old_size = msg_queue.size();
    
    while(msg_queue.empty() && time(nullptr) <= start_time + timeout)
    {
        try_poll(); // Check that there are no errors on the socket
        
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
                
                // The buffer might contain more than one sip message, extract all of them into the messages queue
                for(long offset = 0; num_bytes > 0; num_bytes -= offset)
                {
                    msg_queue.push_back(new SipMessage(buf + offset, offset, num_bytes));
                }

                break;
            }
        }
        
        usleep(10000);
    }
    
    if(!msg_queue.empty())
    {
        for(long i = old_size; i < msg_queue.size(); ++i)
        {
            msg_queue[i]->print();
        }

        SipMessage* front = msg_queue.front();
        if(front->get_kind() == kind)
        {
            msg_queue.pop_front();
            return front;
        }
    }
    
    return nullptr;

} // get_message()


//======================================================================================================================
// Send message through the socket, return true if successful, false o/w
//======================================================================================================================
bool SipConnection::send_message(SipMessage &message)
{
    if(pfd.fd == -1)
    {
        connect();
    }

    time_t start_time = time(nullptr);
    int timeout = 3;
    
    while(time(nullptr) <= start_time + timeout)
    {
        try_poll();
        
        if(pfd.revents & POLLOUT)
        {
            long bytes_to_write;
            message.write_to_buffer(buf, bytes_to_write);
            long bytes_writen = ::write(pfd.fd, buf, bytes_to_write);

            if(bytes_writen != bytes_to_write)
            {
                throw string("write() error");
            }
            
            message.print();
            
            return true;
        }
        
        usleep(10000);
    }
    
    return false;
}


//======================================================================================================================
// Accept a client connection.
//======================================================================================================================
void SipConnection::connect()
{
    sockaddr_in client_addr;
    socklen_t client_length = sizeof(client_addr);
    
    // Wait for the client to connect; if the timeout expires throw error
    cout << "Waiting for client to connect on " << ip << ":" << port << endl;
    time_t start_time = time(nullptr);
    
    while(time(nullptr) <= start_time + CONNECT_TIMEOUT)
    {
        pfd.fd = accept(bounded_socket, (sockaddr *) &client_addr, &client_length);
        
        if(pfd.fd == -1)
        {
            if(errno == EWOULDBLOCK) // No connections, sleep and try again
            {
                usleep(1000);
            }
            else // Real error
            {
                throw string("accept() error");
            }
        }
        else // Successful
        {
            cout << "Connected to client" << endl << endl;
            break;
        }
    }
    
    if(pfd.fd == -1)
    {
        throw string("Client failed to connect for " + to_string(CONNECT_TIMEOUT) + " second" + (CONNECT_TIMEOUT > 1 ?   "s" : ""));
    }
}


//======================================================================================================================
// poll() the file descriptors (which are sockets) and exit if error
//======================================================================================================================
void SipConnection::try_poll()
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








