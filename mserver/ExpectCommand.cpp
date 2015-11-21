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
//==========================================================================================================
void ExpectCommand::interpret(string &line, ifstream &file)
{
    vector<Token*> tokens;
    convert_to_tokens(line, file, tokens);
}


//==========================================================================================================
//==========================================================================================================
void ExpectCommand::convert_to_tokens(string &line, ifstream &file, vector<Token*> tokens)
{
    for(bool stop = false;; getline(file, line))
    {
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
}


//==========================================================================================================
// Replace:
// Header names --> last message header value
// [var]        --> reader.get_value()
// But don't replace inside strings ("...[not-replaced]...")
//==========================================================================================================
void ExpectCommand::convert_to_tokens(string &line, vector<Token*> tokens)
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
        string s = line.substr(0, i);
        
        if(const_tokens.count(s) > 0)
        {
            pos += i;
            return const_tokens[s];
        }
    }
    
    return nullptr;
}


//==========================================================================================================
//==========================================================================================================
Token* ExpectCommand::try_num(string &line, int &pos)
{
    smatch match;
    
    if(regex_search(line, match, regex("^\\d+")))
    {
        pos += match.length();
        return new Int(stoi(match.str()));
    }
    
    return nullptr;
}


//==========================================================================================================
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
    
    if(regex_search(line, match, regex("^" + SipParser::inst().header_name_str)))
    {
        pos += match.length();
        
        // Format for query is last_<header-name>_value
        return new String(reader.get_value("last_" + match.str() + "_value"));
    }

    return nullptr;
}


//==========================================================================================================
//==========================================================================================================
Token* ExpectCommand::try_var(string &line, int &pos)
{
    smatch match;
    
    if(regex_search(line, match, regex("^\\w+")))
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

























