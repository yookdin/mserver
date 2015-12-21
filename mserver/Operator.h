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
    enum Associativity {LEFT, RIGHT, NONE};
    typedef Operator::Associativity Associativity;
    
    Operator(int _num_operands, int _precedence, Associativity _associativity, string _str):
    Token(OP), num_operands(_num_operands), precedence(_precedence), associativity(_associativity), str(_str) {}
    
    static const string add_str, sub_str, mul_str, div_str, mod_str, or_str, and_str, not_str, equal_str, not_equal_str,
    less_than_str, greater_than_str, lte_str, gte_str, match_str, no_match_str, ternary_if_str, ternary_else_str;
    
    const string str;
    const int num_operands;
    const int precedence;
    const Associativity associativity;
    
    virtual string to_string() { return str; }
    
    //======================================================================================================
    //======================================================================================================
    Value* eval(vector<Value*> operands)
    {
        if(operands.size() != num_operands)
        {
            throw string("Wrong num of operands for " + str + ", expected " + std::to_string(num_operands) + ", got " + std::to_string(operands.size()));
        }
        
        return execute(operands);
    }
    
protected:
    virtual Value* execute(vector<Value*> operands) = 0;
};

//==========================================================================================================
//==========================================================================================================
class Add: public Operator
{
public:
    Add(): Operator(2, 3, LEFT, Operator::add_str) {}
    
protected:
    Value* execute(vector<Value*> operands)
    {
        return &(*operands[0] + *operands[1]);
    }
};


//==========================================================================================================
//==========================================================================================================
class Subtract: public Operator
{
public:
    Subtract(): Operator(2, 3, LEFT, Operator::sub_str) {}
    
protected:
    Value* execute(vector<Value*> operands)
    {
        return &(*operands[0] - *operands[1]);
    }
};


//==========================================================================================================
//==========================================================================================================
class Mul: public Operator
{
public:
    Mul(): Operator(2, 2, LEFT, Operator::mul_str) {}
    
protected:
    Value* execute(vector<Value*> operands)
    {
        return &(*operands[0] * *operands[1]);
    }
};


//==========================================================================================================
//==========================================================================================================
class Div: public Operator
{
public:
    Div(): Operator(2, 2, LEFT, Operator::div_str) {}
    
protected:
    Value* execute(vector<Value*> operands)
    {
        return &(*operands[0] / *operands[1]);
    }
};


//==========================================================================================================
//==========================================================================================================
class Mod: public Operator
{
public:
    static const string str;
    
    Mod(): Operator(2, 2, LEFT, Operator::mod_str) {}
    
protected:
    Value* execute(vector<Value*> operands)
    {
        return &(*operands[0] % *operands[1]);
    }
};


//==========================================================================================================
//==========================================================================================================
class Or: public Operator
{
public:
    static const string str;
    
    Or(): Operator(2, 8, LEFT, Operator::or_str) {}
    
protected:
    Value* execute(vector<Value*> operands)
    {
        return &(*operands[0] || *operands[1]);
    }
};


//==========================================================================================================
//==========================================================================================================
class And: public Operator
{
public:
    static const string str;
    
    And(): Operator(2, 7, LEFT, Operator::and_str) {}
    
protected:
    Value* execute(vector<Value*> operands)
    {
        return &(*operands[0] && *operands[1]);
    }
};


//==========================================================================================================
//==========================================================================================================
class Not: public Operator
{
public:
    static const string str;
    
    Not(): Operator(1, 1, RIGHT, Operator::not_str) {}
    
protected:
    Value* execute(vector<Value*> operands)
    {
        return &(!(*operands[0]));
    }
};


//==========================================================================================================
//==========================================================================================================
class Equal: public Operator
{
public:
    Equal(): Operator(2, 6, LEFT, Operator::equal_str) {}
    
protected:
    Value* execute(vector<Value*> operands)
    {
        return &(*operands[0] == *operands[1]);
    }
};


//==========================================================================================================
//==========================================================================================================
class NotEqual: public Operator
{
public:
    NotEqual(): Operator(2, 6, LEFT, Operator::not_equal_str) {}
    
protected:
    Value* execute(vector<Value*> operands)
    {
        return &(*operands[0] != *operands[1]);
    }
};


//==========================================================================================================
//==========================================================================================================
class LessThan: public Operator
{
public:
    static const string str;
    
    LessThan(): Operator(2, 5, NONE, Operator::less_than_str) {}
    
protected:
    Value* execute(vector<Value*> operands)
    {
        return &(*operands[0] < *operands[1]);
    }
};


//==========================================================================================================
//==========================================================================================================
class GreaterThan: public Operator
{
public:
    static const string str;
    
    GreaterThan(): Operator(2, 5, NONE, Operator::greater_than_str) {}
    
protected:
    Value* execute(vector<Value*> operands)
    {
        return &(*operands[0] > *operands[1]);
    }
};


//==========================================================================================================
//==========================================================================================================
class LessThanEqual: public Operator
{
public:
    LessThanEqual(): Operator(2, 5, NONE, Operator::lte_str) {}
    
protected:
    Value* execute(vector<Value*> operands)
    {
        return &(*operands[0] <= *operands[1]);
    }
};


//==========================================================================================================
//==========================================================================================================
class GreaterThanEqual: public Operator
{
public:
    static const string str;
    
    GreaterThanEqual(): Operator(2, 5, NONE, Operator::gte_str) {}
    
protected:
    Value* execute(vector<Value*> operands)
    {
        return &(*operands[0] >= *operands[1]);
    }
};


//==========================================================================================================
// String matching
//==========================================================================================================
class Match: public Operator
{
public:
    Match(): Operator(2, 4, NONE, Operator::match_str) {}
    
protected:
    Value* execute(vector<Value*> operands)
    {
        return &(operands[0]->match(*operands[1]));
    }
};


//==========================================================================================================
// No string match
//==========================================================================================================
class NotMatch: public Operator
{
public:
    NotMatch(): Operator(2, 4, NONE, Operator::no_match_str) {}
    
protected:
    Value* execute(vector<Value*> operands)
    {
        return &(operands[0]->not_match(*operands[1]));
    }
};


//==========================================================================================================
// The "?" of the ternary conditional operator (x ? y : z). Only this operator will be used in the output
// queue of a conversion to RPN of the original expression.
//==========================================================================================================
class TernaryIf: public Operator
{
public:
    TernaryIf(): Operator(3, 10, RIGHT, Operator::ternary_if_str) {}
    
protected:
    Value* execute(vector<Value*> operands)
    {
        return *operands[0] ? operands[1] : operands[2];
    }
};


//==========================================================================================================
// The ":" of the ternary conditional operator (x ? y : z). Will not be used in the output queue of a
// conversion to RPN of the original expression. It serves only as a separator between the if and else parts
// of the operator.
//==========================================================================================================
class TernaryElse: public Operator
{
public:
    TernaryElse(): Operator(0, 9, NONE, Operator::ternary_else_str) {}
    
protected:
    Value* execute(vector<Value*> operands)
    {
        throw string("TernaryElse::execute() should never be called!");
    }
};






#endif /* Operator_h */
