//
//  AssignmentEvaluator.hpp
//  mserver
//
//  Created by Yuval Dinari on 12/18/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef AssignmentEvaluator_hpp
#define AssignmentEvaluator_hpp

#include "common.h"
#include "ScriptReader.h"

//==========================================================================================================
//==========================================================================================================
class AssignmentEvaluator
{
public:
    static AssignmentEvaluator* inst();

    void eval(string line, map<string, string>& vars, ScriptReader& reader);
    
private:
    static AssignmentEvaluator* the_inst;
    AssignmentEvaluator(){}
    
    const regex assignment_start = regex("(, *)?(\\w+) *(=)[^=]");

};

#endif /* AssignmentEvaluator_hpp */
