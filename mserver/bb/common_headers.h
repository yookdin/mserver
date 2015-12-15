//
//  common_headers.h
//  VoxipBBTests
//
//  Created by Yuval Dinari on 5/21/15.
//  Copyright (c) 2015 Yuval Dinari. All rights reserved.
//

#ifndef VoxipBBTests_common_h
#define VoxipBBTests_common_h


#include <fstream>
#include <future>

#include "PJLIBResources.h"

#include "VOIPManager.h"
#include "VxConnectivity.h"
#include "VxThread.h"
#include "BBSimulatedConnectivity.h"
#include "BBSimulatedDeviceLayer.h"
#include "BBSimulatedDNSResolver.h"
#include "BBSimulatedObserver.h"
#include "BBSimulatedDeviceState.h"
#include "VxMisc.h"
#include "VOIPSettings.h"
#include "VxRegStateMachine.h"
#include "VoXIPQueue.h"
#include "VoXIPMsgIds.h"
#include "VoXIPUIEvents.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "VxCallsMgrMock.h"
#include "VOIPCallBackMock.h"
#include "VoXIPQueueMock.h"
#include "SIPLayerMock.h"

#include "bbutils.h"
#include "TestsEnv.h"

#endif
