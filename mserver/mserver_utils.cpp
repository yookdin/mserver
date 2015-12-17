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

string unquote(string s)
{
    if(s.front() == '"' && s.back() == '"')
    {
        s.erase(0,1);
        s.erase(s.length() - 1);
    }
    
    return s;
}


//==========================================================================================================
// Return the start positon of the first occurrence not inside double quotes of end_str
//==========================================================================================================
int find_end_str_position(string& line, string end_str)
{
    bool inside_quotes = false;
    
    for(int i = 0; i < line.length(); i++)
    {
        if(line[i] == '"' && (i == 0 || line[i-1] != '\\'))
        {
            inside_quotes = !inside_quotes;
        }
        
        if(inside_quotes)
        {
            continue; // Ignore everything inside quotes
        }
        
        if(strncmp(line.c_str() + i, end_str.c_str(), end_str.length()) == 0)
        {
            return i;
        }
    }
    
    return -1;
}
