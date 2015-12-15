//
//  BBSimulatedVideoRender.h
//
//  Created by Gil Osher on 05/08/12.
//
//

#include "VxVideoRender.h"

#ifndef _BBSimulatedVideoRender_h
#define _BBSimulatedVideoRender_h

class BBFileRenderer;

class BBSimulatedVideoRender : public VxVideoRender
{
public:
    BBSimulatedVideoRender();
    virtual ~BBSimulatedVideoRender();
    virtual void Reset();
    virtual bool SetSurface(VxSurfaceHolderPtr surface);
    bool StopCameraRendering();
    int GetMaxRenderWidth() const;

    int GetMaxRenderHeight() const;
protected:
    virtual bool AddCameraInternal(int camera_id);
    virtual bool AddRemoteInternal();
    virtual bool ConnectChannelInternal();


    virtual bool RemoveCameraInternal(int camera_id, bool b_show_stop_animation);
    virtual bool RemoveRemoteInternal(bool b_show_stop_animation);
    virtual bool DisconnectChannelInternal();

    virtual bool UpdateCameraInternal(int camera_id, bool is_back);
    virtual bool UpdateRemoteInternal(int cur_channel, int new_channel);

    virtual bool UpdateCameraOnOrientationChangedInternal(int old_orientation, int new_orientation);
    virtual bool UpdateRemoteOnCameraChangeInternal(bool is_to_back);
    virtual bool UpdateRemoteOnOrientationChangedInternal(int old_orientation, int new_orientation, bool is_back_camera);
    virtual bool UpdateRemoteOnCodecChangedInternal(int width, int height);
    virtual void UpdateLocalRenderedFrameSizeInternal(int width, int height);

private:
    voxip_shared_ptr<BBFileRenderer> _pLocalFileRenderer;
    voxip_shared_ptr<BBFileRenderer> _pRemoteFileRenderer;
};

#endif
