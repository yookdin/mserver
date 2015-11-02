

#include "Command.h"
#include "ScriptReader.h"


//==========================================================================================================
// Remove leading white spaces and tabs. Remove new line char at the end.
//==========================================================================================================
void Command::trim(string &line)
{
    line.erase(0, line.find_first_not_of(' '));
    line.erase(0, line.find_first_not_of('\t'));

    if(line.back() == '\r' || line.back() == '\n')
    {
        line.pop_back();
    }
}


//==========================================================================================================
// Variable are specified by a name inside brackets: [var_name].
//==========================================================================================================
void Command::replace_vars(string &line)
{
    string line2 = line;
    regex var_re("\\[(\\w+)\\]");
    sregex_iterator iter(line.begin(), line.end(), var_re);
    sregex_iterator end;
    long offset = 0;
    
    while(iter != end) {
        //cout << iter->position() << ": " << iter->str() << endl;
        string value = reader.get_value((*iter)[1].str());
        line2.replace(iter->position() + offset, iter->length(), value);
        offset += value.length() - iter->length();
        iter++;
    }

    line = line2;
}