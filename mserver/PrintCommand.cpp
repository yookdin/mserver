//
//  PrintCommand.cpp
//  mserver
//
//  Created by Yuval Dinari on 12/14/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "PrintCommand.hpp"
#include "ScriptReader.h"

//==========================================================================================================
//==========================================================================================================
void PrintCommand::interpret(string &line, ifstream &file, ScriptReader &reader)
{
    reader.replace_vars(line);
    cout << line << endl;
}

