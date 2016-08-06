//
//  YASAudioEngineRouteSampleViewController.m
//

#import "YASAudioEngineRouteSampleSelectionViewController.h"
#import "YASAudioEngineRouteSampleViewController.h"
#import "YASAudioSliderCell.h"
#import "yas_audio.h"
#import "yas_objc_unowned.h"

using namespace yas;

static uint32_t const YASAudioEngineRouteSampleDestinationChannelCount = 2;

typedef NS_ENUM(NSUInteger, YASAudioEngineRouteSampleSection) {
    YASAudioEngineRouteSampleSectionSlider,
    YASAudioEngineRouteSampleSectionDestinations,
    YASAudioEngineRouteSampleSectionCount,
};

typedef NS_ENUM(NSUInteger, YASAudioEngineRouteSampleSourceIndex) {
    YASAudioEngineRouteSampleSourceIndexSine,
    YASAudioEngineRouteSampleSourceIndexInput,
    YASAudioEngineRouteSampleSourceIndexCount,
};

@interface YASAudioEngineRouteSampleViewController ()

@property (nonatomic, strong) IBOutlet UISlider *slider;

@end

namespace yas {
namespace sample {
    struct route_vc_internal {
        audio::engine engine;
        audio::unit_io_node io_node;
        audio::unit_mixer_node mixer_node;
        audio::route_node route_node;
        audio::tap_node sine_node;

        base engine_observer = nullptr;

        void disconnectNodes() {
            engine.disconnect(mixer_node.unit_node().node());
            engine.disconnect(route_node.node());
            engine.disconnect(sine_node.node());
            engine.disconnect(io_node.unit_node().node());
        }

        void connect_nodes() {
            auto const sample_rate = io_node.device_sample_rate();

            auto const format = audio::format({.sample_rate = sample_rate, .channel_count = 2});

            engine.connect(mixer_node.unit_node().node(), io_node.unit_node().node(), format);
            engine.connect(route_node.node(), mixer_node.unit_node().node(), format);
            engine.connect(sine_node.node(), route_node.node(), 0, YASAudioEngineRouteSampleSourceIndexSine, format);
            engine.connect(io_node.unit_node().node(), route_node.node(), 1, YASAudioEngineRouteSampleSourceIndexInput, format);
        }
    };
}
}

@implementation YASAudioEngineRouteSampleViewController {
    sample::route_vc_internal _internal;
}

- (void)dealloc {
    yas_release(_slider);

    _slider = nil;

    yas_super_dealloc();
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];

    if (self.isMovingToParentViewController) {
        BOOL success = NO;
        NSString *errorMessage = nil;
        NSError *error = nil;

        if ([[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayAndRecord error:&error]) {
            [self setupEngine];

            auto const start_result = _internal.engine.start_render();
            if (start_result) {
                [self.tableView reloadData];
                [self _updateSlider];
                success = YES;
            } else {
                auto const error_string = to_string(start_result.error());
                errorMessage = (__bridge NSString *)to_cf_object(error_string);
            }
        } else {
            errorMessage = error.description;
        }

        if (!success) {
            [self _showErrorAlertWithMessage:errorMessage];
        }
    }
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];

    if (self.isMovingFromParentViewController) {
        if (_internal.engine) {
            _internal.engine.stop();
        }

        [[AVAudioSession sharedInstance] setActive:NO error:nil];
    }
}

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(UITableViewCell *)cell {
    id destinationViewController = segue.destinationViewController;
    if ([destinationViewController isKindOfClass:[YASAudioEngineRouteSampleSelectionViewController class]]) {
        YASAudioEngineRouteSampleSelectionViewController *controller = destinationViewController;
        NSIndexPath *indexPath = [self.tableView indexPathForCell:cell];
        controller.fromCellIndexPath = indexPath;
    }
}

- (IBAction)unwindForSegue:(UIStoryboardSegue *)segue {
    id sourceViewController = segue.sourceViewController;
    if ([sourceViewController isKindOfClass:[YASAudioEngineRouteSampleSelectionViewController class]]) {
        YASAudioEngineRouteSampleSelectionViewController *controller = sourceViewController;
        NSIndexPath *fromIndexPath = controller.fromCellIndexPath;
        NSIndexPath *selectedIndexPath = controller.selectedIndexPath;

        if (selectedIndexPath) {
            auto const dst_bus_idx = 0;
            auto const dst_ch_idx = static_cast<uint32_t>(fromIndexPath.row);
            uint32_t src_bus_idx = -1;
            uint32_t src_ch_idx = -1;

            if (selectedIndexPath.section == YASAudioEngineRouteSampleSelectionSectionSine) {
                src_bus_idx = YASAudioEngineRouteSampleSourceIndexSine;
                src_ch_idx = static_cast<uint32_t>(selectedIndexPath.row);
            } else if (selectedIndexPath.section == YASAudioEngineRouteSampleSelectionSectionInput) {
                src_bus_idx = YASAudioEngineRouteSampleSourceIndexInput;
                src_ch_idx = static_cast<uint32_t>(selectedIndexPath.row);
            }

            if (src_bus_idx == -1 || src_ch_idx == -1) {
                _internal.route_node.remove_route_for_destination({dst_bus_idx, dst_ch_idx});
            } else {
                _internal.route_node.add_route({src_bus_idx, src_ch_idx, dst_bus_idx, dst_ch_idx});
            }

            [self.tableView reloadSections:[NSIndexSet indexSetWithIndex:fromIndexPath.section]
                          withRowAnimation:UITableViewRowAnimationAutomatic];
        }
    }
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    if (!_internal.engine) {
        return 0;
    }

    return YASAudioEngineRouteSampleSectionCount;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    if (!_internal.engine) {
        return 0;
    }

    switch (section) {
        case YASAudioEngineRouteSampleSectionSlider:
            return 1;
        case YASAudioEngineRouteSampleSectionDestinations:
            return YASAudioEngineRouteSampleDestinationChannelCount;
        default:
            break;
    }
    return 0;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    switch (indexPath.section) {
        case YASAudioEngineRouteSampleSectionSlider: {
            YASAudioSliderCell *cell =
                [tableView dequeueReusableCellWithIdentifier:@"SliderCell" forIndexPath:indexPath];
            return cell;
        }

        case YASAudioEngineRouteSampleSectionDestinations: {
            UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"Cell" forIndexPath:indexPath];
            auto const &routes = _internal.route_node.routes();
            audio::route::point dst_point{0, static_cast<uint32_t>(indexPath.row)};
            auto it = std::find_if(routes.begin(), routes.end(),
                                   [dst_point = std::move(dst_point)](const audio::route &route) {
                                       return route.destination == dst_point;
                                   });
            NSString *sourceIndexText = nil;
            if (it != routes.end()) {
                auto const &route = *it;
                sourceIndexText =
                    [NSString stringWithFormat:@"bus=%@ ch=%@", @(route.source.bus), @(route.source.channel)];
            } else {
                sourceIndexText = @"None";
            }
            cell.textLabel.text =
                [NSString stringWithFormat:@"Destination : %@ - Source : %@", @(indexPath.row), sourceIndexText];
            return cell;
        }
    }

    return nil;
}

#pragma mark -

- (IBAction)volumeSliderChanged:(UISlider *)sender {
    float const value = sender.value;
    if (_internal.mixer_node) {
        _internal.mixer_node.set_input_volume(value, 0);
    }
}

#pragma mark -

- (void)setupEngine {
    _internal = sample::route_vc_internal();

    _internal.mixer_node.set_input_volume(1.0, 0);
    _internal.route_node.set_routes({{0, 0, 0, 0}, {0, 1, 0, 1}});

    double phase = 0;

    auto tap_render_function = [phase](audio::pcm_buffer &buffer, uint32_t const bus_idx,
                                       const audio::time &when) mutable {
        buffer.clear();

        double const start_phase = phase;
        double const phase_per_frame = 1000.0 / buffer.format().sample_rate() * audio::math::two_pi;
        audio::frame_enumerator enumerator(buffer);
        auto const *flex_ptr = enumerator.pointer();
        uint32_t const length = enumerator.frame_length();

        while (flex_ptr->v) {
            phase = audio::math::fill_sine(flex_ptr->f32, length, start_phase, phase_per_frame);
            yas_audio_frame_enumerator_move_channel(enumerator);
        }
    };

    _internal.sine_node.set_render_function(tap_render_function);

    auto unowned_self = make_objc_ptr([[YASUnownedObject alloc] init]);
    [unowned_self.object() setObject:self];

    _internal.engine_observer = _internal.engine.subject().make_observer(
        audio::engine::method::configuration_change, [unowned_self](auto const &context) {
            if ([UIApplication sharedApplication].applicationState == UIApplicationStateActive) {
                [[unowned_self.object() object] _updateEngine];
            }
        });

    _internal.connect_nodes();
}

- (void)_updateEngine {
    _internal.disconnectNodes();
    _internal.connect_nodes();

    [self.tableView reloadData];
    [self _updateSlider];
}

#pragma mark -

- (void)_showErrorAlertWithMessage:(NSString *)message {
    UIAlertController *controller = [UIAlertController alertControllerWithTitle:@"Error"
                                                                        message:message
                                                                 preferredStyle:UIAlertControllerStyleAlert];
    [controller addAction:[UIAlertAction actionWithTitle:@"OK"
                                                   style:UIAlertActionStyleDefault
                                                 handler:^(UIAlertAction *action) {
                                                     [self.navigationController popViewControllerAnimated:YES];
                                                 }]];
    [self presentViewController:controller animated:YES completion:NULL];
}

- (void)_updateSlider {
    if (_internal.mixer_node) {
        NSIndexPath *indexPath = [NSIndexPath indexPathForRow:0 inSection:YASAudioEngineRouteSampleSectionSlider];
        UITableViewCell *cell = [self.tableView cellForRowAtIndexPath:indexPath];
        if (cell) {
            for (UIView *view in cell.contentView.subviews) {
                if ([view isKindOfClass:[UISlider class]]) {
                    UISlider *slider = (UISlider *)view;
                    slider.value = _internal.mixer_node.input_volume(0);
                }
            }
        }
    }
}

@end
