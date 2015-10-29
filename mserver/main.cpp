
#include "common_headers.h"
#include "MServer.hpp"
#include "SipParser.hpp"

int main(int argc, char * argv[])
{
    //MServer mserver(argc, argv);
    
    SipParser::inst.print();
    
	return 0;
}

