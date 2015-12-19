//
//  OptionParser.hpp
//
//  Created by Yuval Dinari on 11/8/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef OptionParser_hpp
#define OptionParser_hpp

#include "common_headers.h"


//==========================================================================================================
//==========================================================================================================
class OptionParser {
public:
    
    //======================================================================================================
    //======================================================================================================
    class Option
    {
    public:
        Option(bool _mandatory, bool _need_arg, bool _multi = false, string default_val = "");
        bool set_value(string& val);
        
        string get_value()           { return (values.empty() ? "" : values[0]); }
        vector<string>& get_values() { return values;              }
        bool was_found()             { return found;               }
        bool missing()               { return mandatory && !found; }
        
        string name;

    private:
        void set_found();
        
        // If initiated to non-empty, this is considered its default value and the option is allowed w/o an arg
        vector<string> values;
        bool mandatory;
        bool need_arg;
        bool found;
        bool multi;
    };
    
    
    OptionParser(int argc, char * argv[], map<string, Option>& _options);
    
private:
    map<string, Option>& options;
    
    bool set_value(string& opt_name, string& val, bool from_cmd_line);
    void check_missing_options(map<string, Option>& options);
    void set_option_names(map<string, Option>&);
    Option* get_opt(string& opt_name);
};

typedef OptionParser::Option Option;


#endif /* OptionParser_hpp */
