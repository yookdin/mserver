//
//  SetCommand.hpp
//  mserver
//
//  Created by Yuval Dinari on 12/8/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef SetCommand_hpp
#define SetCommand_hpp

#include "Command.h"

//==========================================================================================================
//==========================================================================================================
class SetCommand: public Command
{
public:
    SetCommand(): Command("set") {}
    virtual void interpret(string &line, ifstream &file, ScriptReader &reader);
};



#endif /* SetCommand_hpp */
