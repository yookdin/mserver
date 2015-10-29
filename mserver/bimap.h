//
//  bimap.h
//  mserver
//
//  Created by Yuval Dinari on 10/28/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef bimap_h
#define bimap_h

#include <map>

using namespace std;

//==========================================================================================================
// Like a regular map, but can also call m[val] and get the key of this value. If value doesn't exist an
// exception is thrown.
//==========================================================================================================
template <class K, class V>
class bimap: public map<K, V> {
public:
    
    V& operator[] (const K& key) {
        return map<K, V>::operator[](key);
    }
    
    K operator[] (const V value) {
        // Linear search
        for(auto pair: *this) {
            if(pair.second == value)
            {
                return pair.first;
            }
        }
        
        throw string("bimap doesn't contain value");
    }
};


#endif /* bimap_h */
