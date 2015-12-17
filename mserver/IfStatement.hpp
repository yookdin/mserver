//
//  IfStatement.hpp
//  mserver
//
//  Created by Yuval Dinari on 12/16/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef IfStatement_hpp
#define IfStatement_hpp


#include "common.h"

class ScriptReader;

//==========================================================================================================
// A class represent an if-statement in a script
//==========================================================================================================
class IfStatement
{
public:
    IfStatement(bool _execute, string condition_str, ScriptReader* reader, bool _implicit = false);
    
    const bool implicit;
    bool should_execute = false;
    
    void switch_to_else();
    
private:
    const bool parent_execute;
    bool else_found = false;
};


#endif /* IfStatement_hpp */
