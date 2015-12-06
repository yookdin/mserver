//
//  Connection.hpp
//  mserver
//
//  Created by Yuval Dinari on 11/30/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef Connection_hpp
#define Connection_hpp

#include "common_headers.h"

class Connection
{
protected:
    Connection(string ip, int port, int type);
    ~Connection(){ close(bounded_socket); }
    void bind_socket(string ip, int port, int type);
    
    int bounded_socket;
};

#endif /* Connection_hpp */
