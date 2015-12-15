///Users/rsalfati/Dev/Sources/git_vo_xIP/i_phone_voxip_framework/i_phone_vo_xIPFramework.xcodeproj
//  BBSimulatedVideoCapture.h
//
//  Created by Gil Osher on 12/09/12.
//
//

#ifndef BBSimulatedVideoCapture_h
#define BBSimulatedVideoCapture_h

#include "VxVideoCapture.h"

class BBSimulatedVideoCapture : public VxVideoCapture
{
public:
    BBSimulatedVideoCapture();
    virtual ~BBSimulatedVideoCapture();

    int GetMaxCaptureWidth() const;
    int GetMaxCaptureHeight() const;
    int GetMaxSendFPS() const;
    int GetMaxReceiveFPS() const;
    void CropFrameToSize(int width, int height);
    bool IsDeviceSupportsCapturingFullscreenVideo();
    virtual bool IsCameraAvailable();

protected:
    
    virtual int AllocateCameraInternal(bool isBack, int width, int height, int fps, int &cameraId);
    virtual int DeallocateCameraInternal(int cameraId);
    virtual int StartCameraInternal(bool isBack, int width, int height, int fps, int &cameraId, VxSurfaceHolderPtr surface);
    virtual int StopCameraInternal(int cameraId);
    virtual int getStopCameraDelayInternal();  // the time delay in seconds to wait before stopCapture
    virtual int shouldDeallocateCameraOnStopCameraInternal();  // should deallocate camera after stopCapture
};

#endif /* defined(BBSimulatedVideoCapture_h) */
