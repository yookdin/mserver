//
//  Bool.h
//  mserver
//
//  Created by Yuval Dinari on 11/20/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef Bool_h
#define Bool_h

#include "common_headers.h"
#include "Value.h"

//==========================================================================================================
//==========================================================================================================
class Bool: public Value
{
public:
    Bool(bool _val = false): Value(BOOL), val(_val) {}
    bool get_bool() { return val; }

    Value& operator||(Value& other) { return *(new Bool(val || other.get_bool())); }
    Value& operator&&(Value& other) { return *(new Bool(val && other.get_bool())); }
    Value& operator!()              { return *(new Bool(!val)); }
    Value& operator==(Value& other) { return *(new Bool(val == other.get_bool())); }
    Value& operator!=(Value& other) { return *(new Bool(val != other.get_bool())); }

    void flip() { val = !val; }
    void print() { cout << (val ? "true" : "false") << endl; }
    
private:
    bool val;
};

#endif /* Bool_h */
