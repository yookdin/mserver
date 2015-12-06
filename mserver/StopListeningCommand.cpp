//
//  StopListeningCommand.cpp
//  mserver
//
//  Created by Yuval Dinari on 12/6/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "StopListeningCommand.hpp"
#include "MServer.hpp"

//==========================================================================================================
//==========================================================================================================
void StopListeningCommand::interpret(string &line, ifstream &file, ScriptReader &reader)
{
    MServer::inst.stop_listening();
}


//==========================================================================================================
//==========================================================================================================
string StopListeningCommand::get_start_regex_str()
{
    return "<(stop_listening)/>";
}
