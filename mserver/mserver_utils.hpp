//
//  mserver_utils.hpp
//  mserver
//
//  Created by Yuval Dinari on 12/8/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef mserver_utils_hpp
#define mserver_utils_hpp

#include "OptionParser.hpp"
#include "bimap.h"

enum Direction {IN, OUT};

extern const string string_regex_str;
extern const string var_regex_str;
extern const string brackets_var_regex_str;

string unquote(string s);
int find_end_str_position(string& line, string end_str);
bool stob(string);
void trim(string &line);

#endif /* mserver_utils_hpp */
