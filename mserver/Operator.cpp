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

const string Add::str = "+";
const string Subtract::str = "-";
const string Div::str = "/";
const string Mul::str = "*";
const string Mod::str = "%";
const string Or::str = "||";
const string And::str = "&&";
const string Not::str = "!";
const string Equal::str = "==";
const string NotEqual::str = "!=";
const string LessThan::str = "<";
const string GreaterThan::str = ">";
const string LessThanEqual::str = "<=";
const string GreaterThanEqual::str = ">=";
const string Match::str = "~";
const string NotMatch::str = "!~";
