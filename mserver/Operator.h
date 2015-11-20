//
//  Operator.h
//  mserver
//
//  Created by Yuval Dinari on 11/20/15.
//  Copyright © 2015 Vonage. All rights reserved.
//
// Precedence order (but not necessarily precedence numerical value) of ops is like in C++:
// 1: !
// 2: / * %
// 3: + -
// 4: ~ !~
// 5: < <= > >=
// 6: == !=
// 7: &&
// 8: ||
//
// Associativity is more strict than C++, int and bool are not interchangable, therefore
// (> < >= <=) cannot be contactenated. Match operators (~ !~) are also not contactenatable.

#ifndef Operator_h
#define Operator_h

#include "Token.h"
#include "Value.h"

//==========================================================================================================
//==========================================================================================================
class Operator: public Token
{
public:
    enum Associativiy {LEFT, RIGHT, NONE};
    typedef Operator::Associativiy Associativity;
    
    Operator(int _num_operands, int _precedence, Associativiy _associativiy):
        Token(OP), num_operands(_num_operands), precedence(_precedence), associativiy(_associativiy) {}

    const int num_operands;
    const int precedence;
    const Associativiy associativiy;
    
    virtual Value* execute(vector<Value*> vals) = 0;
    
protected:
    void check_operands_num(long num, string str)
    {
        if(num != num_operands)
        {
            throw string("Wrong num of operands for " + str + ", expected " + to_string(num_operands) + ", got " + to_string(num));
        }
    }
};

//==========================================================================================================
//==========================================================================================================
class Add: public Operator
{
public:
    static const string str;
    
    Add(): Operator(2, 3, LEFT) {}
    
    Value* execute(vector<Value*> vals)
    {
        check_operands_num(vals.size(), str);
        return &(*vals[0] + *vals[1]);
    }
};


//==========================================================================================================
//==========================================================================================================
class Subtract: public Operator
{
public:
    static const string str;
    
    Subtract(): Operator(2, 3, LEFT) {}
    
    Value* execute(vector<Value*> vals)
    {
        check_operands_num(vals.size(), str);
        return &(*vals[0] - *vals[1]);
    }
};


//==========================================================================================================
//==========================================================================================================
class Mul: public Operator
{
public:
    static const string str;
    
    Mul(): Operator(2, 2, LEFT) {}
    
    Value* execute(vector<Value*> vals)
    {
        check_operands_num(vals.size(), str);
        return &(*vals[0] * *vals[1]);
    }
};


//==========================================================================================================
//==========================================================================================================
class Div: public Operator
{
public:
    static const string str;
    

    Div(): Operator(2, 2, LEFT) {}
    
    Value* execute(vector<Value*> vals)
    {
        check_operands_num(vals.size(), str);
        return &(*vals[0] / *vals[1]);
    }
};


//==========================================================================================================
//==========================================================================================================
class Mod: public Operator
{
public:
    static const string str;
    

    Mod(): Operator(2, 2, LEFT) {}
    
    Value* execute(vector<Value*> vals)
    {
        check_operands_num(vals.size(), str);
        return &(*vals[0] % *vals[1]);
    }
};


//==========================================================================================================
//==========================================================================================================
class Or: public Operator
{
public:
    static const string str;
    

    Or(): Operator(2, 8, LEFT) {}
    
    Value* execute(vector<Value*> vals)
    {
        check_operands_num(vals.size(), str);
        return &(*vals[0] || *vals[1]);
    }
};


//==========================================================================================================
//==========================================================================================================
class And: public Operator
{
public:
    static const string str;
    

    And(): Operator(2, 7, LEFT) {}
    
    Value* execute(vector<Value*> vals)
    {
        check_operands_num(vals.size(), str);
        return &(*vals[0] && *vals[1]);
    }
};


//==========================================================================================================
//==========================================================================================================
class Not: public Operator
{
public:
    static const string str;
    

    Not(): Operator(1, 1, RIGHT) {}
    
    Value* execute(vector<Value*> vals)
    {
        check_operands_num(vals.size(), str);
        return &(!(*vals[0]));
    }
};


//==========================================================================================================
//==========================================================================================================
class Equal: public Operator
{
public:
    static const string str;
    

    Equal(): Operator(2, 6, LEFT) {}
    
    Value* execute(vector<Value*> vals)
    {
        check_operands_num(vals.size(), str);
        return &(*vals[0] == *vals[1]);
    }
};


//==========================================================================================================
//==========================================================================================================
class NotEqual: public Operator
{
public:
    static const string str;
    

    NotEqual(): Operator(2, 6, LEFT) {}
    
    Value* execute(vector<Value*> vals)
    {
        check_operands_num(vals.size(), str);
        return &(*vals[0] != *vals[1]);
    }
};


//==========================================================================================================
//==========================================================================================================
class LessThan: public Operator
{
public:
    static const string str;
    

    LessThan(): Operator(2, 5, NONE) {}
    
    Value* execute(vector<Value*> vals)
    {
        check_operands_num(vals.size(), str);
        return &(*vals[0] < *vals[1]);
    }
};


//==========================================================================================================
//==========================================================================================================
class GreaterThan: public Operator
{
public:
    static const string str;
    

    GreaterThan(): Operator(2, 5, NONE) {}
    
    Value* execute(vector<Value*> vals)
    {
        check_operands_num(vals.size(), str);
        return &(*vals[0] > *vals[1]);
    }
};


//==========================================================================================================
//==========================================================================================================
class LessThanEqual: public Operator
{
public:
    static const string str;
    

    LessThanEqual(): Operator(2, 5, NONE) {}
    
    Value* execute(vector<Value*> vals)
    {
        check_operands_num(vals.size(), str);
        return &(*vals[0] <= *vals[1]);
    }
};


//==========================================================================================================
//==========================================================================================================
class GreaterThanEqual: public Operator
{
public:
    static const string str;
    

    GreaterThanEqual(): Operator(2, 5, NONE) {}
    
    Value* execute(vector<Value*> vals)
    {
        check_operands_num(vals.size(), str);
        return &(*vals[0] >= *vals[1]);
    }
};


//==========================================================================================================
//==========================================================================================================
class Match: public Operator
{
public:
    static const string str;
    

    Match(): Operator(2, 4, NONE) {}

    Value* execute(vector<Value*> vals)
    {
        check_operands_num(vals.size(), str);
        return &(vals[0]->match(*vals[1]));
    }
};


//==========================================================================================================
//==========================================================================================================
class NotMatch: public Operator
{
public:
    static const string str;
    

    NotMatch(): Operator(2, 4, NONE) {}
    
    Value* execute(vector<Value*> vals)
    {
        check_operands_num(vals.size(), str);
        return &(vals[0]->not_match(*vals[1]));
    }
};


#endif /* Operator_h */
