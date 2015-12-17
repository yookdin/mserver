//
//  ExpressionEvaluator.hpp
//  mserver
//
//  Created by Yuval Dinari on 12/17/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef ExpressionEvaluator_hpp
#define ExpressionEvaluator_hpp

#include "Command.h"
#include "Value.h"

//==========================================================================================================
//==========================================================================================================
class ExpressionEvaluator
{
public:
    static ExpressionEvaluator* inst();

    bool eval_bool(string& exp, ScriptReader* reader);
    int eval_int(string& exp, ScriptReader* reader);
    string eval_string(string& exp, ScriptReader* reader);
    string eval_as_string(string& exp, ScriptReader* reader);
    
private:
    static ExpressionEvaluator* the_inst;
    ExpressionEvaluator();
    
    const regex num_regex;
    const regex str_regex;
    const regex bool_regex;
    
    map<string, Token*> const_tokens;
    
    ScriptReader* reader = nullptr;
    
    Value* eval(string& exp, ScriptReader* reader);
    void convert_to_tokens(string &line, vector<Token*>& tokens);
    void convert_to_rpn(vector<Token*>& tokens, deque<Token*>& output);
    Value* eval_rpn(deque<Token*>& output);
    
    Token* try_const_token  (string& line, int& pos);
    Token* try_num          (string& line, int& pos);
    Token* try_bool         (string& line, int& pos);
    Token* try_string       (string& line, int& pos);
    Token* try_var          (string& line, int& pos);
    Token* interpret_value  (string& val);
};


#endif /* ExpressionEvaluator_hpp */
