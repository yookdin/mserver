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
    OpenParen(): Token(OPEN_PAREN) {}
};

//==========================================================================================================
//==========================================================================================================
class CloseParen: public Token
{
public:
    CloseParen(): Token(OPEN_PAREN) {}
};


#endif /* Paren_h */
