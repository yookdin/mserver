//
//  PlayFileCommand.hpp
//  mserver
//
//  Created by Yuval Dinari on 12/21/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef PlayFileCommand_hpp
#define PlayFileCommand_hpp


#include "Command.h"

//==========================================================================================================
//==========================================================================================================
class PlayFileCommand: public Command
{
public:
    PlayFileCommand(): Command("play") {}
    virtual void interpret(string &line, ifstream &file, ScriptReader &reader) = 0;
    
private:
    string filename;
    void process_args(string& line, ScriptReader &reader);
};



#endif /* PlayFileCommand_hpp */
