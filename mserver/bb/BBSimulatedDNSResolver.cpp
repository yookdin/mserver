
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/errno.h>
#include <ifaddrs.h>

#include "VxLog.h"
#include "VxMisc.h"
#include "BBSimulatedDNSResolver.h"
#include "VOIPSettings.h"
#include "bbutils.h"

//==================================================================================================
// Populate local ips list on construction
//==================================================================================================
BBSimulatedDNSResolver::BBSimulatedDNSResolver()
{
    find_ips();
    
    if(ips.size() == 0)
    {
        LOG_ERROR("Something is wrong, no IPs found");
        exit_nicely(PERIPHERAL_ERR);
    }
    
    if(!ips[0].is_loopback())
    {
        LOG_ERROR("Something is wrong, first IP should be %s", LOOPBACK_IP);
        exit_nicely(PERIPHERAL_ERR);
    }
}

BBSimulatedDNSResolver::~BBSimulatedDNSResolver(void) {}

bool BBSimulatedDNSResolver::Verify()
{
    return true;
}

//==================================================================================================
// For BB tests, ignore input <s_name> and just return the list of local ips already found.
//==================================================================================================
bool BBSimulatedDNSResolver::Resolve(const std::string &s_name, std::vector<VxIpAddress> &v_ips)
{
    v_ips = ips; // Content is copied
    return true;
}


//==================================================================================================
// Return to first ip in the list
//==================================================================================================
void BBSimulatedDNSResolver::reset_ip()
{
    cur_ip_index = 0;
}


//==================================================================================================
// Move to the next ip in the list
//==================================================================================================
void BBSimulatedDNSResolver::advance_ip()
{
    cur_ip_index = (cur_ip_index + 1) % ips.size();
}

//==================================================================================================
// Return the current ip
//==================================================================================================
VxIpAddress BBSimulatedDNSResolver::get_cur_ip()
{
    return ips[cur_ip_index];
}


//==================================================================================================
//==================================================================================================
VxIpAddress BBSimulatedDNSResolver::get_real_ip()
{
    if(ips.size() < 2 || ips[1].is_loopback())
    {
        LOG_ERROR("Something is wrong, no actuall ip in the list");
        exit_nicely(PERIPHERAL_ERR);
    }
    
    return ips[1];
}


//==================================================================================================
// Return the list of IPs
//==================================================================================================
std::vector<VxIpAddress>& BBSimulatedDNSResolver::get_ips()
{
    return ips;
}


//==================================================================================================
//==================================================================================================
unsigned long BBSimulatedDNSResolver::get_num_ips()
{
    return ips.size();
}


//==================================================================================================
// Leave just <num> ips in the list
//==================================================================================================
void BBSimulatedDNSResolver::reduce_ips(int num)
{
    if(num < ips.size())
    {
        ips.resize(num);
    }
}


//==================================================================================================
// Find local ips (pjsip does something similar in if_enum_by_af())
//==================================================================================================
void BBSimulatedDNSResolver::find_ips()
{
    ips.clear();
    cur_ip_index = 0;
    struct ifaddrs *ifap = nullptr, *it;

    if (getifaddrs(&ifap) != 0)
    {
        LOG_ERROR("getifaddrs() failed");
        return;
    }

    for (it = ifap; it != nullptr; it = it->ifa_next)
    {
        struct sockaddr *ad = it->ifa_addr;

        if(ad == nullptr || ad->sa_family != AF_INET)
        {
            continue;
        }

        ips.push_back(VxIpAddress::from_sockaddr(ad));
    }

    freeifaddrs(ifap);
    print_ips();
}


//==================================================================================================
//==================================================================================================
void BBSimulatedDNSResolver::print_ips()
{
    std::string msg;

    for(auto ip : ips)
    {
        msg += " " + ip.to_string();
    }
    
    LOG_DEBUG("IPs list: %s", msg.c_str());
}





