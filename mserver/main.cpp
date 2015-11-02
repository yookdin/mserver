
#include "common_headers.h"
#include "MServer.hpp"
#include "SipParser.hpp"

int main(int argc, char * argv[])
{
    MServer::inst.run(argc, argv);
    
	return 0;
}

