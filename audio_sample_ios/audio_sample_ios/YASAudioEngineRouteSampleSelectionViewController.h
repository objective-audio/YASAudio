//
//  YASAudioEngineRouteSampleSelectionViewController.h
//

#import <UIKit/UIKit.h>
#import "yas_audio_route.h"

typedef NS_ENUM(NSUInteger, YASAudioEngineRouteSampleSelectionSection) {
    YASAudioEngineRouteSampleSelectionSectionNone,
    YASAudioEngineRouteSampleSelectionSectionSine,
    YASAudioEngineRouteSampleSelectionSectionInput,
    YASAudioEngineRouteSampleSelectionSectionCount,
};

@interface YASAudioEngineRouteSampleSelectionViewController : UITableViewController

@property (nonatomic, strong) NSIndexPath *fromCellIndexPath;
@property (nonatomic, strong, readonly) NSIndexPath *selectedIndexPath;

@end