
#include <arpa/inet.h>
#include "pjlib.h"
#include "BBSimulatedObserver.h"
#include "BBSimulatedConnectivity.h"
#include "bbutils.h"

//======================================================================================================================
//======================================================================================================================
BBSimulatedConnectivity::BBSimulatedConnectivity(void):
    _bNetworkCDMA(false), _bWiFiEnabled(false), _bIsVideoNetworkAllowed(true), _uiCurrentIP()
{
    setNetworkObserver	 ( voxip_make_shared<BBSimulatedObserver>("Network"));
    setNativeCallObserver( voxip_make_shared<BBSimulatedObserver>("NativeCall"));
    setGSMSignalObserver ( voxip_make_shared<BBSimulatedObserver>("GSMSignal"));
    setWiFiSignalObserver( voxip_make_shared<BBSimulatedObserver>("WiFiSignal"));
}


//======================================================================================================================
//======================================================================================================================
BBSimulatedConnectivity::~BBSimulatedConnectivity(void) {}


//======================================================================================================================
//======================================================================================================================
bool BBSimulatedConnectivity::Verify()
{
    return true;
}

//======================================================================================================================
//======================================================================================================================
void BBSimulatedConnectivity::set_carrier_name(const std::string &s_name)
{
    _sCarrierName = s_name;
}

//======================================================================================================================
//======================================================================================================================
void BBSimulatedConnectivity::getCarrierName(std::string &s_name)
{
    s_name = _sCarrierName;
}

//======================================================================================================================
//======================================================================================================================
void BBSimulatedConnectivity::setCurrentIP(const std::string &s_iP)
{
    _uiCurrentIP = VxIpAddress::from_string(s_iP);
}


//======================================================================================================================
//======================================================================================================================
void BBSimulatedConnectivity::setCurrentIP(const VxIpAddress& addr)
{
    _uiCurrentIP = addr;
}


//======================================================================================================================
//======================================================================================================================
VxIpAddress BBSimulatedConnectivity::getCurrentIP()
{
    return _uiCurrentIP;
}

//======================================================================================================================
//======================================================================================================================
string BBSimulatedConnectivity::getCurrentIPString(string preferred_network)
{
    return _uiCurrentIP.to_string();
}

//======================================================================================================================
//======================================================================================================================
string  BBSimulatedConnectivity::get3GNetworkInterfaceName()
{
    return "";
}

//======================================================================================================================
//======================================================================================================================
void BBSimulatedConnectivity::set_network_cDMA(bool b_network_cDMA)
{
    _bNetworkCDMA = b_network_cDMA;
}

//======================================================================================================================
//======================================================================================================================
bool BBSimulatedConnectivity::isNetworkCDMA()
{
    return _bNetworkCDMA;
}

//======================================================================================================================
//======================================================================================================================
bool BBSimulatedConnectivity::IsVideoNetworkAllowed()
{
    return _bIsVideoNetworkAllowed;
}

//======================================================================================================================
//======================================================================================================================
void BBSimulatedConnectivity::set_is_video_network_allowed(bool b_is_video_network_allowed)
{
    _bIsVideoNetworkAllowed = b_is_video_network_allowed;
}

//======================================================================================================================
//======================================================================================================================
int BBSimulatedConnectivity::getNetworkType()
{
    return 0; //UNKNOWN
}

//======================================================================================================================
//======================================================================================================================
string BBSimulatedConnectivity::getNetworkTypeName()
{
    return "UNKNOWN";
}

//======================================================================================================================
//======================================================================================================================
bool BBSimulatedConnectivity::isWiFiEnabled()
{
    return _bWiFiEnabled;
}

//======================================================================================================================
//======================================================================================================================
void BBSimulatedConnectivity::SetWiFiEnableState(bool b_enabled)
{
    _bWiFiEnabled = b_enabled;
}

//======================================================================================================================
//======================================================================================================================
bool BBSimulatedConnectivity::isIPv6Network()
{
    return false;
}