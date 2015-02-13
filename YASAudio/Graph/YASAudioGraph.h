//
//  YASAudioGraph.h
//  Copyright (c) 2015 Yuki Yasoshima.
//

#import <AudioToolbox/AudioToolbox.h>
#import "YASWeakSupport.h"
#import "YASAudioTypes.h"

@class YASAudioUnit;

@interface YASAudioGraph : YASWeakProvider

@property (nonatomic, assign, getter=isRunning) BOOL running;

- (YASAudioUnit *)addAudioUnitWithAudioComponentDescription:(const AudioComponentDescription *)acd prepareBlock:(void (^)(YASAudioUnit *audioUnit))prepareBlock;
- (YASAudioUnit *)addAudioUnitWithType:(OSType)type subType:(OSType)subType prepareBlock:(void (^)(YASAudioUnit *audioUnit))prepareBlock;
- (void)removeAudioUnit:(YASAudioUnit *)unit;

+ (BOOL)isInterrupting;

@end

@interface YASAudioGraph (YASInternal)

@property (nonatomic, copy, readonly) NSNumber *key;

+ (void)audioUnitRender:(YASAudioUnitRenderParameters *)renderParameters graphKey:(NSNumber *)graphKey unitKey:(NSNumber *)unitKey;

@end