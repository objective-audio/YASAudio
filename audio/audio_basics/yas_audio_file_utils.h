//
//  yas_audio_file_utils.h
//

#pragma once

#include <AVFoundation/AVFoundation.h>
#include <AudioToolbox/AudioToolbox.h>
#include <CoreFoundation/CoreFoundation.h>

namespace yas {
namespace audio {
    namespace file_type {
        extern CFStringRef const three_gpp;
        extern CFStringRef const three_gpp2;
        extern CFStringRef const aifc;
        extern CFStringRef const aiff;
        extern CFStringRef const amr;
        extern CFStringRef const ac3;
        extern CFStringRef const mpeg_layer3;
        extern CFStringRef const core_audio_format;
        extern CFStringRef const mpeg4;
        extern CFStringRef const apple_m4a;
        extern CFStringRef const wave;
    }

    AudioFileTypeID to_audio_file_type_id(CFStringRef const fileType);
    CFStringRef to_file_type(AudioFileTypeID const fileTypeID);

    namespace ext_audio_file_utils {
        Boolean can_open(CFURLRef const url);
        Boolean open(ExtAudioFileRef *ext_audio_file, CFURLRef const url);
        Boolean create(ExtAudioFileRef *extAudioFile, CFURLRef const url, AudioFileTypeID const file_type_id,
                       AudioStreamBasicDescription const &asbd);
        Boolean dispose(ExtAudioFileRef const ext_audio_file);
        Boolean set_client_format(AudioStreamBasicDescription const &asbd, ExtAudioFileRef const ext_audio_file);
        Boolean get_audio_file_format(AudioStreamBasicDescription *asbd, ExtAudioFileRef const ext_audio_file);
        AudioFileID get_audio_file_id(ExtAudioFileRef const ext_audio_file);
        SInt64 get_file_length_frames(ExtAudioFileRef const ext_audio_file);
        AudioFileTypeID get_audio_file_type_id(ExtAudioFileRef const ext_audio_file);
        CFStringRef get_audio_file_type(ExtAudioFileRef const ext_auidio_file);
    }

    CFDictionaryRef wave_file_settings(Float64 const sample_rate, UInt32 const channel_count, UInt32 const bit_depth);
    CFDictionaryRef aiff_file_settings(Float64 const sample_rate, UInt32 const channel_count, UInt32 const bit_depth);
    CFDictionaryRef linear_pcm_file_settings(Float64 const sample_rate, UInt32 const channel_count,
                                             UInt32 const bit_depth, const bool is_big_endian, const bool is_float,
                                             const bool is_non_interleaved);
    CFDictionaryRef aac_settings(Float64 const sample_rate, UInt32 const channel_count, UInt32 const bit_depth,
                                 const AVAudioQuality encoder_quality, UInt32 const bit_rate,
                                 UInt32 const bit_depth_hint, const AVAudioQuality converter_quality);
}
}