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
    //------------------------------------------------------------------------------------------------------
    // Unfortunately this is needed if you want a connection between an enum value and a string
    //------------------------------------------------------------------------------------------------------
    sip_element_names[NUM]          = "NUM";
    sip_element_names[NAME]         = "NAME";
    sip_element_names[PHONE_NUM]    = "PHONE_NUM";
    sip_element_names[USER]         = "USER";
    sip_element_names[PASSWORD]     = "PASSWORD";
    sip_element_names[URI]          = "URI";
    sip_element_names[HOST]         = "HOST";
    sip_element_names[IP]           = "IP";
    sip_element_names[DOMAIN]       = "DOMAIN";
    sip_element_names[PORT]         = "PORT";
    
    //------------------------------------------------------------------------------------------------------
    // Init sip_element_re
    //------------------------------------------------------------------------------------------------------
    string s;
    
    for(auto pair: sip_element_names)
    {
        s += pair.second + "|";
    }
    
    s.resize(s.length() - 1); // Remove last '|'
    //cout << s << endl;
    sip_element_re = s;
    
    //------------------------------------------------------------------------------------------------------
    // Create regular expressions for the SIP elements. Some contain reference to other regular-expressions,
    // which will eventually be replaced with literal strings.
    // A reference is just the name of the sip element (TODO: check if these names are never used literally
    // in SIP).
    // If this isn't done right reference loops might occur, but I don't check for them.
    //------------------------------------------------------------------------------------------------------
    matchers[NUM]       = new SipMatcher("\\d+", true); // Any sequence of digits
    matchers[NAME]      = new SipMatcher("\\w+", true); // Any sequence of alpha-numeric chars
    matchers[PHONE_NUM] = new SipMatcher("\\+?\\d[\\d-]*\\d", true);
    matchers[USER]      = new SipMatcher("NAME|PHONE_NUM");
    matchers[PASSWORD]  = new SipMatcher("\\S+", true); // Any sequence of non white spaces
    matchers[URI]       = new SipMatcher("sip:((USER)(:(PASSWORD))?@)?(HOST)(:(PORT))?");
    matchers[HOST]      = new SipMatcher("IP|DOMAIN");
    matchers[IP]        = new SipMatcher("(\\d{1,3}\\.){3}\\d{1,3}", true); // TODO: support IP6?
    matchers[DOMAIN]    = new SipMatcher("NAME\\.NAME");
    matchers[PORT]      = new SipMatcher("NUM");
    
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
        cout << "Element name: " << sip_element_names[pair.first] << endl;
        pair.second->print();
        cout << endl;
    }
}


/**********************************************************************************************************
 *
 *                                  CLASS SipParser::SipMatcher
 *
 **********************************************************************************************************/

//==========================================================================================================
//==========================================================================================================
SipParser::SipMatcher::SipMatcher(string _highlevel_re_str, bool _final):
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
    return false;
}


//==========================================================================================================
//==========================================================================================================
string SipParser::SipMatcher::get_match()
{
    return "";
}


//==========================================================================================================
//==========================================================================================================
string SipParser::SipMatcher::get_sub_match(SipElement elem, int pos)
{
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
            
            for(auto p: element_subs)
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
}


//==========================================================================================================
//==========================================================================================================
void SipParser::SipMatcher::print()
{
    if(highlevel_re_str != re_str)
    {
        cout << "highlevel: \"" << highlevel_re_str << "\"" << ", ";
    }

    cout << "regex: \"" << re_str << "\"" << endl;
    
    if(!subs.empty())
    {
        cout << "sub-matches:" << endl;
    }
    
    for(auto p: subs)
    {
        cout << SipParser::inst.get_sip_elem_name(p.first) << ":" << p.second << endl;
    }
}



























