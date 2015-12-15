
#include "BBSimulatedDNSResolver.h"
#include "BBSimulatedConnectivity.h"
#include "BBSimulatedDeviceState.h"
#include "BBSimulatedVideoCapture.h"
#include "BBSimulatedVideoRender.h"

#include "VoXIPDeviceLayer.h"
#include "BBSimulatedDeviceLayer.h"
#include "TestsEnv.h"

BBSimulatedDeviceLayer::BBSimulatedDeviceLayer(void)
{
}

BBSimulatedDeviceLayer::~BBSimulatedDeviceLayer(void)
{
}

BBSimulatedDeviceLayer* BBSimulatedDeviceLayer::Inst()
{
    static BBSimulatedDeviceLayer *the_instance = nullptr;

    if(the_instance == nullptr)
    {
        the_instance = new BBSimulatedDeviceLayer();
    }

    return the_instance;
}

bool BBSimulatedDeviceLayer::Init()
{
    // Only the resolver is not re-created for each test. This is because it is needed before tests begin
    // to run mserver, and it is also ok to reuse it.
    VoXIPDeviceLayer::Inst()->addApiImp(EDNSResolver, TestsEnv::inst()->resolver);
    VoXIPDeviceLayer::Inst()->addApiImp(EConnectivity, voxip_make_shared<BBSimulatedConnectivity>());
    VoXIPDeviceLayer::Inst()->addApiImp(EDeviceState, voxip_make_shared<BBSimulatedDeviceState>());
    VoXIPDeviceLayer::Inst()->addApiImp(EVideoRender, voxip_make_shared<BBSimulatedVideoRender>());
    VoXIPDeviceLayer::Inst()->addApiImp(EVideoCapture, voxip_make_shared<BBSimulatedVideoCapture>());
    return true;
}

