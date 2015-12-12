

#include "Command.h"
#include "ScriptReader.h"


//==========================================================================================================
// Remove leading and trailing spaces. Remove C++ style comment: //... but only if preceded by space, o/w it
// may be part of SIP
//==========================================================================================================
void Command::trim(string &line)
{
    string spaces = " \t\n\r";
    line.erase(0, line.find_first_not_of(spaces));
    
    size_t pos = line.find(" //");
    if(pos != string::npos)
    {
        line.erase(pos);
    }
    
    line.erase(line.find_last_not_of(spaces) + 1);
}


//==========================================================================================================
// Replace variable specified by a name inside brackets: [var_name] with their value returned from the
// containing script reader.
// Note: the replacement may itself contain reference to other values. So the process is repeated for the
// replacemed string until it contain no variables.
//==========================================================================================================
void Command::replace_vars(string &line, ScriptReader &reader, int call_number)
{
    while(true)
    {
        string line2 = line;
        sregex_iterator iter(line.begin(), line.end(), ScriptReader::script_var_regex);
        sregex_iterator end;

        if(iter == end)
        {
            break;
        }
        
        long offset = 0;
        bool only_len = true;
        
        // Replace all occurrences of [var] in the string
        for(;iter != end; ++iter) {
            string var = (*iter)[1].str();
            
            if(var == "len")
            {
                continue; // [len] is not yet known, it should be replaced after reading entire message and knowing its length
            }
            
            only_len = false;
            string value = reader.get_value(var, call_number);
            line2.replace(iter->position() + offset, iter->length(), value);
            offset += value.length() - iter->length();
        }
        
        line = line2;
        
        if(only_len) // Current line contains only [len] variables, which value isn't known yet
        {
            break;
        }
    }
    
    sregex_iterator iter(line.begin(), line.end(), ScriptReader::literal_var_regex);
    sregex_iterator end;
    
    if(iter == end)
    {
        return;
    }
    
    string line2 = line;
    long offset = 0;
    
    // Replace all occurrences of \[var\] with [var]
    for(;iter != end; ++iter) {
        string var = (*iter)[0].str();
        var.erase(0, 1);
        var.erase(var.length() - 2, 1);
        
        line2.replace(iter->position() + offset, iter->length(), var);
        offset += var.length() - iter->length();
    }
    
    line = line2;
}


//==========================================================================================================
// Replace occurences of [len] with the given string
//==========================================================================================================
void Command::replace_len(string &line, string& len)
{
    string line2 = line;
    regex var_re("\\[len\\]");
    sregex_iterator iter(line.begin(), line.end(), var_re);
    sregex_iterator end;
    long offset = 0;
    
    while(iter != end) {
        line2.replace(iter->position() + offset, iter->length(), len);
        offset += len.length() - iter->length();
        iter++;
    }
    
    line = line2;
}



















