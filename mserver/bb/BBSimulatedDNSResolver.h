
#pragma once

#include "VxDNSResolver.h"


struct BBSimulatedDNSResolver : public VxDNSResolver
{
public:
    BBSimulatedDNSResolver(void);
    ~BBSimulatedDNSResolver(void);
    bool Verify();

    virtual bool Resolve(const std::string &s_name, std::vector<VxIpAddress> &v_iPs);

    VxIpAddress get_cur_ip();               // Return the current sip server ip
    void reset_ip();                        // Return to the first ip in the list
    void advance_ip();                      // Use the next ip in the list from now on
    unsigned long get_num_ips();            // Return the number of ips in the list
    std::vector<VxIpAddress>& get_ips();    // Return the list of ips
    void reduce_ips(int num);               // Leave just <num> ips in the list
    void find_ips();                        // Find the local ips to be used as mock sip server ips
    VxIpAddress get_real_ip();              // Return a non-loop back IP

private:

    std::vector<VxIpAddress> ips;
    int cur_ip_index;
    
    void print_ips();
};

