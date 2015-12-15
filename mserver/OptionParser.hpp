//
//  OptionParser.hpp
//  mserver
//
//  Created by Yuval Dinari on 11/8/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef OptionParser_hpp
#define OptionParser_hpp

#include "common.h"

#define DEFAULT_PV_NAME "*"

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
        bool is_param_val_opt()      { return param_val_opt;       }
        
        string name;

    protected:
        bool param_val_opt = false;
        
    private:
        void set_found();
        
        // If initiated to non-empty, this is considered its default value and the option is allowed w/o an arg
        vector<string> values;
        bool mandatory;
        bool need_arg;
        bool found;
        bool multi;
    };
    
    //======================================================================================================
    //======================================================================================================
    class ParamValOption: public Option
    {
    public:
        ParamValOption();
    };
    
    
    OptionParser(string& line, map<string, Option>& _options, string end_str = "");
    OptionParser(int argc, char * argv[], map<string, Option>& _options);
    
private:
    static const regex eq_pair_regex;
    static const regex naked_eq_pair_regex;

    map<string, Option>& options;
    
    bool set_value(string& opt_name, string& val, bool from_cmd_line);
    void check_missing_options(map<string, Option>& options);
    void set_option_names(map<string, Option>&);
    Option* get_opt(string& opt_name);
    void parse_eq_pair(string& pair, string& name, string& val, bool from_cmd_line);
    void remove_param_val_opts();
};

typedef OptionParser::Option Option;
typedef OptionParser::ParamValOption ParamValOption;

#endif /* OptionParser_hpp */
