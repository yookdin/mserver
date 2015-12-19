//
//  AssignmentEvaluator.cpp
//  mserver
//
//  Created by Yuval Dinari on 12/18/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "AssignmentEvaluator.hpp"
#include "ExpressionEvaluator.hpp"

//==========================================================================================================
//==========================================================================================================
AssignmentEvaluator* AssignmentEvaluator::the_inst = nullptr;

//==========================================================================================================
//==========================================================================================================
AssignmentEvaluator* AssignmentEvaluator::inst()
{
    if(the_inst == nullptr)
    {
        the_inst = new AssignmentEvaluator();
    }
    
    return the_inst;
}


//==========================================================================================================
// Parse a string containing multipe assignments like:
// a=x b = 3 + (4 * [x]) ...
// and put vars and values in vars map.
//==========================================================================================================
void AssignmentEvaluator::eval(string line, map<string, string>& vars, ScriptReader& reader)
{
    sregex_iterator iter(line.begin(), line.end(), assignment_start);
    sregex_iterator end;
    
    string var;
    int rvalue_pos = -1;
    
    // Each start of an assignment marks the end of the last one (except the first), and you know which sub-string
    // to evaluate as the rvalue expression of the assignment
    for(;iter != end; ++iter)
    {
        if(rvalue_pos > -1)
        {
            vars[var] = ExpressionEvaluator::inst()->eval_as_string(line.substr(rvalue_pos, iter->position() - rvalue_pos), reader);
        }
        
        var = (*iter)[2];
        rvalue_pos = iter->position(3) + 1; // Just after the equal sign
    }

    // The last assignment was not applied. The rest of the line is the sub string to evaluate
    if(rvalue_pos > -1)
    {
        vars[var] = ExpressionEvaluator::inst()->eval_as_string(line.substr(rvalue_pos), reader);
    }
}

