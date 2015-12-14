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
    PrintCommand(){}
    virtual void interpret(string &line, ifstream &file, ScriptReader &reader);
    virtual string get_start_regex_str();
    
};


#endif /* PrintCommand_hpp */
