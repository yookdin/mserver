
#pragma once

#include <string>
#include "VxConnectivity.h"

class BBSimulatedConnectivity : public VxConnectivity
{
public:
    BBSimulatedConnectivity(void);
    ~BBSimulatedConnectivity(void);
    bool Verify();

    void	set_carrier_name(const std::string &s_name);
    virtual void	getCarrierName(std::string &sName);
    bool	IsVideoNetworkAllowed();
    void	set_is_video_network_allowed(bool b_is_video_network_allowed);
    virtual bool	isNetworkCDMA();
    void	set_network_cDMA(bool b_network_cDMA);
    bool	isWiFiEnabled();
    void	SetWiFiEnableState(bool b_enabled);
    
    virtual VxIpAddress getCurrentIP();
    virtual void	setCurrentIP(const std::string &sIP);
    virtual std::string getCurrentIPString(string preferred_network = CONNECT_TYPE_UNKNOWN);
    void setCurrentIP(const VxIpAddress& addr);
    
    virtual string  get3GNetworkInterfaceName();
    virtual int     getNetworkType();
    virtual string  getNetworkTypeName();
    virtual bool    isIPv6Network();

private:
    std::string		_sCarrierName;
    VxIpAddress     _uiCurrentIP;
    bool			_bNetworkCDMA;
    bool			_bWiFiEnabled;
    bool			_bIsVideoNetworkAllowed;
};

