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
    ExpectCommand();
    virtual void interpret(string &line, ifstream &file, ScriptReader &reader);
    virtual string get_start_regex_str();

private:
    const regex end_regex;
};

#endif /* ExpectCommand_hpp */
