//
//  SipParser.cpp
//  mserver
//
//  Created by Yuval Dinari on 10/28/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "SipParser.hpp"



//==========================================================================================================
// The only instance of SipParser
//==========================================================================================================
SipParser SipParser::inst;

//==========================================================================================================
// Init maps, process regexes
//==========================================================================================================
SipParser::SipParser()
{

#define QUOTE(EXP) #EXP
    
#define ADD_ELEMENT_MATCHER(ELEM, STR, BOOL)\
    sip_element_names[ELEM] = QUOTE(_##ELEM##_);\
    matchers[ELEM] = new SipMatcher(ELEM, STR, BOOL)

    //------------------------------------------------------------------------------------------------------
    // Create regular expressions for the SIP elements. Some contain reference to other regular-expressions,
    // which will eventually be replaced with literal strings.
    // A reference is just the name of the sip element (TODO: check if these names are never used literally
    // in SIP).
    // If this isn't done right reference loops might occur, but I don't check for them.
    //------------------------------------------------------------------------------------------------------
    ADD_ELEMENT_MATCHER(METHOD,             "[[:upper:]]+",                             true);
    ADD_ELEMENT_MATCHER(NUM,                "\\d+",                                     true);
    ADD_ELEMENT_MATCHER(NAME,               "\\w+",                                     true);
    ADD_ELEMENT_MATCHER(USER_NAME,          "[^ :@]+",                                  true);
    ADD_ELEMENT_MATCHER(PASSWORD,           "\\S+",                                     true);
    ADD_ELEMENT_MATCHER(IP,                 "(\\d{1,3}\\.){3}\\d{1,3}",                 true);
    ADD_ELEMENT_MATCHER(SIP_VERSION,        "SIP/2.0",                                  true);
    ADD_ELEMENT_MATCHER(PARAM,              "[^ ;=]+(=[^ ;=]+)?",                       true);
    ADD_ELEMENT_MATCHER(STATUS_CODE,        "\\d{3}",                                   true);
    ADD_ELEMENT_MATCHER(HEADER_NAME,        "[[:alnum:]]+(-[[:alnum:]]+)*",             true);
    ADD_ELEMENT_MATCHER(HEADER_VALUE,       ".*",                                       true); // Allow anything
    
    
    ADD_ELEMENT_MATCHER(PHONE_NUM,          "\\+?_NUM_(-_NUM_)*",                       false);

    // TODO: add headers (optional after the parameter list) ?
    ADD_ELEMENT_MATCHER(URI,                "sip:((_USER_)(:(_PASSWORD_))?@)?(_HOST_)(:(_PORT_))?(_OPT_PARAM_LIST_)", false);
    
    ADD_ELEMENT_MATCHER(PORT,               "_NUM_",                                    false);
    ADD_ELEMENT_MATCHER(USER,               "_USER_NAME_|_PHONE_NUM_",                  false);
    ADD_ELEMENT_MATCHER(HOST,               "_IP_|_DOMAIN_",                            false);
    ADD_ELEMENT_MATCHER(DOMAIN,             "_NAME_\\._NAME_",                          false);
    ADD_ELEMENT_MATCHER(REQUEST_LINE,       "(_METHOD_) (_URI_) (_SIP_VERSION_)",       false);
    
    // Don't care about the reason phrase (everything after the status code)
    ADD_ELEMENT_MATCHER(STATUS_LINE,        "(_SIP_VERSION_) (_STATUS_CODE_) .*",       false);

    ADD_ELEMENT_MATCHER(OPT_PARAM_LIST,     "(;_PARAM_)*",                              false);
    ADD_ELEMENT_MATCHER(HEADER_LINE,        "(_HEADER_NAME_) *: *(_HEADER_VALUE_)" CRLF,     false);
    ADD_ELEMENT_MATCHER(START_LINE,         "(_STATUS_LINE_)|(_REQUEST_LINE_)" CRLF,         false);
    
#undef ADD_ELEMENT_MATCHER
    
    //------------------------------------------------------------------------------------------------------
    // Init sip_element_re
    //------------------------------------------------------------------------------------------------------
    string s;
    
    for(auto pair: sip_element_names)
    {
        s += "\\b" + pair.second + "\\b|";
    }
    
    s.resize(s.length() - 1); // Remove last '|'
    sip_element_re = s;
    finalize_matchers();
}


//==========================================================================================================
// Since the original regular expression contain references, it is needed to resolove them.
//==========================================================================================================
void SipParser::finalize_matchers()
{
    for(auto pair: matchers)
    {
        pair.second->finalize();
    }
}


//==========================================================================================================
// Return whether the given string matches the regex for the given SIP element.
//==========================================================================================================
bool SipParser::match(SipElement elem, string line)
{
    cur_matcher = matchers[elem];
    return cur_matcher->match(line);
}


//==========================================================================================================
//==========================================================================================================
string SipParser::get_match(SipElement elem, string line)
{
    cur_matcher = matchers[elem];
    return cur_matcher->get_match(line);
}


//==========================================================================================================
//==========================================================================================================
string SipParser::get_match()
{
    if(cur_matcher != nullptr)
    {
        return cur_matcher->get_match();
    }

    throw string("No match has been performed");
}


//==========================================================================================================
// After a successful match this will return the sub-match of the given element within the match of the last
// matched element. If called with elements not contained in the last element, an empty string will be
// returned.
//==========================================================================================================
string SipParser::get_sub_match(SipElement elem, int pos)
{
    if(cur_matcher != nullptr)
    {
        return cur_matcher->get_sub_match(elem, pos);
    }
    else
    {
        throw string("No match has been performed");
    }
}


//=====================================x=====================================================================
//==========================================================================================================
void SipParser::print_match(SipElement elem, string line)
{
    match(elem, line);
    cur_matcher->print_match();
}


//==========================================================================================================
// Private function, used for the finalize matcher process.
//==========================================================================================================
SipParser::SipMatcher* SipParser::get_matcher(string elem_name)
{
    SipElement elem = sip_element_names[elem_name];
    
    if(!matchers[elem]->is_final())
    {
        matchers[elem]->finalize();
    }

    return matchers[elem];
}


//==========================================================================================================
//==========================================================================================================
void SipParser::print()
{
    for(auto pair: matchers)
    {
        pair.second->print();
        cout << endl;
    }
}


/***********************************************************************************************************
 *                                                                                                         *
 *                                  CLASS SipParser::SipMatcher                                            *
 *                                                                                                         *
 ***********************************************************************************************************/

//==========================================================================================================
//==========================================================================================================
SipParser::SipMatcher::SipMatcher(SipElement _elem, string _highlevel_re_str, bool _final):
    elem(_elem),
    highlevel_re_str(_highlevel_re_str),
    final(_final)
{
    if(final)
    {
        re_str = highlevel_re_str;
        re = re_str;
    }
}


//==========================================================================================================
//==========================================================================================================
bool SipParser::SipMatcher::match(string line)
{
    cur_line = line;
    return regex_match(cur_line, match_result, re);
}


//==========================================================================================================
//==========================================================================================================
string SipParser::SipMatcher::get_match(string line)
{
    cur_line = line;
    match(cur_line);
    return match_result.str();
}


//==========================================================================================================
//==========================================================================================================
string SipParser::SipMatcher::get_match()
{
    return match_result.str();
}


//==========================================================================================================
//==========================================================================================================
string SipParser::SipMatcher::get_sub_match(SipElement sub_elem, int pos)
{
    int cur_pos = -1;
    
    for(auto pair: subs)
    {
        if(pair.first == sub_elem && ++cur_pos == pos)
        {
            return match_result[pair.second];
        }
    }
    
    return "";
}


//==========================================================================================================
//==========================================================================================================
void SipParser::SipMatcher::finalize()
{
    if(final)
    {
        return;
    }
    
    re_str = highlevel_re_str;
    smatch match;
    string::const_iterator first = re_str.begin();
    string::const_iterator last = re_str.end();
    long first_pos = 0; // The position of the first iterator; needed for getting absolute positons from relative match positions
    
    //-------------------------------------------------------------------------------------------------------
    // Go over all references to other SIP elements in the original string, and replace them with the actual
    // string of the regex of that element.
    // In the process keep track of sub-matches and their positons.
    //-------------------------------------------------------------------------------------------------------
    while(regex_search(first, last, match, SipParser::inst.sip_element_re))
    {
        // NOTE: if matcher for the matched element is not finalized yet, it will first be finalized before
        // returning it!
        SipMatcher *matcher = SipParser::inst.get_matcher(match.str());
        long pos = first_pos + match.position();

        //---------------------------------------------------------------------------------------------------
        // Add to the submatches list a pair of the found element and the number of its sub match, so later
        // we can retrieve it using the SipElement enum.
        // Add also the list of submatches that the matcher for this element contains, but need to adjust
        // their positions by adding an offset of the current submatch position.
        // All this is done only if the current element is surrounded by () in the original high level re.
        //---------------------------------------------------------------------------------------------------
        if((pos > 0 && re_str[pos-1] == '(') &&
           (pos + match.length() < re_str.length() && re_str[pos + match.length()] == ')' ))
        {
            long num_subs = count_num_subs(pos); // Number of submatches up to and including the current one
            SipElement elem = SipParser::inst.get_sip_elem(match.str());
            subs.emplace(subs.end(), elem, num_subs); // Add a new pair to the vector
            
            auto element_subs = matcher->subs;
            
            for(auto &p: element_subs)
            {
                p.second += num_subs;
            }
            
            subs.insert(subs.end(), element_subs.begin(), element_subs.end());
        }
        
        // Replace reference with actual regex string in final_re
        re_str.replace(pos, match.length(), matcher->get_re());
        
        // Adjust first to point to after the replaced portion
        first_pos = pos + matcher->get_re_length();
        first = re_str.begin() + first_pos;

        // Adjust last to the new end, because string length might have changed
        last = re_str.end();
        
    } // while
    
    re = re_str;
    final = true;
    self_check();
}


//==========================================================================================================
//==========================================================================================================
void SipParser::SipMatcher::print()
{
    cout << "Element name: " << SipParser::inst.get_sip_elem_name(elem) << endl;

    string title;

    if(highlevel_re_str != re_str)
    {
        cout << "Highlevel regex: \"" << highlevel_re_str << "\"" << endl;
        title = "Actual regex   :";
    }
    else
    {
        title = "Actual regex:";
    }
    
    cout << title << " \"" << re_str << "\"" << endl;
    
    if(!subs.empty())
    {
        cout << "sub-matches:" << endl;
    }
    
    for(auto p: subs)
    {
        cout << SipParser::inst.get_sip_elem_name(p.first) << ": " << p.second << endl;
    }
}


//==========================================================================================================
//==========================================================================================================
void SipParser::SipMatcher::self_check()
{
    regex possible_elem("_([[:upper:]]+_)+");
    sregex_iterator iter(re_str.begin(), re_str.end(), possible_elem);
    sregex_iterator end;

    vector<string> names;
    
    while(iter != end) {
        names.push_back(iter->str());
        iter++;
    }

    if(!names.empty())
    {
        cout << "Possible unresolved element names in SIP matcher " << SipParser::inst.get_sip_elem_name(elem) << ":" << endl;

        for(auto s: names)
        {
            cout << s << endl;
        }
        
        cout << endl;
    }
}


//==========================================================================================================
//==========================================================================================================
void SipParser::SipMatcher::print_match()
{
    cout << "High-level  : \"" << highlevel_re_str << "\"" << endl;
    cout << "Actual regex: \"" << re_str << "\"" << endl;
    cout << "Entire match: " << get_match() << endl;
    for(auto p: subs)
    {
        cout << "Sub-match " << SipParser::inst.get_sip_elem_name(p.first) << "(" << p.second << "): '" << match_result[p.second] << "'" << endl;
    }
    cout << endl;
}
























