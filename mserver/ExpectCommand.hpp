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

// TODO:
// o Quote replaced values if detected as strings: containing spaces; not a number?
// o Supported operators:
//   o arithmetic: + - * / %
//   o bit-operations (| & etc.)?
//   o boolean: ||, &&, yield => ?
//   o comparison: == != > < >= <= ~(regex match); except match all can be for numbers and strings
// o Parentheses ()


//==========================================================================================================
//==========================================================================================================
class ExpectCommand: public Command
{
public:
private:
};

#endif /* ExpectCommand_hpp */
