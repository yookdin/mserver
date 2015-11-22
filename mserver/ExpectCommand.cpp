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
const regex ExpectCommand::end_regex("</expect>");

//==========================================================================================================
// Map of static tokens (ones without values)
//==========================================================================================================
map<string, Token*> ExpectCommand::const_tokens = ExpectCommand::init_const_tokens();


//==========================================================================================================
//==========================================================================================================
map<string, Token*> ExpectCommand::init_const_tokens()
{
    map<string, Token*> tokens;
    tokens[OpenParen::str] = new OpenParen;
    tokens[CloseParen::str] = new CloseParen;
    tokens[Add::str] = new Add;
    tokens[Subtract::str] = new Subtract;
    tokens[Div::str] = new Div;
    tokens[Mul::str] = new Mul;
    tokens[Mod::str] = new Mod;
    tokens[Equal::str] = new Equal;
    tokens[NotEqual::str] = new NotEqual;
    tokens[LessThan::str] = new LessThan;
    tokens[GreaterThan::str] = new GreaterThan;
    tokens[LessThanEqual::str] = new LessThanEqual;
    tokens[GreaterThanEqual::str] = new GreaterThanEqual;
    tokens[Or::str] = new Or;
    tokens[And::str] = new And;
    tokens[Not::str] = new Not;
    tokens[Match::str] = new Match;
    tokens[NotMatch::str] = new NotMatch;
    
    return tokens;
}


//==========================================================================================================
// Parse and evaluate the given expression. If it evaluates to false throw an error
//==========================================================================================================
void ExpectCommand::interpret(string &line, ifstream &file)
{
    vector<Token*> tokens;
    deque<Token*> output;
    convert_to_tokens(line, file, tokens);
    convert_to_rpn(tokens, output);
    
    if(!eval_expression(output))
    {
        throw string("Expected condition didn't occur");
    }
    
//    for(auto token: tokens)
//    {
//        delete token;
//    }
}


//==========================================================================================================
// Convert text to tokens object for the evaluation process
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
        token->print();
        cout << " ";
    }
    cout << endl;
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
        // o must try header-name before var, o/w the header name or part of it may be interpreted as var
        if((token = try_num(line, pos)) != nullptr ||
           (token = try_const_token(line, pos)) != nullptr ||
           (token = try_string(line, pos)) != nullptr ||
           (token = try_header_name(line, pos)) != nullptr ||
           (token = try_var(line, pos)) != nullptr)
        {
            tokens.push_back(token);
        }
        else
        {
            throw string("Unknown element: " + line.substr(pos));
        }
    }
}

//==========================================================================================================
// Const tokens are operators (+, -, ==, etc.) of parentheses
//==========================================================================================================
Token* ExpectCommand::try_const_token(string &line, int &pos)
{
    // Const token (operator or parenthesis) symbol may be one or two characters
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
// Try interpreting the current element as a number
//==========================================================================================================
Token* ExpectCommand::try_num(string &line, int &pos)
{
    smatch match;
    
    if(regex_search(line.substr(pos), match, regex("^-?\\d+")))
    {
        pos += match.length();
        return new Int(stoi(match.str()));
    }
    
    return nullptr;
}


//==========================================================================================================
// Try interpreting the current element as a string
//==========================================================================================================
Token* ExpectCommand::try_string(string &line, int &pos)
{
    int start = pos + 1;
    
    if(line[pos] == '"')
    {
        for(++pos; pos < line.length(); ++pos)
        {
            if(line[pos] == '"' && line[pos-1] != '\\')
            {
                break;
            }
        }
        
        if(line[pos] != '"')
        {
            throw string("Unterminated string");
        }
        
        Token* res = new String(line.substr(start, pos - start));
        pos++; // Skip over last "
        return res;
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
    
    if(regex_search(line.substr(pos), match, regex("^" + SipParser::inst().header_name_str)))
    {
        pos += match.length();
        
        // Format for query is last_<header-name>_value
        return new String(reader.get_value("last_" + match.str() + "_value"));
    }

    return nullptr;
}


//==========================================================================================================
// Try interpreting the current element as a variable name
//==========================================================================================================
Token* ExpectCommand::try_var(string &line, int &pos)
{
    smatch match;
    
    if(regex_search(line.substr(pos), match, regex("^\\w+")))
    {
        pos += match.length();
        string val = reader.get_value(match.str());
        
        // Guess the type of the value returned
        if(regex_match(val, match, regex("\\d+")))
        {
            return new Int(stoi(val));
        }
        else if(regex_match(val, match, regex("true|false")))
        {
            return new Bool(val == "true");
        }
        else
        {
            return new String(val);
        }
    }

    return nullptr;
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
                
                ops.pop(); // Pop the left-paren that's at the top
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
        token->print();
        cout << " ";
    }
    cout << endl;

    
} // convert_to_rpn()


//==========================================================================================================
// Expression is expected to be boolean, evaluate it and return its value.
//==========================================================================================================
bool ExpectCommand::eval_expression(deque<Token*>& output)
{
    stack<Value*> values;
    
    for(; !output.empty(); output.pop_front())
    {
        if(output.front()->kind == VALUE)
        {
            values.push((Value*)output.front());
        }
        else // Token is an operator
        {
            Operator* op = (Operator*)output.front();
            
            if(values.size() < op->num_operands)
            {
                throw string("Not enough operands for operator " + op->get_symbol());
            }
            
            vector<Value*> operands;

            for(int i = 0; i < op->num_operands; ++i)
            {
                operands.insert(operands.begin(), values.top()); // Return operands to original order
                values.pop();
            }
            
            values.push(op->execute(operands)); // Return the value of the operation to the stack
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
    
    return values.top()->get_bool();
}



































