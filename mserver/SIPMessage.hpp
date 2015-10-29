//
//  SIPMessage.hpp
//  mserver
//
//  Created by Yuval Dinari on 10/27/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef SIPMessage_hpp
#define SIPMessage_hpp

#include "common_headers.h"

class SIPMessage {
public:
    SIPMessage(vector<string> lines);
    
private:
    static regex request_line_regex; // Request start line
    static regex status_line_regex;  // Response start line
    
    
    string start_line;
    vector<string> header;
    vector<string> body;
};

#endif /* SIPMessage_hpp */
