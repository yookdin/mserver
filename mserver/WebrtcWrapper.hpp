//
//  WebrtcWrapper.hpp
//  mserver
//
//  Created by Yuval Dinari on 12/21/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#ifndef WebrtcWrapper_hpp
#define WebrtcWrapper_hpp

#include <WebRTC/vonage_frame_scale_utils.h>
#include <WebRTC/common_types.h>
#include <WebRTC/voe_base.h>
#include <WebRTC/voe_codec.h>
#include <WebRTC/voe_volume_control.h>
#include <WebRTC/voe_audio_processing.h>
#include <WebRTC/voe_dtmf.h>
#include <WebRTC/voe_rtp_rtcp.h>
#include <WebRTC/voe_video_sync.h>
#include <WebRTC/voe_hardware.h>
#include <WebRTC/voe_file.h>
#include <WebRTC/voe_network.h>
#include <WebRTC/voe_neteq_stats.h>

#include "common.h"


//======================================================================================================================
//======================================================================================================================
class WebrtcWrapper
{
public:
    static WebrtcWrapper* inst();
    ~WebrtcWrapper();

private:
    WebrtcWrapper();
    static WebrtcWrapper* instance;
    
    webrtc::VoiceEngine*         voice_engine;
    webrtc::VoEBase*             voice_engine_base_interface;
    webrtc::VoECodec*            voice_engine_codec_interface;
    webrtc::VoEVolumeControl*    voice_engine_volume_interface;
    webrtc::VoEAudioProcessing*  voice_engine_audio_processing_interface;
    webrtc::VoEDtmf*             voice_engine_dtmf_interface;
    webrtc::VoERTP_RTCP*         voice_engine_rtprtcp_interface;
    webrtc::VoEVideoSync*        voice_engine_video_sync_interface;
    webrtc::VoEHardware*         voice_engine_rtprtcp_interface_hardware;
    webrtc::VoENetwork*          voice_engine_rtprtcp_interface_network;
    webrtc::VoEFile*             voice_engine_file_interface;
    webrtc::VoENetEqStats*       voice_engine_net_eq_stats_interface;


};


#endif /* WebrtcWrapper_hpp */
