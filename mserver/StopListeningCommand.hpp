//
//  StopListeningCommand.hpp
//  mserver
//
//  Created by Yuval Dinari on 12/6/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef StopListeningCommand_hpp
#define StopListeningCommand_hpp

#include "Command.h"

//==========================================================================================================
//==========================================================================================================
class StopListeningCommand: public Command
{
public:
    StopListeningCommand(){}
    virtual void interpret(string &line, ifstream &file, ScriptReader &reader);
    virtual string get_start_regex_str();
};


#endif /* StopListeningCommand_hpp */
