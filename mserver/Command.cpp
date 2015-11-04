

#include "Command.h"
#include "ScriptReader.h"


//==========================================================================================================
// Remove leading white spaces and tabs. Remove new line char at the end.
//==========================================================================================================
void Command::trim(string &line)
{
    string spaces = " \t";
    line.erase(0, line.find_first_not_of(spaces));
    line.erase(line.find_last_not_of(spaces) + 1);
    
    // Don't count on input script to contain correct end of line. Strip it, and later if needed CRLF will be appended
    if(line.back() == '\r' || line.back() == '\n')
    {
        line.pop_back();
    }
}


//==========================================================================================================
// Replace variable specified by a name inside brackets: [var_name] with their value returned from the
// containing script reader.
// Return true if the special var [len] is seen, since it is not yet known and will be replaced later.
//==========================================================================================================
bool Command::replace_vars(string &line)
{
    bool contains_len = false;
    string line2 = line;
    regex var_re("\\[(\\w+)\\]");
    sregex_iterator iter(line.begin(), line.end(), var_re);
    sregex_iterator end;
    long offset = 0;
    
    for(;iter != end; ++iter) {
        //cout << iter->position() << ": " << iter->str() << endl;
        string var = (*iter)[1].str();

        if(var == "len")
        {
            contains_len = true;
            continue; // [len] is not yet known, it should be replaced after reading entire message and knowing its length
        }
        
        string value = reader.get_value(var);
        line2.replace(iter->position() + offset, iter->length(), value);
        offset += value.length() - iter->length();
    }

    line = line2;
    return contains_len;
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



















