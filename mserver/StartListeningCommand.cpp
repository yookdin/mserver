//
//  StartListeningCommand.cpp
//  mserver
//
//  Created by Yuval Dinari on 12/6/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "MServer.hpp"
#include "StartListeningCommand.hpp"


//==========================================================================================================
//==========================================================================================================
void StartListeningCommand::interpret(string &line, ifstream &file, ScriptReader &reader)
{
    MServer::inst.start_listening();
}


//==========================================================================================================
//==========================================================================================================
string StartListeningCommand::get_start_regex_str()
{
    return "<(start_listening)/>";
}
