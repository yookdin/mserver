//
//  IfStatement.cpp
//  mserver
//
//  Created by Yuval Dinari on 12/16/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "IfStatement.hpp"


//==========================================================================================================
//==========================================================================================================
IfStatement::IfStatement(bool _parent_execute, string condition_str, bool _implicit):
    parent_execute(_parent_execute), implicit(_implicit)
{
    if(parent_execute)
    {
        // tmp, replace with expression evaluation
        if(regex_search(condition_str, regex("^ *true")))
        {
            should_execute = true;
        }
    }
}


//==========================================================================================================
//==========================================================================================================
void IfStatement::switch_to_else() {
    if(else_found)
    {
        throw string("Multiple else clauses in if statement");
    }
    
    if(parent_execute)
    {
        should_execute = !should_execute;
    }
    
    else_found = true;
}
