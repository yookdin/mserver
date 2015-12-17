//
//  ExpressionEvaluator.cpp
//  mserver
//
//  Created by Yuval Dinari on 12/17/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "ExpressionEvaluator.hpp"
#include "Int.h"
#include "Bool.h"
#include "String.h"
#include "Operator.h"
#include "Paren.h"
#include "ScriptReader.h"


//==========================================================================================================
//==========================================================================================================
ExpressionEvaluator* ExpressionEvaluator::the_inst = nullptr;

//==========================================================================================================
//==========================================================================================================
ExpressionEvaluator* ExpressionEvaluator::inst()
{
    if(the_inst == nullptr)
    {
        the_inst = new ExpressionEvaluator();
    }
    
    return the_inst;
}


//==========================================================================================================
//==========================================================================================================
ExpressionEvaluator::ExpressionEvaluator():
    num_regex("^-?\\d+"),
    bool_regex("^true|false"),
    str_regex("^(" + string_regex_str + "|[\\w.]+)")  // Note: '.' inside brackets is literal
{
    // Map of constant tokens (ones without values)
    const_tokens[OpenParen::str] = new OpenParen;
    const_tokens[CloseParen::str] = new CloseParen;
    const_tokens[Add::str] = new Add;
    const_tokens[Subtract::str] = new Subtract;
    const_tokens[Div::str] = new Div;
    const_tokens[Mul::str] = new Mul;
    const_tokens[Mod::str] = new Mod;
    const_tokens[Equal::str] = new Equal;
    const_tokens[NotEqual::str] = new NotEqual;
    const_tokens[LessThan::str] = new LessThan;
    const_tokens[GreaterThan::str] = new GreaterThan;
    const_tokens[LessThanEqual::str] = new LessThanEqual;
    const_tokens[GreaterThanEqual::str] = new GreaterThanEqual;
    const_tokens[Or::str] = new Or;
    const_tokens[And::str] = new And;
    const_tokens[Not::str] = new Not;
    const_tokens[Match::str] = new Match;
    const_tokens[NotMatch::str] = new NotMatch;
}


//==========================================================================================================
// Evaluate an expression that is expected to be boolean
//==========================================================================================================
bool ExpressionEvaluator::eval_bool(string& exp, ScriptReader* reader)
{
    Value* val = eval(exp, reader);
    
    if(val->get_type() != BOOL)
    {
        throw string("Expected expression type to be BOOL but found " + val->get_type_name());
    }

    return val->get_bool();
}


//==========================================================================================================
// Evaluate an expression that is expected to be integer
//==========================================================================================================
int ExpressionEvaluator::eval_int(string& exp, ScriptReader* reader)
{
    Value* val = eval(exp, reader);
    
    if(val->get_type() != INT)
    {
        throw string("Expected expression type to be INT but found " + val->get_type_name());
    }
    
    return val->get_int();
}


//==========================================================================================================
// Evaluate an expression that is expected to be string
//==========================================================================================================
string ExpressionEvaluator::eval_string(string& exp, ScriptReader* reader)
{
    Value* val = eval(exp, reader);
    
    if(val->get_type() != STRING)
    {
        throw string("Expected expression type to be STRING but found " + val->get_type_name());
    }
    
    return val->get_string();
}


//==========================================================================================================
// Evaluate expression not caring about its type, and return its string representation
//==========================================================================================================
string ExpressionEvaluator::eval_as_string(string& exp, ScriptReader* reader)
{
    Value* val = eval(exp, reader);
    return val->to_string();
}


//==========================================================================================================
//==========================================================================================================
Value* ExpressionEvaluator::eval(string& exp, ScriptReader* _reader)
{
    reader = _reader;
    vector<Token*> tokens;
    deque<Token*> output;
    convert_to_tokens(exp, tokens);
    convert_to_rpn(tokens, output);
    return eval_rpn(output);
}


//==========================================================================================================
//==========================================================================================================
void ExpressionEvaluator::convert_to_tokens(string &exp, vector<Token*>& tokens)
{
    Token* token;
    int pos = 0;
    
    while(pos < exp.length())
    {
        if(isspace(exp[pos]))
        {
            ++pos;
            continue;
        }
        
        // Note:
        // o must try num before operator, o/w in "-3" the minus sign will be interpreted as a
        //   subtraction operator
        // o must try bool before string, o/w true|false will be interpreted as string
        if((token = try_num         (exp, pos)) != nullptr ||
           (token = try_const_token (exp, pos)) != nullptr ||
           (token = try_bool        (exp, pos)) != nullptr ||
           (token = try_string      (exp, pos)) != nullptr ||
           (token = try_var         (exp, pos)) != nullptr)
        {
            tokens.push_back(token);
        }
        else
        {
            throw string("Unknown element at: " + exp.substr(pos));
        }
    }
}


//==========================================================================================================
// Convert original order to RPN = Reverse Polish Notation, meaaning that the operator follows its operands:
// 3 + 4 --> 3 4 +
// This is the shunting-yard-algorithm.
//==========================================================================================================
void ExpressionEvaluator::convert_to_rpn(vector<Token*>& tokens, deque<Token*>& output)
{
    stack<Token*> ops;
    
    for(auto token: tokens)
    {
        switch (token->kind) {
            case VALUE: // Values are immediately added to the output stack
            {
                output.push_back(token);
                break;
            }
            case OP: // Operator will cause the pop of previous operators if they should be applied before it
            {
                Operator* op = (Operator*)token;
                
                while(!ops.empty() && ops.top()->kind == OP)
                {
                    Operator* top = (Operator*)ops.top();
                    
                    // Lower precedence means apply first!
                    if(top->precedence < op->precedence ||
                       (top->precedence == op->precedence && top->associativiy == Operator::LEFT))
                    {
                        output.push_back(ops.top());
                        ops.pop();
                    }
                    else
                    {
                        break;
                    }
                }
                
                ops.push(op);
                break;
            }
            case OPEN_PAREN: // Push to the stack to mark the beginning of the expression inside the parentheses
            {
                ops.push(token);
                break;
            }
            // Close of parenthesised expression: all operators in the stack should be applied before those not read yet,
            // and those before the start of the parenthesised expression, so pop them out and add to output
            case CLOSE_PAREN:
            {
                while(!ops.empty() && ops.top()->kind == OP)
                {
                    output.push_back(ops.top());
                    ops.pop();
                }
                
                if(ops.empty())
                {
                    throw string("Missing open parenthesis");
                }
                
                ops.pop(); // Pop the left-paren that's at the top (the ops stack contains only operators and left-paren tokens)
                break;
            }
        } // switch token->kind
    } // for tokens
    
    while(!ops.empty())
    {
        output.push_back(ops.top());
        ops.pop();
    }
    
    if(output.empty())
    {
        throw string("Empty expression");
    }
    
    if(output.back()->kind == OPEN_PAREN)
    {
        throw string("Missing close parenthesis");
    }
    
    
    string expression_rpn;
    
    for(auto token: output)
    {
        expression_rpn += token->to_string() + " ";
    }
    
    //cout << expression_rpn << endl;
    
} // convert_to_rpn


//==========================================================================================================
// Try interpreting the current element as a number
//==========================================================================================================
Token* ExpressionEvaluator::try_num(string &exp, int &pos)
{
    smatch match;
    
    if(regex_search(exp.cbegin() + pos, exp.cend(), match, num_regex))
    {
        pos += match.length();
        return new Int(stoi(match.str()));
    }
    
    return nullptr;
}


//==========================================================================================================
//==========================================================================================================
Token* ExpressionEvaluator::try_bool(string& exp, int& pos)
{
    smatch match;
    
    if(regex_search(exp.cbegin() + pos, exp.cend(), match, bool_regex))
    {
        pos += match.length();
        return new Bool(match.str());
    }
    
    return nullptr;
}

//==========================================================================================================
// Const tokens are operators (+, -, ==, etc.) or parentheses
//==========================================================================================================
Token* ExpressionEvaluator::try_const_token(string &exp, int &pos)
{
    // Const token symbol may be one or two characters
    // Note: must check the two chars option first, because an operator can be a prefix of another operator,
    // like < and <=
    for(int i = 2; i > 0; --i)
    {
        string s = exp.substr(pos, i);
        
        if(const_tokens.count(s) > 0)
        {
            pos += i;
            return const_tokens[s];
        }
    }
    
    return nullptr;
}


//==========================================================================================================
// Try interpreting the current element as a string. A string doens't have to be quoted. Any sequence of
// word characters and dots is considered a string.
//==========================================================================================================
Token* ExpressionEvaluator::try_string(string &exp, int &pos)
{
    smatch match;
    
    if(regex_search(exp.cbegin() + pos, exp.cend(), match, str_regex))
    {
        pos += match.length();
        return new String(unquote(match.str())); // match[1] is without the " at the beginning and end
    }
    
    return nullptr;
}


//==========================================================================================================
// Try interpreting the current element as a variable name
//==========================================================================================================
Token* ExpressionEvaluator::try_var(string &exp, int &pos)
{
    smatch match;
    
    if(regex_search(exp.cbegin() + pos, exp.cend(), match, ScriptReader::script_var_regex))
    {
        pos += match.length();
        
        // Third parameter 'true' means: if variable not found try to interpret it as last_<var>
        string val = reader->get_value(match[1], -1, true);
        return interpret_value(val);
    }
    
    return nullptr;
}


//==========================================================================================================
// Figure out the type of a value returned from a variable or header replacement and return appropriate
// token.
//==========================================================================================================
Token* ExpressionEvaluator::interpret_value(string& val)
{
    if(regex_match(val, num_regex))
    {
        return new Int(stoi(val));
    }
    else if(regex_match(val, bool_regex))
    {
        return new Bool(val);
    }
    else
    {
        return new String(val);
    }
}


//==========================================================================================================
// The output queue contains the tokens in reverse polish notation (operator follows its operands - "3 4 +")
// The algorithm for evalutating it:
// o Read a token from the queue
// o If it's a value: push it on a temp stack
// o If it's an operator: pop as many values as the operator needs out of stack, apply the operator on them,
//   and return the result to the stack (for use by other operators not applied yet)
// o After reading all the tokens from the output a single value should remain in the stack, which is the
//   the result of the entire expression
//==========================================================================================================
Value* ExpressionEvaluator::eval_rpn(deque<Token*>& output)
{
    stack<Value*> values;
    
    for(; !output.empty(); output.pop_front())
    {
        if(output.front()->kind == VALUE) // Add to stack
        {
            values.push((Value*)output.front());
        }
        else // Token is an operator: pop operands from stack, apply operator on them, return value to the stack
        {
            Operator* op = (Operator*)output.front();
            
            if(values.size() < op->num_operands)
            {
                throw string("Not enough operands for operator " + op->to_string());
            }
            
            vector<Value*> operands;
            
            for(int i = 0; i < op->num_operands; ++i)
            {
                operands.insert(operands.begin(), values.top()); // Return operands to original order
                values.pop();
            }
            
            values.push(op->execute(operands)); // Return the value of the operation to the stack
            
            // The operands have been used and are no longer needed, delete them;
            // Note: don't delete ops - they're static and will be reused
            for(auto val: operands)
            {
                delete val;
            }
        }
    } // for token in output
    
    if(values.size() != 1) // At the end only one value should be left
    {
        throw string("Expression syntax error: consecutive expressions");
    }
    
    return values.top();
}


































