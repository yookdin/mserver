//
//  Value.h
//  mserver
//
//  Created by Yuval Dinari on 11/20/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef Value_h
#define Value_h

#include "common.h"
#include "Token.h"


//==========================================================================================================
// These are the types supported inside an expression to be evaluated
//==========================================================================================================
enum ValueType { STRING, INT, BOOL };


//==========================================================================================================
// Abstract class to represent a value of one of the supported types.
// Specific methods for getting values for all types must be defined here, because they have different
// return types, so we can't use just one virtual method get_value().
// A similar thing happens for operators - cannot define them only in the types that support them, because
// those types will not be known until runtime.
// By default, they all throw exceptions. The derived classes will override this behavior only for the
// methods they need.
//==========================================================================================================
class Value: public Token
{
public:
    ValueType get_type() { return type; }

    //------------------------------------------------------------------------------------------------------
    // Get value methods; derived classes will override only the one appropriate for them
    //------------------------------------------------------------------------------------------------------
    virtual int get_int() { throw string("Can't call get_int() for type " + get_type_name()); }
    virtual string get_string() { throw string("Can't call get_string() for type " + get_type_name()); }
    virtual bool get_bool() { throw string("Can't call get_bool() for type " + get_type_name()); }

private:
    Value& throw_wrong_op(string op) { throw string("Operator " + op + " not supported by type " + get_type_name()); }
    
public:
    
    //------------------------------------------------------------------------------------------------------
    // These are all the supported operators, but not all types support all of them. Derived classes will
    // override only the ones appropriate for them
    //------------------------------------------------------------------------------------------------------
    virtual Value& operator+ (Value& other) { return throw_wrong_op("+");  }
    virtual Value& operator- (Value& other) { return throw_wrong_op("-");  }
    virtual Value& operator* (Value& other) { return throw_wrong_op("*");  }
    virtual Value& operator/ (Value& other) { return throw_wrong_op("/");  }
    virtual Value& operator% (Value& other) { return throw_wrong_op("%");  }
    virtual Value& operator||(Value& other) { return throw_wrong_op("||"); }
    virtual Value& operator&&(Value& other) { return throw_wrong_op("&&"); }
    virtual Value& operator! ()             { return throw_wrong_op("!");  }
    virtual Value& operator==(Value& other) { return throw_wrong_op("=="); }
    virtual Value& operator!=(Value& other) { return throw_wrong_op("!="); }
    virtual Value& operator< (Value& other) { return throw_wrong_op("<");  }
    virtual Value& operator> (Value& other) { return throw_wrong_op(">");  }
    virtual Value& operator<=(Value& other) { return throw_wrong_op("<="); }
    virtual Value& operator>=(Value& other) { return throw_wrong_op(">="); }
    virtual Value& match     (Value& other) { return throw_wrong_op("~");  }
    virtual Value& not_match (Value& other) { return throw_wrong_op("!~"); }
    
    string get_type_name()
    {
        switch(type)
        {
            case STRING: return "STRING";
            case INT: return "INT";
            case BOOL: return "BOOL";
        }
    }
    
protected:
    Value(ValueType _type): Token(VALUE), type(_type) {}
    ValueType type;
};

#endif /* Value_h */
