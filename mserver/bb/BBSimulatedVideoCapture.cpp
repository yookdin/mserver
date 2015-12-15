//
//  BBSimulatedVideoCapture.cpp
//
//  Created by Gil Osher on 12/09/12.
//
//

#include <string.h>

#include "VxLog.h"
#include "BBSimulatedVideoCapture.h"

BBSimulatedVideoCapture::BBSimulatedVideoCapture()
{
}

BBSimulatedVideoCapture::~BBSimulatedVideoCapture()
{
}

int BBSimulatedVideoCapture::AllocateCameraInternal(bool is_back, int width, int height, int fps, int &camera_id)
{
    VOXIP_DLOG("BBSimulatedVideoCapture::AllocateCameraInternal(is_back=%d, width=%d, height=%d, fps=%d, &camera_id=%d)\n", (int)is_back, width, height, fps, camera_id);
    return 0;
}

int BBSimulatedVideoCapture::StartCameraInternal(bool is_back, int width, int height, int fps, int &camera_id, VxSurfaceHolderPtr surface)
{
    VOXIP_DLOG("BBSimulatedVideoCapture::StartCameraInternal(is_back=%d, width=%d, height=%d, fps=%d, &camera_id=%d)\n", (int)is_back, width, height, fps, camera_id);
    camera_id = 1000;
    return 0;
}

int BBSimulatedVideoCapture::DeallocateCameraInternal(int camera_id)
{
    VOXIP_DLOG("BBSimulatedVideoCapture::DeallocateCameraInternal(camera_id=%d)\n", camera_id);
    return 0;
}

int BBSimulatedVideoCapture::StopCameraInternal(int camera_id)
{
    VOXIP_DLOG("BBSimulatedVideoCapture::StopCameraInternal(camera_id=%d)\n", camera_id);
    return 0;
}

int BBSimulatedVideoCapture::getStopCameraDelayInternal()
{
    return 30;
}

int BBSimulatedVideoCapture::shouldDeallocateCameraOnStopCameraInternal()
{
    return 0;
}

void BBSimulatedVideoCapture::CropFrameToSize(int width, int height)
{
    VOXIP_DLOG("BBSimulatedVideoCapture::crop_frame_to_size() width [%d], height [%d]", width, height);
}

bool BBSimulatedVideoCapture::IsDeviceSupportsCapturingFullscreenVideo()
{
    return true;
}

bool BBSimulatedVideoCapture::IsCameraAvailable()
{
    return true;
}

int BBSimulatedVideoCapture::GetMaxCaptureWidth() const
{
    return 240;
}

int BBSimulatedVideoCapture::GetMaxCaptureHeight() const
{
    return 320;
}

int BBSimulatedVideoCapture::GetMaxSendFPS() const
{
    return 15;
}

int BBSimulatedVideoCapture::GetMaxReceiveFPS() const
{
    return 15;
}

