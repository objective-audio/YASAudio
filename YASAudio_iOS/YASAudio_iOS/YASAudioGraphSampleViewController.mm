//
//  YASAudioGraphSampleViewController.m
//  Copyright (c) 2015 Yuki Yasoshima.
//

#import "YASAudioGraphSampleViewController.h"
#import <AVFoundation/AVFoundation.h>
#import "yas_audio_graph.h"
#import "yas_audio_unit.h"
#import "yas_audio_format.h"
#import "YASMacros.h"
#import <memory>
#import <exception>
#import <iostream>

@interface YASAudioGraphSampleViewController ()

@property (nonatomic, strong) IBOutlet UISlider *slider;

@end

@implementation YASAudioGraphSampleViewController {
    yas::audio_graph_ptr _audio_graph;
    yas::audio_unit_ptr _io_unit;
    yas::audio_unit_ptr _mixer_unit;
}

- (void)dealloc
{
    YASRelease(_slider);

    _slider = nil;

    YASSuperDealloc;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    [self setupAudioGraph];

    [self volumeSliderChanged:self.slider];
}

- (IBAction)volumeSliderChanged:(UISlider *)sender
{
    const AudioUnitParameterValue value = sender.value;
    _mixer_unit->set_parameter_value(value, kMultiChannelMixerParam_Volume, kAudioUnitScope_Input, 0);
}

- (void)setupAudioGraph
{
    AVAudioSession *audioSession = [AVAudioSession sharedInstance];
    [audioSession setCategory:AVAudioSessionCategoryPlayAndRecord error:nil];
    const Float64 sample_rate = [audioSession sampleRate];

    auto format = yas::audio_format::create(sample_rate, 2);
    _audio_graph = yas::audio_graph::create();

    _io_unit = yas::audio_unit::create(kAudioUnitType_Output, yas::audio_unit::sub_type_default_io());
    _io_unit->set_enable_input(true);
    _io_unit->set_enable_output(true);
    _io_unit->set_maximum_frames_per_slice(4096);

    _audio_graph->add_audio_unit(_io_unit);

    _io_unit->attach_render_callback(0);
    _io_unit->set_input_format(format->stream_description(), 0);
    _io_unit->set_output_format(format->stream_description(), 1);

    _mixer_unit = yas::audio_unit::create(kAudioUnitType_Mixer, kAudioUnitSubType_MultiChannelMixer);
    _mixer_unit->set_maximum_frames_per_slice(4096);

    _audio_graph->add_audio_unit(_mixer_unit);

    _mixer_unit->attach_render_callback(0);
    _mixer_unit->set_element_count(1, kAudioUnitScope_Input);
    _mixer_unit->set_output_format(format->stream_description(), 0);
    _mixer_unit->set_input_format(format->stream_description(), 0);

    std::weak_ptr<yas::audio_unit> weak_mixer_unit = _mixer_unit;

    _io_unit->set_render_callback([weak_mixer_unit](yas::render_parameters &render_parameters) {
        if (auto shared_mixer_unit = weak_mixer_unit.lock()) {
            shared_mixer_unit->audio_unit_render(render_parameters);
        }
    });

    std::weak_ptr<yas::audio_unit> weak_io_unit = _io_unit;

    _mixer_unit->set_render_callback([weak_io_unit](yas::render_parameters &render_parameters) {
        if (auto shared_io_unit = weak_io_unit.lock()) {
            render_parameters.in_bus_number = 1;
            try {
                shared_io_unit->audio_unit_render(render_parameters);
            } catch (std::runtime_error e) {
                std::cout << e.what() << std::endl;
            }
        }
    });

    _audio_graph->start();
}

@end