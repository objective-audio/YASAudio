//
//  YASAudioFormat.m
//  Copyright (c) 2015 Yuki Yasoshima.
//

#import "YASAudioFormat.h"
#import "YASMacros.h"
#import "NSException+YASAudio.h"
#import "NSString+YASAudio.h"
#import <AVFoundation/AVFoundation.h>

static UInt32 YASAudioSampleByteCountWithPCMFormat(YASAudioPCMFormat pcmFormat)
{
    switch (pcmFormat) {
        case YASAudioPCMFormatFloat32:
        case YASAudioPCMFormatFixed824:
            return 4;
        case YASAudioPCMFormatInt16:
            return 2;
        case YASAudioPCMFormatFloat64:
            return 8;
        default:
            return 0;
    }
}

@implementation YASAudioFormat {
    AudioStreamBasicDescription _asbd;
    YASAudioPCMFormat _pcmFormat;
}

- (instancetype)initWithStreamDescription:(const AudioStreamBasicDescription *)asbd
{
    self = [super init];
    if (self) {
        _asbd = *asbd;
        _asbd.mReserved = 0;
        _pcmFormat = YASAudioPCMFormatOther;
        _standard = NO;
        if (_asbd.mFormatID == kAudioFormatLinearPCM) {
            if ((_asbd.mFormatFlags & kAudioFormatFlagIsFloat) &&
                ((_asbd.mFormatFlags & kAudioFormatFlagIsBigEndian) == kAudioFormatFlagsNativeEndian) &&
                (_asbd.mFormatFlags & kAudioFormatFlagIsPacked)) {
                if (_asbd.mBitsPerChannel == 64) {
                    _pcmFormat = YASAudioPCMFormatFloat64;
                } else if (_asbd.mBitsPerChannel == 32) {
                    _pcmFormat = YASAudioPCMFormatFloat32;
                    if (_asbd.mFormatFlags & kAudioFormatFlagIsNonInterleaved) {
                        _standard = YES;
                    }
                }
            } else if ((_asbd.mFormatFlags & kAudioFormatFlagIsSignedInteger) &&
                       ((_asbd.mFormatFlags & kAudioFormatFlagIsBigEndian) == kAudioFormatFlagsNativeEndian) &&
                       (_asbd.mFormatFlags & kAudioFormatFlagIsPacked)) {
                UInt32 fraction = (_asbd.mFormatFlags & kLinearPCMFormatFlagsSampleFractionMask) >>
                                  kLinearPCMFormatFlagsSampleFractionShift;
                if (_asbd.mBitsPerChannel == 32 && fraction == 24) {
                    _pcmFormat = YASAudioPCMFormatFixed824;
                } else if (_asbd.mBitsPerChannel == 16) {
                    _pcmFormat = YASAudioPCMFormatInt16;
                }
            }
        }
    }
    return self;
}

- (instancetype)initStandardFormatWithSampleRate:(double)sampleRate channels:(UInt32)channels
{
    return [self initWithPCMFormat:YASAudioPCMFormatFloat32 sampleRate:sampleRate channels:channels interleaved:NO];
}

- (instancetype)initWithPCMFormat:(YASAudioPCMFormat)pcmFormat
                       sampleRate:(double)sampleRate
                         channels:(UInt32)channels
                      interleaved:(BOOL)interleaved
{
    if (pcmFormat == YASAudioPCMFormatOther || channels == 0) {
        YASRaiseWithReason(
            ([NSString stringWithFormat:@"%s - Invalid argument. bitDepth(%@) channels(%@)", __PRETTY_FUNCTION__,
                                        [NSString yas_stringWithPCMFormat:pcmFormat], @(channels)]));
        YASRelease(self);
        return nil;
    }

    AudioStreamBasicDescription asbd = {
        .mSampleRate = sampleRate, .mFormatID = kAudioFormatLinearPCM,
    };

    asbd.mFormatFlags = kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked;

    if (pcmFormat == YASAudioPCMFormatFloat32 || pcmFormat == YASAudioPCMFormatFloat64) {
        asbd.mFormatFlags |= kAudioFormatFlagIsFloat;
    } else if (pcmFormat == YASAudioPCMFormatInt16) {
        asbd.mFormatFlags |= kAudioFormatFlagIsSignedInteger;
    } else if (pcmFormat == YASAudioPCMFormatFixed824) {
        asbd.mFormatFlags |= kAudioFormatFlagIsSignedInteger | (24 << kLinearPCMFormatFlagsSampleFractionShift);
    }

    if (!interleaved) {
        asbd.mFormatFlags |= kAudioFormatFlagIsNonInterleaved;
    }

    if (pcmFormat == YASAudioPCMFormatFloat64) {
        asbd.mBitsPerChannel = 64;
    } else if (pcmFormat == YASAudioPCMFormatInt16) {
        asbd.mBitsPerChannel = 16;
    } else {
        asbd.mBitsPerChannel = 32;
    }

    asbd.mChannelsPerFrame = channels;

    UInt32 size = sizeof(AudioStreamBasicDescription);
    YASRaiseIfAUError(AudioFormatGetProperty(kAudioFormatProperty_FormatInfo, 0, NULL, &size, &asbd));

    return [self initWithStreamDescription:&asbd];
}

- (instancetype)initWithSettings:(NSDictionary *)settings
{
    AudioStreamBasicDescription asbd;
    [settings yas_getStreamDescription:&asbd];
    return [self initWithStreamDescription:&asbd];
}

- (UInt32)channelCount
{
    return _asbd.mChannelsPerFrame;
}

- (UInt32)bufferCount
{
    return self.isInterleaved ? 1 : _asbd.mChannelsPerFrame;
}

- (UInt32)stride
{
    return self.isInterleaved ? _asbd.mChannelsPerFrame : 1;
}

- (double)sampleRate
{
    return _asbd.mSampleRate;
}

- (BOOL)isInterleaved
{
    return !(_asbd.mFormatFlags & kAudioFormatFlagIsNonInterleaved);
}

- (const AudioStreamBasicDescription *)streamDescription
{
    return &_asbd;
}

- (UInt32)sampleByteCount
{
    return YASAudioSampleByteCountWithPCMFormat(_pcmFormat);
}

- (BOOL)isEqual:(id)other
{
    if (other == self) {
        return YES;
    } else if (![other isKindOfClass:self.class]) {
        return NO;
    } else {
        return [self isEqualToAudioFormat:other];
    }
}

- (BOOL)isEqualToAudioFormat:(YASAudioFormat *)otherFormat
{
    if (self == otherFormat) {
        return YES;
    } else {
        return memcmp(self.streamDescription, otherFormat.streamDescription, sizeof(AudioStreamBasicDescription)) == 0;
    }
}

- (NSUInteger)hash
{
    NSUInteger hash = _asbd.mFormatID + _asbd.mFormatFlags + _asbd.mChannelsPerFrame + _asbd.mBitsPerChannel;
    return hash;
}

- (NSString *)description
{
    NSMutableString *result = [NSMutableString stringWithFormat:@"<%@: %p>\n", self.class, self];
    NSDictionary *asbdDict = @{
        @"pcmFormat": [NSString yas_stringWithPCMFormat:_pcmFormat],
        @"sampleRate": @(_asbd.mSampleRate),
        @"bitsPerChannel": @(_asbd.mBitsPerChannel),
        @"bytesPerFrame": @(_asbd.mBytesPerFrame),
        @"bytesPerPacket": @(_asbd.mBytesPerPacket),
        @"channelsPerFrame": @(_asbd.mChannelsPerFrame),
        @"formatFlags": [self _formatFlagsString],
        @"formatID": [NSString yas_fileTypeStringWithHFSTypeCode:_asbd.mFormatID],
        @"framesPerPacket": @(_asbd.mFramesPerPacket)
    };
    [result appendString:asbdDict.description];
    return result;
}

- (NSString *)_formatFlagsString
{
    NSDictionary *flags = @{
        @(kAudioFormatFlagIsFloat): @"kAudioFormatFlagIsFloat",
        @(kAudioFormatFlagIsBigEndian): @"kAudioFormatFlagIsBigEndian",
        @(kAudioFormatFlagIsSignedInteger): @"kAudioFormatFlagIsSignedInteger",
        @(kAudioFormatFlagIsPacked): @"kAudioFormatFlagIsPacked",
        @(kAudioFormatFlagIsAlignedHigh): @"kAudioFormatFlagIsAlignedHigh",
        @(kAudioFormatFlagIsNonInterleaved): @"kAudioFormatFlagIsNonInterleaved",
        @(kAudioFormatFlagIsNonMixable): @"kAudioFormatFlagIsNonMixable"
    };
    NSMutableString *result = [NSMutableString string];
    for (NSNumber *flag in flags) {
        if (_asbd.mFormatFlags & flag.unsignedIntegerValue) {
            if (result.length != 0) {
                [result appendString:@" | "];
            }
            [result appendString:flags[flag]];
        }
    }
    return result;
}

@end

@implementation NSDictionary (YASAudioFormat)

- (void)yas_getStreamDescription:(AudioStreamBasicDescription *)outFormat
{
    memset(outFormat, 0, sizeof(AudioStreamBasicDescription));

    NSNumber *formatID = self[AVFormatIDKey];
    NSNumber *sampleRate = self[AVSampleRateKey];
    NSNumber *numberOfChannels = self[AVNumberOfChannelsKey];
    NSNumber *bitDepth = self[AVLinearPCMBitDepthKey];

    outFormat->mFormatID = (UInt32)formatID.unsignedLongValue;
    outFormat->mSampleRate = (Float64)sampleRate.doubleValue;
    outFormat->mChannelsPerFrame = (UInt32)numberOfChannels.unsignedLongValue;
    outFormat->mBitsPerChannel = (UInt32)bitDepth.unsignedLongValue;

    if (outFormat->mFormatID == kAudioFormatLinearPCM) {
        NSNumber *isBigEndian = self[AVLinearPCMIsBigEndianKey];
        NSNumber *isFloat = self[AVLinearPCMIsFloatKey];
        NSNumber *isNonInterleaved = self[AVLinearPCMIsNonInterleaved];

        outFormat->mFormatFlags = kAudioFormatFlagIsPacked;
        if (isBigEndian.boolValue) {
            outFormat->mFormatFlags |= kAudioFormatFlagIsBigEndian;
        }
        if (isFloat.boolValue) {
            outFormat->mFormatFlags |= kAudioFormatFlagIsFloat;
        } else {
            outFormat->mFormatFlags |= kAudioFormatFlagIsSignedInteger;
        }
        if (isNonInterleaved.boolValue) {
            outFormat->mFormatFlags |= kAudioFormatFlagIsNonInterleaved;
        }
    }

    UInt32 size = sizeof(AudioStreamBasicDescription);

    AudioFormatGetProperty(kAudioFormatProperty_FormatInfo, 0, NULL, &size, outFormat);
}

@end
