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
    StartListeningCommand(){}
    virtual void interpret(string &line, ifstream &file, ScriptReader &reader);
    virtual string get_start_regex_str();
};


#endif /* StartListeningCommand_hpp */
