//
//  WebrtcWrapper.cpp
//  mserver
//
//  Created by Yuval Dinari on 12/21/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "WebrtcWrapper.hpp"


//======================================================================================================================
//======================================================================================================================
WebrtcWrapper::WebrtcWrapper()
{
    voice_engine                            = webrtc::VoiceEngine::Create();
    voice_engine_base_interface             = webrtc::VoEBase::GetInterface(voice_engine);
    voice_engine_codec_interface            = webrtc::VoECodec::GetInterface(voice_engine);
    voice_engine_volume_interface           = webrtc::VoEVolumeControl::GetInterface(voice_engine);
    voice_engine_audio_processing_interface = webrtc::VoEAudioProcessing::GetInterface(voice_engine);
    voice_engine_dtmf_interface             = webrtc::VoEDtmf::GetInterface(voice_engine);
    voice_engine_rtprtcp_interface          = webrtc::VoERTP_RTCP::GetInterface(voice_engine);
    voice_engine_video_sync_interface       = webrtc::VoEVideoSync::GetInterface(voice_engine);
    voice_engine_rtprtcp_interface_hardware = webrtc::VoEHardware::GetInterface(voice_engine);
    voice_engine_rtprtcp_interface_network  = webrtc::VoENetwork::GetInterface(voice_engine);
    voice_engine_file_interface             = webrtc::VoEFile::GetInterface(voice_engine);
    voice_engine_net_eq_stats_interface     = webrtc::VoENetEqStats::GetInterface(voice_engine);
}


//======================================================================================================================
//======================================================================================================================
WebrtcWrapper::~WebrtcWrapper()
{
    voice_engine_base_interface->Release();
    voice_engine_codec_interface->Release();
    voice_engine_volume_interface->Release();
    voice_engine_video_sync_interface->Release();
    voice_engine_audio_processing_interface->Release();
    voice_engine_dtmf_interface->Release();
    voice_engine_rtprtcp_interface->Release();
    voice_engine_rtprtcp_interface_hardware->Release();
    voice_engine_rtprtcp_interface_network->Release();
    voice_engine_file_interface->Release();
    voice_engine_net_eq_stats_interface->Release();
    
    webrtc::VoiceEngine::Delete(voice_engine);
}
