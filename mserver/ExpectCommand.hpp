//
//  ExpectCommand.hpp
//  mserver
//
//  Created by Yuval Dinari on 11/19/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef ExpectCommand_hpp
#define ExpectCommand_hpp

#include "Command.h"
#include "Int.h"
#include "Bool.h"
#include "String.h"


//==========================================================================================================
//==========================================================================================================
class ExpectCommand: public Command
{
public:
    ExpectCommand(): Command("expect") {}
    virtual void interpret(string &line, ifstream &file, ScriptReader &reader);
};

#endif /* ExpectCommand_hpp */
