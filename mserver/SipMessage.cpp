//
//  SipMessage.cpp
//  mserver
//
//  Created by Yuval Dinari on 11/2/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "SipMessage.hpp"
#include "SipParser.hpp"

//==========================================================================================================
//==========================================================================================================
SipMessage::SipMessage(vector<string> &lines)
{
    auto parser = SipParser::inst;
    
    if(!parser.match(START_LINE, lines[0]))
    {
        throw string("SIP messaage expected to start with request or status line but received:\n" + lines[0]);
    }

    // TMP
    for(auto s: lines)
    {
        cout << s << endl;
    }
}
