//
//  YASAudioEngineEffectsSampleEditViewController.m
//  Copyright (c) 2015 Yuki Yasoshima.
//

#import "YASAudioEngineEffectsSampleEditViewController.h"
#import "YASAudioEngineSampleParameterCell.h"
#import "YASAudio.h"

@interface YASAudioEngineEffectsSampleEditViewController ()

@property (nonatomic, strong) NSArray *globalParameterInfos;

@end

@implementation YASAudioEngineEffectsSampleEditViewController

- (void)dealloc
{
    YASRelease(_audioUnitNode);
    YASRelease(_globalParameterInfos);

    _audioUnitNode = nil;
    _globalParameterInfos = nil;

    YASSuperDealloc;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
}

- (void)setAudioUnitNode:(YASAudioUnitNode *)audioUnitNode
{
    if (_audioUnitNode != audioUnitNode) {
        YASRelease(_audioUnitNode);
        _audioUnitNode = YASRetain(audioUnitNode);
    }

    self.globalParameterInfos = [_audioUnitNode.audioUnit getParameterInfosWithScope:kAudioUnitScope_Global].allValues;

    [self.tableView reloadData];
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return self.globalParameterInfos.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    YASAudioEngineSampleParameterCell *cell =
        [tableView dequeueReusableCellWithIdentifier:@"ParameterCell" forIndexPath:indexPath];
    [cell setParameterInfo:self.globalParameterInfos[indexPath.row] node:self.audioUnitNode];

    return cell;
}

@end