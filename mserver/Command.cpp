

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



















