//
//  yas_audio_file_utils.mm
//  Copyright (c) 2015 Yuki Yasoshima.
//

#include "yas_audio_file_utils.h"
#include "yas_exception.h"

using namespace yas;

const CFStringRef audio::file_type::three_gpp = CFSTR("public.3gpp");
const CFStringRef audio::file_type::three_gpp2 = CFSTR("public.3gpp2");
const CFStringRef audio::file_type::aifc = CFSTR("public.aifc-audio");
const CFStringRef audio::file_type::aiff = CFSTR("public.aiff-audio");
const CFStringRef audio::file_type::amr = CFSTR("org.3gpp.adaptive-multi-rate-audio");
const CFStringRef audio::file_type::ac3 = CFSTR("public.ac3-audio");
const CFStringRef audio::file_type::mpeg_layer3 = CFSTR("public.mp3");
const CFStringRef audio::file_type::core_audio_format = CFSTR("com.apple.coreaudio-format");
const CFStringRef audio::file_type::mpeg4 = CFSTR("public.mpeg-4");
const CFStringRef audio::file_type::apple_m4a = CFSTR("com.apple.m4a-audio");
const CFStringRef audio::file_type::wave = CFSTR("com.microsoft.waveform-audio");

AudioFileTypeID yas::audio::to_audio_file_type_id(const CFStringRef fileType) {
    if (CFEqual(fileType, file_type::three_gpp)) {
        return kAudioFile3GPType;
    } else if (CFEqual(fileType, file_type::three_gpp2)) {
        return kAudioFile3GP2Type;
    } else if (CFEqual(fileType, file_type::aifc)) {
        return kAudioFileAIFCType;
    } else if (CFEqual(fileType, file_type::aiff)) {
        return kAudioFileAIFFType;
    } else if (CFEqual(fileType, file_type::amr)) {
        return kAudioFileAMRType;
    } else if (CFEqual(fileType, file_type::ac3)) {
        return kAudioFileAC3Type;
    } else if (CFEqual(fileType, file_type::mpeg_layer3)) {
        return kAudioFileMP3Type;
    } else if (CFEqual(fileType, file_type::core_audio_format)) {
        return kAudioFileCAFType;
    } else if (CFEqual(fileType, file_type::mpeg4)) {
        return kAudioFileMPEG4Type;
    } else if (CFEqual(fileType, file_type::apple_m4a)) {
        return kAudioFileM4AType;
    } else if (CFEqual(fileType, file_type::wave)) {
        return kAudioFileWAVEType;
    }
    return 0;
}

CFStringRef yas::audio::to_file_type(const AudioFileTypeID fileTypeID) {
    switch (fileTypeID) {
        case kAudioFile3GPType:
            return file_type::three_gpp;
        case kAudioFile3GP2Type:
            return file_type::three_gpp2;
        case kAudioFileAIFCType:
            return file_type::aifc;
        case kAudioFileAIFFType:
            return file_type::aiff;
        case kAudioFileAMRType:
            return file_type::amr;
        case kAudioFileAC3Type:
            return file_type::ac3;
        case kAudioFileMP3Type:
            return file_type::mpeg_layer3;
        case kAudioFileCAFType:
            return file_type::core_audio_format;
        case kAudioFileMPEG4Type:
            return file_type::mpeg4;
        case kAudioFileM4AType:
            return file_type::apple_m4a;
        case kAudioFileWAVEType:
            return file_type::wave;
        default:
            break;
    }
    return nil;
}

#pragma mark - audio file

namespace yas {
namespace audio_file_utils {
    static Boolean open(AudioFileID *file_id, const CFURLRef url) {
        OSStatus err = AudioFileOpenURL(url, kAudioFileReadPermission, kAudioFileWAVEType, file_id);
        return err == noErr;
    }

    static Boolean close(const AudioFileID file_id) {
        OSStatus err = AudioFileClose(file_id);
        return err == noErr;
    }

    static AudioFileTypeID get_audio_file_type_id(const AudioFileID file_id) {
        UInt32 fileType;
        UInt32 size = sizeof(AudioFileTypeID);
        raise_if_au_error(AudioFileGetProperty(file_id, kAudioFilePropertyFileFormat, &size, &fileType));
        return fileType;
    }

    static Boolean get_audio_file_format(AudioStreamBasicDescription *asbd, const AudioFileID file_id) {
        UInt32 size = sizeof(AudioStreamBasicDescription);
        OSStatus err = AudioFileGetProperty(file_id, kAudioFilePropertyDataFormat, &size, asbd);
        return err == noErr;
    }
}
}

#pragma mark - ext audio file

Boolean audio::ext_audio_file_utils::can_open(const CFURLRef url) {
    Boolean result = true;
    AudioFileID file_id;
    AudioStreamBasicDescription asbd;
    if (audio_file_utils::open(&file_id, url)) {
        if (!audio_file_utils::get_audio_file_format(&asbd, file_id)) {
            result = false;
        }
        audio_file_utils::close(file_id);
    } else {
        result = false;
    }
    return result;
}

Boolean audio::ext_audio_file_utils::open(ExtAudioFileRef *ext_audio_file, const CFURLRef url) {
    OSStatus err = ExtAudioFileOpenURL(url, ext_audio_file);
    return err == noErr;
}

Boolean audio::ext_audio_file_utils::create(ExtAudioFileRef *extAudioFile, const CFURLRef url,
                                            const AudioFileTypeID file_type_id,
                                            const AudioStreamBasicDescription &asbd) {
    OSStatus err = ExtAudioFileCreateWithURL(url, file_type_id, &asbd, NULL, kAudioFileFlags_EraseFile, extAudioFile);
    return err == noErr;
}

Boolean audio::ext_audio_file_utils::dispose(const ExtAudioFileRef ext_audio_file) {
    OSStatus err = ExtAudioFileDispose(ext_audio_file);
    return err == noErr;
}

Boolean audio::ext_audio_file_utils::set_client_format(const AudioStreamBasicDescription &asbd,
                                                       const ExtAudioFileRef ext_audio_file) {
    UInt32 size = sizeof(AudioStreamBasicDescription);
    OSStatus err = noErr;
    yas_raise_if_au_error(
        err = ExtAudioFileSetProperty(ext_audio_file, kExtAudioFileProperty_ClientDataFormat, size, &asbd));
    return err == noErr;
}

Boolean audio::ext_audio_file_utils::get_audio_file_format(AudioStreamBasicDescription *asbd,
                                                           const ExtAudioFileRef ext_audio_file) {
    UInt32 size = sizeof(AudioStreamBasicDescription);
    OSStatus err = noErr;
    yas_raise_if_au_error(
        err = ExtAudioFileGetProperty(ext_audio_file, kExtAudioFileProperty_FileDataFormat, &size, asbd));
    return err == noErr;
}

AudioFileID audio::ext_audio_file_utils::get_audio_file_id(const ExtAudioFileRef ext_audio_file) {
    UInt32 size = sizeof(AudioFileID);
    AudioFileID file_id = 0;
    yas_raise_if_au_error(ExtAudioFileGetProperty(ext_audio_file, kExtAudioFileProperty_AudioFile, &size, &file_id));
    return file_id;
}

SInt64 audio::ext_audio_file_utils::get_file_length_frames(const ExtAudioFileRef ext_audio_file) {
    SInt64 result = 0;
    UInt32 size = sizeof(SInt64);
    yas_raise_if_au_error(
        ExtAudioFileGetProperty(ext_audio_file, kExtAudioFileProperty_FileLengthFrames, &size, &result));
    return result;
}

AudioFileTypeID audio::ext_audio_file_utils::get_audio_file_type_id(const ExtAudioFileRef ext_audio_file) {
    AudioFileID file_id = get_audio_file_id(ext_audio_file);
    return audio_file_utils::get_audio_file_type_id(file_id);
}

CFStringRef get_audio_file_type(const ExtAudioFileRef ext_audio_file) {
    return audio::to_file_type(audio::ext_audio_file_utils::get_audio_file_type_id(ext_audio_file));
}

#pragma mark -

CFDictionaryRef yas::audio::wave_file_settings(const Float64 sample_rate, const UInt32 channel_count,
                                               const UInt32 bit_depth) {
    return linear_pcm_file_settings(sample_rate, channel_count, bit_depth, false, bit_depth >= 32, false);
}

CFDictionaryRef yas::audio::aiff_file_settings(const Float64 sample_rate, const UInt32 channel_count,
                                               const UInt32 bit_depth) {
    return linear_pcm_file_settings(sample_rate, channel_count, bit_depth, true, bit_depth >= 32, false);
}

CFDictionaryRef yas::audio::linear_pcm_file_settings(const Float64 sample_rate, const UInt32 channel_count,
                                                     const UInt32 bit_depth, const bool is_big_endian,
                                                     const bool is_float, const bool is_non_interleaved) {
    return (__bridge CFDictionaryRef) @{
        AVFormatIDKey: @(kAudioFormatLinearPCM),
        AVSampleRateKey: @(sample_rate),
        AVNumberOfChannelsKey: @(channel_count),
        AVLinearPCMBitDepthKey: @(bit_depth),
        AVLinearPCMIsBigEndianKey: @(is_big_endian),
        AVLinearPCMIsFloatKey: @(is_float),
        AVLinearPCMIsNonInterleaved: @(is_non_interleaved),
        AVChannelLayoutKey: [NSData data]
    };
}

CFDictionaryRef yas::audio::aac_settings(const Float64 sample_rate, const UInt32 channel_count, const UInt32 bit_depth,
                                         const AVAudioQuality encoder_quality, const UInt32 bit_rate,
                                         const UInt32 bit_depth_hint, const AVAudioQuality converter_quality) {
    return (__bridge CFDictionaryRef) @{
        AVFormatIDKey: @(kAudioFormatMPEG4AAC),
        AVSampleRateKey: @(sample_rate),
        AVNumberOfChannelsKey: @(channel_count),
        AVLinearPCMBitDepthKey: @(bit_depth),
        AVLinearPCMIsBigEndianKey: @(NO),
        AVLinearPCMIsFloatKey: @(NO),
        AVEncoderAudioQualityKey: @(encoder_quality),
        AVEncoderBitRateKey: @(bit_rate),
        AVEncoderBitDepthHintKey: @(bit_depth_hint),
        AVSampleRateConverterAudioQualityKey: @(converter_quality)
    };
}
