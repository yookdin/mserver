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
    NextIPCommand(){}
    virtual void interpret(string &line, ifstream &file, ScriptReader &reader);
    virtual string get_start_regex_str();
};

#endif /* NextIPCommand_hpp */
