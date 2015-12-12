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
    SetCommand();
    virtual void interpret(string &line, ifstream &file, ScriptReader &reader);
    virtual string get_start_regex_str();
    
private:
    const regex end_regex;
    const string value_str;
    const string ternarry_op_str;
    const regex var_set_regex;
};



#endif /* SetCommand_hpp */
