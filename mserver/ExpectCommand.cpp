//
//  ExpectCommand.cpp
//  mserver
//
//  Created by Yuval Dinari on 11/19/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "SipParser.hpp"
#include "ExpectCommand.hpp"
#include "Operator.h"
#include "Paren.h"
#include "ScriptReader.h"


//==========================================================================================================
//==========================================================================================================
ExpectCommand::ExpectCommand():
    end_regex("</expect>"),
    num_regex("^-?\\d+"),
    var_regex("^\\w+"),
    bool_regex("^true|false"),
    str_regex("^\"(\\\\\"|[^\"])*\"") // Yes, all these backslashes are needed, because after the normal replacement, the regex does another one it seems
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
// Parse and evaluate the given expression. If it evaluates to false throw an error
//==========================================================================================================
void ExpectCommand::interpret(string &line, ifstream &file, ScriptReader &_reader)
{
    reader = &_reader;
    vector<Token*> tokens;
    deque<Token*> output;
    convert_to_tokens(line, file, tokens);
    convert_to_rpn(tokens, output);
    
    if(!eval_expression(output))
    {
        throw string("Expected condition: (" + expression + ") didn't occur");
    }
}


//==========================================================================================================
// Convert text to tokens objects for the evaluation process
//==========================================================================================================
void ExpectCommand::convert_to_tokens(string &line, ifstream &file, vector<Token*>& tokens)
{
    for(bool stop = false;; getline(file, line))
    {
        trim(line);
        smatch match;
        
        if(regex_search(line, match, end_regex))
        {
            line = match.prefix();
            stop = true;
        }
        
        convert_to_tokens(line, tokens);
        
        if(stop)
        {
            break; // Must break from within loop to not get the next line
        }
    }
    
    if(tokens.empty())
    {
        throw string("Empty expect command");
    }
    
    for(auto token: tokens)
    {
        expression += token->to_string() + " "; // For printing
    }
}


//==========================================================================================================
// Replace:
// Header names --> last message header value
// var          --> reader.get_value()
// But don't replace inside strings ("...var_name...")
//==========================================================================================================
void ExpectCommand::convert_to_tokens(string &line, vector<Token*>& tokens)
{
    Token* token;
    int pos = 0;
    
    while(pos < line.length())
    {
        if(isspace(line[pos]))
        {
            ++pos;
            continue;
        }
        
        // Note:
        // o must try num before operator, o/w in "-3" the minus sign will be interpreted as a
        //   subtraction operator
        // o must try bool before var, o/w true|false will be interpreted as var
        // o must try header-name before var, o/w the header name or part of it may be interpreted as var
        if((token = try_num         (line, pos)) != nullptr ||
           (token = try_const_token (line, pos)) != nullptr ||
           (token = try_bool        (line, pos)) != nullptr ||
           (token = try_string      (line, pos)) != nullptr ||
           (token = try_header_name (line, pos)) != nullptr ||
           (token = try_var         (line, pos)) != nullptr)
        {
            tokens.push_back(token);
        }
        else
        {
            throw string("Unknown element at: " + line.substr(pos));
        }
    }
}

//==========================================================================================================
// Try interpreting the current element as a number
//==========================================================================================================
Token* ExpectCommand::try_num(string &line, int &pos)
{
    smatch match;
    
    if(regex_search(line.substr(pos), match, num_regex))
    {
        pos += match.length();
        return new Int(stoi(match.str()));
    }
    
    return nullptr;
}


//==========================================================================================================
//==========================================================================================================
Token* ExpectCommand::try_bool(string& line, int& pos)
{
    smatch match;
    
    if(regex_search(line.substr(pos), match, bool_regex))
    {
        pos += match.length();
        return new Bool(match.str());
    }
    
    return nullptr;
}

//==========================================================================================================
// Const tokens are operators (+, -, ==, etc.) or parentheses
//==========================================================================================================
Token* ExpectCommand::try_const_token(string &line, int &pos)
{
    // Const token symbol may be one or two characters
    // Note: must check the two chars option first, because an operator can be a prefix of another operator,
    // like < and <=
    for(int i = 2; i > 0; --i)
    {
        string s = line.substr(pos, i);
        
        if(const_tokens.count(s) > 0)
        {
            pos += i;
            return const_tokens[s];
        }
    }
    
    return nullptr;
}


//==========================================================================================================
// Try interpreting the current element as a string
//==========================================================================================================
Token* ExpectCommand::try_string(string &line, int &pos)
{
    smatch match;

    if(regex_search(line.substr(pos), match, str_regex))
    {
        pos += match.length();
        
        // Get the string without the " at the beginning and end
        string full = match.str();
        return new String(string(full.begin() + 1, full.end() - 1));
    }
    
    return nullptr;
}


//==========================================================================================================
// Note: it may be possible to mistake an expression containing variable and subtract operator for a header
// name, for example:
// "Content-Length" : this could mean either the header with that name, or an expression of two variables
// named "Content" and "Length".
// This scenario isn't checked and it is assumed that it will never happen!
//==========================================================================================================
Token* ExpectCommand::try_header_name(string &line, int &pos)
{
    smatch match;
    
    if(regex_search(line.substr(pos), match, regex("^" + SipParser::inst().header_name_regex_str)))
    {
        pos += match.length();
        
        // Format for query is last_<header-name>:value
        string val = reader->get_value("last_" + match.str() + ":value");
        return interpret_value(val);
    }

    return nullptr;
}


//==========================================================================================================
// Try interpreting the current element as a variable name
//==========================================================================================================
Token* ExpectCommand::try_var(string &line, int &pos)
{
    smatch match;
    
    if(regex_search(line.substr(pos), match, var_regex))
    {
        pos += match.length();
        string val = reader->get_value(match.str());
        return interpret_value(val);
    }

    return nullptr;
}


//==========================================================================================================
// Figure out the type of a value returned from a variable or header replacement and return appropriate
// token.
//==========================================================================================================
Token* ExpectCommand::interpret_value(string& val)
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
// Convert original order to RPN = Reverse Polish Notation, meaaning that the operator follow its operands:
// 3 + 4 --> 3 4 +
// This is the shunting-yard-algorithm.
//==========================================================================================================
void ExpectCommand::convert_to_rpn(vector<Token*>& tokens, deque<Token*>& output)
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
             // Operator will cause the pop of previous operators if they should be applied before it
            case OP:
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
    
    for(auto token: output)
    {
        expression_rpn += token->to_string() + " ";
    }
    
} // convert_to_rpn()


//==========================================================================================================
// Expression is expected to be boolean, evaluate it and return its value.
// The output queue contains the tokens in reverse polish notation (operator follows its operands - "3 4 +")
// The algorithm for evalutating it:
// o Read a token from the queue
// o If it's a value: push it on a temp stack
// o If it's an operator: pop as many values as the operator needs out of stack, apply the operator on them,
//   and return the result to the stack (for use by other operators not applied yet)
// o After reading all the tokens from the output a single value should remain in the stack, which is the
//   the result of the entire expression
//==========================================================================================================
bool ExpectCommand::eval_expression(deque<Token*>& output)
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
            
            // Here is where the operands have been used and are no longer needed, delete them;
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
    
    if(values.top()->get_type() != BOOL)
    {
        throw string("Expression type should be BOOL and not " + values.top()->get_type_name());
    }
    
    bool result = values.top()->get_bool();
    delete values.top();
    return result;
}


//==========================================================================================================
//==========================================================================================================
string ExpectCommand::get_start_regex_str()
{
    return "<(expect)>";
}

































