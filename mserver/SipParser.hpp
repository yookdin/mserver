//
//  SipParser.hpp
//  mserver
//
//  Created by Yuval Dinari on 10/28/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef SipParser_hpp
#define SipParser_hpp

#include "common_headers.h"

//==========================================================================================================
// Syntactic element that appear in SIP messages
//==========================================================================================================
enum SipElement
{
    NUM,
    NAME,
    PHONE_NUM,
    PASSWORD,
    USER,
    URI,
    HOST,
    IP,
    DOMAIN,
    PORT,
    METHOD,
    REQUEST_LINE,
    STATUS_LINE,
    SIP_VERSION,
    PARAM,
    OPT_PARAM_LIST,
    STATUS_CODE,
    HEADER_NAME,
    HEADER_VALUE,
    HEADER_LINE,
    USER_NAME,
    START_LINE,
    CSEQ_VALUE
};

//==========================================================================================================
// SIP parser, knows to match strings against a SIP syntactic element, and also return the sub-matches by
// their name (i.e. syntactic element kind)
//==========================================================================================================
class SipParser
{
public:
    static SipParser& inst();
    const string method_str = "ACK|BYE|CANCEL|INFO|INVITE|MESSAGE|NOTIFY|OPTIONS|PRACK|PUBLISH|REFER|REGISTER|SUBSCRIBE|UPDATE";
    const string header_name_str = "[[:alnum:]]+(-[[:alnum:]]+)*";
    
    bool match(SipElement elem, string line);
    string get_match(SipElement elem, string line);
    string get_match();
    string get_sub_match(SipElement elem, int pos = 0);
    void print();
    void print_match(SipElement elem, string line);

private:
    static SipParser* p_inst;
    SipParser();
    
    //======================================================================================================
    // Knows how to match a single element, by using its regex, and also return named submatches.
    //======================================================================================================
    class SipMatcher
    {
    public:
        const SipElement elem;
        const string highlevel_re_str;
        
        SipMatcher(SipElement elem, string highlevel_re, bool _final = false);
        bool match(string line);
        string get_match(string line);
        string get_match();
        string get_sub_match(SipElement sub_elem, int pos = 0);
        void finalize(SipParser*);
        void print();
        
        string get_highlevel_re() { return highlevel_re_str; }
        string get_re() { return re_str; }
        long get_re_length() { return re_str.length(); }
        bool is_final() { return final; }
        void self_check(SipParser*);
        void print_match();
        
    private:
        string cur_line;
        bool final;
        string re_str;
        regex re;
        smatch match_result;
        vector<pair<SipElement, long>> subs;

        // Return number of submatches in re_str from the begining to positon (inclusive)
        long count_num_subs(long pos) {
            return count_if(re_str.begin(), re_str.begin() + pos + 1,
                            [](char c){ return c == '('; });
        }
    };
    
    bimap<SipElement, string> sip_element_names; // Used to get from element to its name and vice versa
    regex sip_element_re;
    map<SipElement, SipMatcher*> matchers;
    SipMatcher *cur_matcher = nullptr;

    SipElement get_sip_elem(string name) { return sip_element_names[name]; }
    string get_sip_elem_name(SipElement elem) { return sip_element_names[elem]; }
    SipMatcher* get_matcher(string);
    void finalize_matchers();
    
};

#endif /* SipParser_hpp */
