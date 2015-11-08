//
//  OptionParser.hpp
//  mserver
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
        Option(bool _mandatory, bool _need_arg, string default_val = "");
        bool missing();
        
        string val; // If val initiated to non-empty, this is considered its default value and the option is allowed w/o an arg
        bool mandatory;
        bool need_arg;
        bool found;
    };
    
    OptionParser(string& line, map<string, Option>& _options);
    OptionParser(int argc, char * argv[], map<string, Option>& _options);
    
private:
    map<string, Option>& options;
    
    Option& check_option(string& opt_name);
    bool set_option_value(Option& opt, string& opt_name, string& opt_val);
    void check_missing_options(map<string, Option>& options);
};

typedef OptionParser::Option Option;

#endif /* OptionParser_hpp */
