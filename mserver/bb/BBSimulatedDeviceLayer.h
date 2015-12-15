
#pragma once

class BBSimulatedDeviceLayer
{
public:
    ~BBSimulatedDeviceLayer(void);

    static BBSimulatedDeviceLayer* Inst();

    bool Init();

private:
    BBSimulatedDeviceLayer(void);
};

