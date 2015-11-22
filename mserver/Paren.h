//
//  Paren.h
//  mserver
//
//  Created by Yuval Dinari on 11/20/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef Paren_h
#define Paren_h

#include "Token.h"

//==========================================================================================================
//==========================================================================================================
class OpenParen: public Token
{
public:
    static const string str;
    OpenParen(): Token(OPEN_PAREN) {}
    string to_string() { return str; }

};

const string OpenParen::str = "(";


//==========================================================================================================
//==========================================================================================================
class CloseParen: public Token
{
public:
    static const string str;
    CloseParen(): Token(CLOSE_PAREN) {}
    string to_string() { return str; }
};

const string CloseParen::str = ")";

#endif /* Paren_h */
