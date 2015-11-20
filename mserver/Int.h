//
//  Int.h
//  mserver
//
//  Created by Yuval Dinari on 11/20/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef Int_h
#define Int_h

#include "Value.h"
#include "Bool.h"

//==========================================================================================================
//==========================================================================================================
class Int: public Value
{
public:
    Int(int _val = 0): Value(INT), val(_val) {}
    int get_int() { return val; }
    
    Value& operator+ (Value& other) { return *(new Int(val  +  other.get_int())); }
    Value& operator- (Value& other) { return *(new Int(val  -  other.get_int())); }
    Value& operator* (Value& other) { return *(new Int(val  *  other.get_int())); }
    Value& operator/ (Value& other) { return *(new Int(val  /  other.get_int())); }
    Value& operator% (Value& other) { return *(new Int(val  %  other.get_int())); }
    Value& operator==(Value& other) { return *(new Bool(val == other.get_int())); }
    Value& operator!=(Value& other) { return *(new Bool(val != other.get_int())); }
    Value& operator< (Value& other) { return *(new Bool(val <  other.get_int())); }
    Value& operator> (Value& other) { return *(new Bool(val >  other.get_int())); }
    Value& operator<=(Value& other) { return *(new Bool(val <= other.get_int())); }
    Value& operator>=(Value& other) { return *(new Bool(val >= other.get_int())); }

    void print() { cout << val << endl; }

private:
    const int val;
    
};

#endif /* Int_h */
