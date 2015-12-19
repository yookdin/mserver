//
//  StartListeningCommand.hpp
//  mserver
//
//  Created by Yuval Dinari on 12/6/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef StartListeningCommand_hpp
#define StartListeningCommand_hpp

#include "Command.h"

//==========================================================================================================
//==========================================================================================================
class StartListeningCommand: public Command
{
public:
    StartListeningCommand(): Command("start_listening") {}
    virtual void interpret(string &line, ifstream &file, ScriptReader &reader);
};


#endif /* StartListeningCommand_hpp */
