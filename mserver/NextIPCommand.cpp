
//
//  NextIPCommand.cpp
//  mserver
//
//  Created by Yuval Dinari on 12/5/15.
//  Copyright © 2015 Vonage. All rights reserved.
//
//  <move_to_next_ip/>

#include "MServer.hpp"
#include "NextIPCommand.hpp"


//==========================================================================================================
//==========================================================================================================
void NextIPCommand::interpret(string &line, ifstream &file, ScriptReader &reader)
{
    MServer::inst.advance_ip();
}


