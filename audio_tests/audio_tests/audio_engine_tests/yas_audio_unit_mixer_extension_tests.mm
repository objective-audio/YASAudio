//
//  yas_audio_unit_mixer_extension_tests.m
//

#import "yas_audio_test_utils.h"

using namespace yas;

@interface yas_audio_unit_mixer_extension_tests : XCTestCase

@end

@implementation yas_audio_unit_mixer_extension_tests

- (void)setUp {
    [super setUp];
}

- (void)tearDown {
    [super tearDown];
}

- (void)test_parameter_exists {
    audio::unit_mixer_extension mixer_ext;

    auto const &paramters = mixer_ext.unit_extension().parameters();
    auto const &input_parameters = paramters.at(kAudioUnitScope_Input);
    auto const &output_parameters = paramters.at(kAudioUnitScope_Output);

    XCTAssertGreaterThanOrEqual(input_parameters.size(), 1);
    XCTAssertGreaterThanOrEqual(output_parameters.size(), 1);

    auto input_parameter_ids = {kMultiChannelMixerParam_Volume,
                                kMultiChannelMixerParam_Enable,
                                kMultiChannelMixerParam_Pan,
                                kMultiChannelMixerParam_PreAveragePower,
                                kMultiChannelMixerParam_PrePeakHoldLevel,
                                kMultiChannelMixerParam_PostAveragePower,
                                kMultiChannelMixerParam_PostPeakHoldLevel};

    for (auto &key : input_parameter_ids) {
        XCTAssertGreaterThanOrEqual(input_parameters.count(key), 1);
    }

    auto output_parameter_ids = {kMultiChannelMixerParam_Volume, kMultiChannelMixerParam_Pan};

    for (auto &key : output_parameter_ids) {
        XCTAssertGreaterThanOrEqual(output_parameters.count(key), 1);
    }
}

- (void)test_element {
    audio::unit_mixer_extension mixer_ext;
    uint32_t const default_element_count = mixer_ext.unit_extension().input_element_count();

    XCTAssertGreaterThanOrEqual(default_element_count, 1);
    XCTAssertNoThrow(mixer_ext.set_input_volume(0.5f, 0));
    XCTAssertThrows(mixer_ext.set_input_volume(0.5f, default_element_count));

    uint32_t const element_count = default_element_count + 8;
    XCTAssertNoThrow(mixer_ext.unit_extension().audio_unit().set_element_count(element_count, kAudioUnitScope_Input));

    XCTAssertGreaterThanOrEqual(mixer_ext.unit_extension().input_element_count(), element_count);
    XCTAssertNoThrow(mixer_ext.set_input_volume(0.5f, element_count - 1));
    XCTAssertThrows(mixer_ext.set_input_volume(0.5f, element_count));
}

- (void)test_restore_parameters {
    audio::unit_mixer_extension mixer_ext;

    uint32_t const bus_idx = 0;
    float const input_volume = 0.5f;
    float const input_pan = 0.75f;
    bool const enabled = false;
    float const output_volume = 0.25f;
    float const output_pan = 0.1f;

    mixer_ext.set_input_volume(input_volume, bus_idx);
    mixer_ext.set_input_pan(input_pan, bus_idx);
    mixer_ext.set_input_enabled(enabled, bus_idx);
    mixer_ext.set_output_volume(output_volume, bus_idx);
    mixer_ext.set_output_pan(output_pan, bus_idx);

    XCTAssertEqual(mixer_ext.input_volume(bus_idx), input_volume);
    XCTAssertEqual(mixer_ext.input_pan(bus_idx), input_pan);
    XCTAssertEqual(mixer_ext.input_enabled(bus_idx), enabled);
    XCTAssertEqual(mixer_ext.output_volume(bus_idx), output_volume);
    XCTAssertEqual(mixer_ext.output_pan(bus_idx), output_pan);

    mixer_ext.unit_extension().manageable().reload_audio_unit();

    XCTAssertNotEqual(mixer_ext.input_volume(bus_idx), input_volume);
    XCTAssertNotEqual(mixer_ext.input_pan(bus_idx), input_pan);
    XCTAssertNotEqual(mixer_ext.input_enabled(bus_idx), enabled);
    XCTAssertNotEqual(mixer_ext.output_volume(bus_idx), output_volume);
    XCTAssertNotEqual(mixer_ext.output_pan(bus_idx), output_pan);

    mixer_ext.unit_extension().manageable().prepare_parameters();

    XCTAssertEqual(mixer_ext.input_volume(bus_idx), input_volume);
    XCTAssertEqual(mixer_ext.input_pan(bus_idx), input_pan);
    XCTAssertEqual(mixer_ext.input_enabled(bus_idx), enabled);
    XCTAssertEqual(mixer_ext.output_volume(bus_idx), output_volume);
    XCTAssertEqual(mixer_ext.output_pan(bus_idx), output_pan);
}

@end