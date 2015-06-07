//
//  YASAudioTypes.h
//  Copyright (c) 2015 Yuki Yasoshima.
//

#import <Foundation/Foundation.h>
#import <AudioUnit/AUComponent.h>

#ifndef __YASAudio_YASAudioTypes_h
#define __YASAudio_YASAudioTypes_h

typedef NS_ENUM(NSUInteger, YASAudioPCMFormat) {
    YASAudioPCMFormatOther = 0,
    YASAudioPCMFormatFloat32 = 1,
    YASAudioPCMFormatFloat64 = 2,
    YASAudioPCMFormatInt16 = 3,
    YASAudioPCMFormatFixed824 = 4
};

typedef NS_ENUM(NSUInteger, YASAudioUnitRenderType) {
    YASAudioUnitRenderTypeNormal,
    YASAudioUnitRenderTypeInput,
    YASAudioUnitRenderTypeNotify,
    YASAudioUnitRenderTypeUnknown,
};

typedef union YASAudioRenderID {
    void *v;
    struct {
        UInt8 graph;
        UInt16 unit;
    };
} YASAudioRenderID;

typedef struct YASAudioUnitRenderParameters {
    YASAudioUnitRenderType inRenderType;
    AudioUnitRenderActionFlags *ioActionFlags;
    const AudioTimeStamp *ioTimeStamp;
    UInt32 inBusNumber;
    UInt32 inNumberFrames;
    AudioBufferList *ioData;
    YASAudioRenderID renderID;
} YASAudioUnitRenderParameters;

typedef union YASAudioMutablePointer {
    void *v;
    Float32 *f32;
    Float64 *f64;
    SInt16 *i16;
    SInt32 *i32;
    SInt8 *i8;
    UInt8 *u8;
} YASAudioPointer;

#endif
