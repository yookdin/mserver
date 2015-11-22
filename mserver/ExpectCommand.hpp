//
//  ExpectCommand.hpp
//  mserver
//
//  Created by Yuval Dinari on 11/19/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef ExpectCommand_hpp
#define ExpectCommand_hpp

#include "Command.h"
#include "Int.h"
#include "Bool.h"
#include "String.h"


//==========================================================================================================
//==========================================================================================================
class ExpectCommand: public Command
{
public:
    ExpectCommand(ScriptReader &_reader): Command(_reader) {}
    virtual void interpret(string &line, ifstream &file);

private:
    static const regex end_regex;
    static map<string, Token*> const_tokens;
    static map<string, Token*> init_const_tokens();

    void convert_to_tokens(string &line, ifstream &file, vector<Token*>& tokens);
    void convert_to_tokens(string &line, vector<Token*>& tokens);
    void convert_to_rpn(vector<Token*>& tokens, deque<Token*>& output);
    bool eval_expression(deque<Token*>& output);

    Token* try_const_token(string& line, int& pos);
    Token* try_num(string &line, int &pos);
    Token* try_string(string &line, int &pos);
    Token* try_header_name(string &line, int &pos);
    Token* try_var(string &line, int &pos);

};

#endif /* ExpectCommand_hpp */
