
#include "VxLog.h"
#include "BBSimulatedObserver.h"
#include "BBSimulatedDeviceState.h"

#include <json/json.h>
#include <boost/lexical_cast.hpp>

BBSimulatedDeviceState::BBSimulatedDeviceState()
{
    setBatteryObserver		(voxip_make_shared<BBSimulatedObserver>("Battery"));
    setRingVolumeObserver	(voxip_make_shared<BBSimulatedObserver>("RingVolume"));
    setHeadsetObserver		(voxip_make_shared<BBSimulatedObserver>("Headset"));
    setAudioSessionObserver	(voxip_make_shared<BBSimulatedObserver>("AudioSession"));
    setApplicationObserver	(voxip_make_shared<BBSimulatedObserver>("Application"));
    simulated_tick_count = -1;
}

BBSimulatedDeviceState::~BBSimulatedDeviceState(void)
{
}

bool BBSimulatedDeviceState::Verify()
{
    return true;
}

void BBSimulatedDeviceState::HandleAudioInterruption(int is_begin)
{
    Json::FastWriter json_writer;
    Json::Value json_root;
    json_root["is_begin"] = boost::lexical_cast<std::string>(is_begin) ;
}

int64_t BBSimulatedDeviceState::get_tick_count()
{
    if (simulated_tick_count > -1)
    {
        return simulated_tick_count;
    }

    return get_tick_count();
}

void  BBSimulatedDeviceState::set_tick_count(int64_t tick_count)
{
    simulated_tick_count = tick_count;
}

