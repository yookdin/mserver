//
//  Operator.cpp
//  mserver
//
//  Created by Yuval Dinari on 11/21/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "common.h"
#include "Operator.h"

// Make these static to enable mapping between the string identifier and the operator object
// (o/w it won't be availabe until the object is created)


const string Operator::add_str = "+";
const string Operator::sub_str = "-";
const string Operator::mul_str = "*";
const string Operator::div_str = "/";
const string Operator::mod_str = "%";
const string Operator::or_str = "||";
const string Operator::and_str = "&&";
const string Operator::not_str = "!";
const string Operator::equal_str = "==";
const string Operator::not_equal_str = "!=";
const string Operator::less_than_str = "<";
const string Operator::greater_than_str = ">";
const string Operator::lte_str = "<=";
const string Operator::gte_str = ">=";
const string Operator::match_str = "~";
const string Operator::no_match_str = "!~";
const string Operator::ternary_if_str = "?";
const string Operator::ternary_else_str = ":";
