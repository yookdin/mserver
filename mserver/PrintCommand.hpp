//
//  PrintCommand.hpp
//  mserver
//
//  Created by Yuval Dinari on 12/14/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef PrintCommand_hpp
#define PrintCommand_hpp

#include "Command.h"

//==========================================================================================================
//==========================================================================================================
class PrintCommand: public Command
{
public:
    PrintCommand(): Command("print") {}
    virtual void interpret(string &line, ifstream &file, ScriptReader &reader);
};


#endif /* PrintCommand_hpp */
