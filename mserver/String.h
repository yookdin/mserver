//
//  String.h
//  mserver
//
//  Created by Yuval Dinari on 11/20/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef String_h
#define String_h

#include "Value.h"
#include "Bool.h"

//==========================================================================================================
//==========================================================================================================
class String: public Value
{
public:
    String(string _val = ""): Value(STRING), val(_val) {}
    string get_string() { return val; }
    
    Value& operator+ (Value& other) { return *(new String(val + other.get_string())); }
    Value& operator==(Value& other) { return *(new Bool(val == other.get_string())); }
    Value& operator!=(Value& other) { return *(new Bool(val != other.get_string())); }
    Value& operator< (Value& other) { return *(new Bool(val <  other.get_string())); }
    Value& operator> (Value& other) { return *(new Bool(val >  other.get_string())); }
    Value& operator<=(Value& other) { return *(new Bool(val <= other.get_string())); }
    Value& operator>=(Value& other) { return *(new Bool(val >= other.get_string())); }

    Value& match(Value& other) { return *(new Bool(regex_match(val, regex(other.get_string())))); }
    Value& not_match(Value& other)
    {
        Value& v = match(other);
        Bool* b = (Bool*)&v;
        b->flip();
        return v;
    }
    
    void print() { cout << '"' << val << '"'; }

private:
    const string val;
    
};

#endif /* String_h */
