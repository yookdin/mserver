//
//  Token.h
//  mserver
//
//  Created by Yuval Dinari on 11/20/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef Token_h
#define Token_h

//==========================================================================================================
//==========================================================================================================
enum TokenKind { VALUE, OP, OPEN_PAREN, CLOSE_PAREN };

//==========================================================================================================
//==========================================================================================================
class Token
{
public:
    Token(TokenKind _kind): kind(_kind) {}
    const TokenKind kind;
    virtual string to_string() = 0;
};

#endif /* Token_h */
