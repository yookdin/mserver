//
//  Bool.h
//  mserver
//
//  Created by Yuval Dinari on 11/20/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef Bool_h
#define Bool_h

#include "common.h"
#include "Value.h"

//==========================================================================================================
//==========================================================================================================
class Bool: public Value
{
public:
    Bool(bool _val = false): Value(BOOL), val(_val) {}

    Bool(string val_str): Value(BOOL), val(stob(val_str)) {}

    bool get_bool() { return val; }

    Value& operator||(Value& other) { return *(new Bool(val || other.get_bool())); }
    Value& operator&&(Value& other) { return *(new Bool(val && other.get_bool())); }
    Value& operator!()              { return *(new Bool(!val)); }
    Value& operator==(Value& other) { return *(new Bool(val == other.get_bool())); }
    Value& operator!=(Value& other) { return *(new Bool(val != other.get_bool())); }
    virtual operator bool() { return val; };


    void flip() { val = !val; }
    virtual string to_string() { return (val ? "true" : "false"); }
    
private:
    bool val;
};

#endif /* Bool_h */
