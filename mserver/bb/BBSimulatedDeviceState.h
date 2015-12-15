
#pragma once

#include "VxDeviceState.h"

class BBSimulatedDeviceState : public VxDeviceState
{
private:
    int64_t simulated_tick_count;
public:
    BBSimulatedDeviceState();
    virtual ~BBSimulatedDeviceState(void);
    bool	Verify();

    virtual void HandleAudioInterruption(int is_begin);

    int64_t get_tick_count();
    void set_tick_count(int64_t tick_count);
};
