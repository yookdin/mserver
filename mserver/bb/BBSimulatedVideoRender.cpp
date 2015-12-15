//
//  BBSimulatedVideoRender.mm
//
//  Created by Gil Osher on 05/08/12.
//
//

#include <string>
#include "stdio.h"

#include "VxLog.h"
#include "VxMisc.h"

#include "WebRTCWrap.h"
#include "BBSimulatedVideoRender.h"

#undef __CLASS__
#define __CLASS__ "BBFileRenderer"

using namespace std;

class BBFileRenderer: public webrtc::ExternalRenderer
{
public:
    BBFileRenderer(const string &file_name): initialized(false), file_name(file_name), _fpFile(nullptr)
    {
        LOG_SCOPE(file_name);
    }

    ~BBFileRenderer()
    {
        LOG_SCOPE();

        if (initialized && _fpFile != nullptr)
        {
            fclose(_fpFile);
        }
    }

    void InitFile()
    {
        LOG_SCOPE();
        char		buf[16];
        string		s_full_file_name = file_name;
        int			h, min, sec, milli;
        LOG_DEBUG("BBFileRenderer\n");
        VxGetCurrentTime(h, min, sec, milli);
        sprintf(buf, ".%02d%02d%02d.out", h, min, sec);
        s_full_file_name += string(buf);
        _fpFile = fopen(s_full_file_name.c_str(), "wb+");

        if ( _fpFile == nullptr )
        {
            LOG_DEBUG("Failed to open file [%s]\n", s_full_file_name.c_str());
        }
        else
        {
            initialized = true;
        }
    }

    // This method will be called when the stream to be rendered changes in
    // resolution or number of streams mixed in the image.
    int FrameSizeChange(unsigned int width,
                        unsigned int height,
                        unsigned int number_of_streams)
    {
        LOG_SCOPE(width, height, number_of_streams);
        return 0;
    }

    // This method is called when a new frame should be rendered.
    int DeliverFrame(unsigned char* buffer,
                     size_t buffer_size,
                     // RTP timestamp in 90kHz.
                     uint32_t timestamp,
                     // NTP time of the capture time in local timebase
                     // in milliseconds.
                     int64_t ntp_time_ms,
                     // Wallclock render time in milliseconds.
                     int64_t render_time_ms,
                     // Handle of the underlying video frame.
                     void* handle)
    {
        LOG_SCOPE(buffer_size, timestamp);

        if (!initialized)
        {
            InitFile();
        }

        long rc;
        rc = fwrite(buffer, buffer_size, 1, _fpFile);
        LOG_DEBUG("write returned: [%d]\n", rc);
        return 0;
    }

    bool IsTextureSupported()
    {
        return false;
    }

private:
    FILE     *_fpFile;
    string file_name;
    bool initialized;

};

#undef __CLASS__
#define __CLASS__ "BBSimulatedVideoRender"


BBSimulatedVideoRender::BBSimulatedVideoRender() : VxVideoRender()
{
    _pLocalFileRenderer = voxip_make_shared<BBFileRenderer>("LocalView");
    _pRemoteFileRenderer = voxip_make_shared<BBFileRenderer>("RemoteView");
}

void BBSimulatedVideoRender::Reset()
{
    LOG_DEBUG("Reset()");
    VxVideoRender::Reset();
}

BBSimulatedVideoRender::~BBSimulatedVideoRender()
{
    LOG_DEBUG("\n");
}

bool BBSimulatedVideoRender::StopCameraRendering()
{
    return true;
}

bool BBSimulatedVideoRender::SetSurface(VxSurfaceHolderPtr surface)
{
    if (GET_SURFACE(mSurfaceHolder) == GET_SURFACE(surface))
    {
        LOG_DEBUG("surface = 0x%x. the surface didn't change, returning", GET_SURFACE(surface));
        return false;
    }

    VxVideoRender::SetSurface(surface);
    return true;
}

bool BBSimulatedVideoRender::AddCameraInternal(int cameral_id)
{
    LOG_DEBUG("- enter (cameral_id=%d)\n", cameral_id);
    WebRTCVideoEngineLibWrapper::instance()->AddLocalRenderer(cameral_id, _pLocalFileRenderer.get(), false, true);
    WebRTCVideoEngineLibWrapper::instance()->StartRenderer(cameral_id);
    return true;
}

bool BBSimulatedVideoRender::ConnectChannelInternal()
{
    LOG_DEBUG("- enter (mChannel=%d)\n", mChannel);

    if (mIsRemoteViewOn)
    {
        WebRTCVideoEngineLibWrapper::instance()->AddRemoteRenderer(mChannel, _pRemoteFileRenderer.get(), true);
        WebRTCVideoEngineLibWrapper::instance()->StartRenderer(mChannel);
    }

    return true;
}

bool BBSimulatedVideoRender::AddRemoteInternal()
{
    LOG_DEBUG("- enter (mChannel=%d)\n", mChannel);

    if (mChannel != -1)
    {
        WebRTCVideoEngineLibWrapper::instance()->AddRemoteRenderer(mChannel, _pRemoteFileRenderer.get(), true);
        WebRTCVideoEngineLibWrapper::instance()->StartRenderer(mChannel);
    }

    return true;
}

bool BBSimulatedVideoRender::RemoveCameraInternal(int camera_id, bool b_show_stop_animation)
{
    LOG_DEBUG("- enter (camera_id=%d)\n", camera_id);
    WebRTCVideoEngineLibWrapper::instance()->StopRenderer(camera_id);
    WebRTCVideoEngineLibWrapper::instance()->RemoveLocalRenderer(camera_id);
    return true;
}

bool BBSimulatedVideoRender::DisconnectChannelInternal()
{
    LOG_DEBUG("- enter (mChannel=%d)\n", mChannel);
    WebRTCVideoEngineLibWrapper::instance()->StopRenderer(mChannel);
    WebRTCVideoEngineLibWrapper::instance()->RemoveRemoteRenderer(mChannel);
    return true;
}

bool BBSimulatedVideoRender::RemoveRemoteInternal(bool b_show_stop_animation)
{
    LOG_DEBUG("- enter (mChannel=%d)\n", mChannel);

    if (mChannel != -1)
    {
        WebRTCVideoEngineLibWrapper::instance()->StopRenderer(mChannel);
        WebRTCVideoEngineLibWrapper::instance()->RemoveRemoteRenderer(mChannel);
    }

    return true;
}

bool BBSimulatedVideoRender::UpdateRemoteInternal(int cur_channel, int new_channel)
{
    WebRTCVideoEngineLibWrapper::instance()->StopRenderer(cur_channel);
    WebRTCVideoEngineLibWrapper::instance()->RemoveRemoteRenderer(cur_channel);
    WebRTCVideoEngineLibWrapper::instance()->AddRemoteRenderer(new_channel, _pRemoteFileRenderer.get(), true);
    WebRTCVideoEngineLibWrapper::instance()->StartRenderer(new_channel);
    return true;
}

int BBSimulatedVideoRender::GetMaxRenderWidth() const
{
    return 240;
}

int BBSimulatedVideoRender::GetMaxRenderHeight() const
{
    return 320;
}

bool BBSimulatedVideoRender::UpdateCameraInternal(int camera_id, bool is_back)
{
    LOG_DEBUG("- enter (camera_id=%d is_back=%d)\n", camera_id, (int)is_back);
    return true;
}

bool BBSimulatedVideoRender::UpdateRemoteOnCameraChangeInternal(bool is_to_back)
{
    LOG_DEBUG("begin (is_to_back = %d)", (int)is_to_back);
    return true;
}

bool BBSimulatedVideoRender::UpdateCameraOnOrientationChangedInternal(int old_orientation, int new_orientation)
{
    LOG_DEBUG("BBSimulatedVideoRender::UpdateCameraOnOrientationChangedInternal - enter\n");
    return true;
}

bool BBSimulatedVideoRender::UpdateRemoteOnOrientationChangedInternal(int old_orientation, int new_orientation, bool is_back_camera)
{
    LOG_DEBUG("BBSimulatedVideoRender::UpdateRemoteOnOrientationChangedInternal - enter\n");
    return true;
}

bool BBSimulatedVideoRender::UpdateRemoteOnCodecChangedInternal(int width, int height)
{
    LOG_DEBUG("BBSimulatedVideoRender::UpdateRemoteOnCodecChangedInternal - enter\n");
    return true;
}

void BBSimulatedVideoRender::UpdateLocalRenderedFrameSizeInternal(int width, int height)
{
    LOG_DEBUG("BBSimulatedVideoRender::UpdateLocalRenderedFrameSizeInternal(width=%d, height=%d) - enter\n", width,  height);
    return;
}
