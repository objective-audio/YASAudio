//
//  YASWeakSupport.h
//  Copyright (c) 2015 Yuki Yasoshima.
//

#import <Foundation/Foundation.h>

@interface YASWeakContainer : NSObject

- (id)retainedObject NS_RETURNS_RETAINED;
- (id)autoreleasingObject;

@end

@interface YASWeakProvider : NSObject

@property (nonatomic, readonly) YASWeakContainer *weakContainer;

@end

@interface NSDictionary (YASWeakSupport)

- (NSDictionary *)yas_unwrappedDictionaryFromWeakContainers;

@end

@interface NSArray (YASWeakSupport)

- (NSArray *)yas_unwrappedArrayFromWeakContainers;

@end