//
//  NextIPCommand.hpp
//  mserver
//
//  Created by Yuval Dinari on 12/5/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef NextIPCommand_hpp
#define NextIPCommand_hpp

#include "Command.h"

//==========================================================================================================
//==========================================================================================================
class NextIPCommand: public Command
{
public:
    NextIPCommand(): Command("move_to_next_ip") {}
    virtual void interpret(string &line, ifstream &file, ScriptReader &reader);
};

#endif /* NextIPCommand_hpp */
