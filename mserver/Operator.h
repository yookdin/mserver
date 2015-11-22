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
// Note: shouldn't have operators of the same precedence with mixed associativity, can cause weird behavior

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
    
    virtual Value* execute(vector<Value*> operands) = 0;
    
protected:
    void check_operands_num(long num, string str)
    {
        if(num != num_operands)
        {
            throw string("Wrong num of operands for " + str + ", expected " + std::to_string(num_operands) + ", got " + std::to_string(num));
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
    
    Value* execute(vector<Value*> operands)
    {
        check_operands_num(operands.size(), str);
        return &(*operands[0] + *operands[1]);
    }
    
    string to_string() { return str; }
};


//==========================================================================================================
//==========================================================================================================
class Subtract: public Operator
{
public:
    static const string str;
    
    Subtract(): Operator(2, 3, LEFT) {}
    
    Value* execute(vector<Value*> operands)
    {
        check_operands_num(operands.size(), str);
        return &(*operands[0] - *operands[1]);
    }

    string to_string() { return str; }
};


//==========================================================================================================
//==========================================================================================================
class Mul: public Operator
{
public:
    static const string str;
    
    Mul(): Operator(2, 2, LEFT) {}
    
    Value* execute(vector<Value*> operands)
    {
        check_operands_num(operands.size(), str);
        return &(*operands[0] * *operands[1]);
    }

    string to_string() { return str; }
};


//==========================================================================================================
//==========================================================================================================
class Div: public Operator
{
public:
    static const string str;
    
    Div(): Operator(2, 2, LEFT) {}
    
    Value* execute(vector<Value*> operands)
    {
        check_operands_num(operands.size(), str);
        return &(*operands[0] / *operands[1]);
    }

    string to_string() { return str; }
};


//==========================================================================================================
//==========================================================================================================
class Mod: public Operator
{
public:
    static const string str;

    Mod(): Operator(2, 2, LEFT) {}
    
    Value* execute(vector<Value*> operands)
    {
        check_operands_num(operands.size(), str);
        return &(*operands[0] % *operands[1]);
    }

    string to_string() { return str; }
};


//==========================================================================================================
//==========================================================================================================
class Or: public Operator
{
public:
    static const string str;

    Or(): Operator(2, 8, LEFT) {}
    
    Value* execute(vector<Value*> operands)
    {
        check_operands_num(operands.size(), str);
        return &(*operands[0] || *operands[1]);
    }

    string to_string() { return str; }
};


//==========================================================================================================
//==========================================================================================================
class And: public Operator
{
public:
    static const string str;

    And(): Operator(2, 7, LEFT) {}
    
    Value* execute(vector<Value*> operands)
    {
        check_operands_num(operands.size(), str);
        return &(*operands[0] && *operands[1]);
    }

    string to_string() { return str; }
};


//==========================================================================================================
//==========================================================================================================
class Not: public Operator
{
public:
    static const string str;

    Not(): Operator(1, 1, RIGHT) {}
    
    Value* execute(vector<Value*> operands)
    {
        check_operands_num(operands.size(), str);
        return &(!(*operands[0]));
    }

    string to_string() { return str; }
};


//==========================================================================================================
//==========================================================================================================
class Equal: public Operator
{
public:
    static const string str;
    
    Equal(): Operator(2, 6, LEFT) {}
    
    Value* execute(vector<Value*> operands)
    {
        check_operands_num(operands.size(), str);
        return &(*operands[0] == *operands[1]);
    }

    string to_string() { return str; }
};


//==========================================================================================================
//==========================================================================================================
class NotEqual: public Operator
{
public:
    static const string str;
    
    NotEqual(): Operator(2, 6, LEFT) {}
    
    Value* execute(vector<Value*> operands)
    {
        check_operands_num(operands.size(), str);
        return &(*operands[0] != *operands[1]);
    }

    string to_string() { return str; }
};


//==========================================================================================================
//==========================================================================================================
class LessThan: public Operator
{
public:
    static const string str;

    LessThan(): Operator(2, 5, NONE) {}
    
    Value* execute(vector<Value*> operands)
    {
        check_operands_num(operands.size(), str);
        return &(*operands[0] < *operands[1]);
    }

    string to_string() { return str; }
};


//==========================================================================================================
//==========================================================================================================
class GreaterThan: public Operator
{
public:
    static const string str;

    GreaterThan(): Operator(2, 5, NONE) {}
    
    Value* execute(vector<Value*> operands)
    {
        check_operands_num(operands.size(), str);
        return &(*operands[0] > *operands[1]);
    }

    string to_string() { return str; }
};


//==========================================================================================================
//==========================================================================================================
class LessThanEqual: public Operator
{
public:
    static const string str;
    
    LessThanEqual(): Operator(2, 5, NONE) {}
    
    Value* execute(vector<Value*> operands)
    {
        check_operands_num(operands.size(), str);
        return &(*operands[0] <= *operands[1]);
    }

    string to_string() { return str; }
};


//==========================================================================================================
//==========================================================================================================
class GreaterThanEqual: public Operator
{
public:
    static const string str;

    GreaterThanEqual(): Operator(2, 5, NONE) {}
    
    Value* execute(vector<Value*> operands)
    {
        check_operands_num(operands.size(), str);
        return &(*operands[0] >= *operands[1]);
    }

    string to_string() { return str; }
};


//==========================================================================================================
//==========================================================================================================
class Match: public Operator
{
public:
    static const string str;
    
    Match(): Operator(2, 4, NONE) {}

    Value* execute(vector<Value*> operands)
    {
        check_operands_num(operands.size(), str);
        return &(operands[0]->match(*operands[1]));
    }

    string to_string() { return str; }
};


//==========================================================================================================
//==========================================================================================================
class NotMatch: public Operator
{
public:
    static const string str;
    
    NotMatch(): Operator(2, 4, NONE) {}
    
    Value* execute(vector<Value*> operands)
    {
        check_operands_num(operands.size(), str);
        return &(operands[0]->not_match(*operands[1]));
    }

    string to_string() { return str; }
};


#endif /* Operator_h */
