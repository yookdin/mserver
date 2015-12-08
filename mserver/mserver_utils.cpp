//
//  mserver_utils.cpp
//  mserver
//
//  Created by Yuval Dinari on 12/8/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "mserver_utils.hpp"

// Yes, all these backslashes are needed, because after the normal replacement, the regex does another one it seems
const string string_regex_str = "\"((\\\\\"|[^\"])*)\"";
