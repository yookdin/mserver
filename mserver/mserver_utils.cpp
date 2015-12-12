//
//  mserver_utils.cpp
//  mserver
//
//  Created by Yuval Dinari on 12/8/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "mserver_utils.hpp"

// Yes, all these backslashes are needed, because after the normal replacement, the regex does another one it seems
const string string_regex_str = "\"((\\\\\"|[^\"])*)\""; // 2 submatches
const string var_regex_str = "(last_)?(((" + string(CSEQ) + ")\\+(\\d+))|([-\\w]+))(:value)?"; // 7 submatches
const string brackets_var_regex_str = "\\[" + var_regex_str + "\\]"; // 7 submatches

string unqoute(string s)
{
    if(s.front() == '"' && s.back() == '"')
    {
        s.erase(0,1);
        s.erase(s.length() - 1);
    }
    
    return s;
}
