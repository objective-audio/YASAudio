//
//  yas_audio_unit_io_node.mm
//  Copyright (c) 2015 Yuki Yasoshima.
//

#include "yas_audio_unit_io_node.h"
#include "yas_audio_tap_node.h"
#include "yas_audio_time.h"

#if TARGET_OS_IPHONE
#import <AVFoundation/AVFoundation.h>
namespace yas
{
    OSType const audio_unit_sub_type_default_io = kAudioUnitSubType_RemoteIO;
}
#elif TARGET_OS_MAC
#include "yas_audio_device.h"
namespace yas
{
    OSType const audio_unit_sub_type_default_io = kAudioUnitSubType_HALOutput;
}
#endif

using namespace yas;

class audio_unit_io_node::impl
{
   public:
    static const UInt32 channel_map_count = 2;
    channel_map_t channel_map[2];
};

audio_unit_io_node::audio_unit_io_node()
    : audio_unit_node({
          .componentType = kAudioUnitType_Output,
          .componentSubType = audio_unit_sub_type_default_io,
          .componentManufacturer = kAudioUnitManufacturer_Apple,
          .componentFlags = 0,
          .componentFlagsMask = 0,
      }),
      _impl(std::make_unique<impl>())
{
}

audio_unit_io_node_sptr audio_unit_io_node::create()
{
    auto node = audio_unit_io_node_sptr(new audio_unit_io_node());
    prepare_for_create(node);
    return node;
}

audio_unit_io_node::~audio_unit_io_node() = default;

void audio_unit_io_node::set_channel_map(const channel_map_t &map, const yas::direction dir)
{
    _impl->channel_map[yas::to_uint32(dir)] = map;

    if (const auto unit = audio_unit()) {
        unit->set_channel_map(map, kAudioUnitScope_Output, yas::to_uint32(dir));
    }
}

const channel_map_t &audio_unit_io_node::channel_map(const yas::direction dir) const
{
    return _impl->channel_map[yas::to_uint32(dir)];
}

void audio_unit_io_node::prepare_audio_unit()
{
    auto unit = audio_unit();
    unit->set_enable_output(true);
    unit->set_enable_input(true);
    unit->set_maximum_frames_per_slice(4096);
}

bus_result_t audio_unit_io_node::next_available_output_bus() const
{
    auto result = super_class::next_available_output_bus();
    if (result && *result == 0) {
        return 1;
    }
    return result;
}

bool audio_unit_io_node::is_available_output_bus(const UInt32 bus_idx) const
{
    if (bus_idx == 1) {
        return super_class::is_available_output_bus(0);
    }
    return false;
}

Float64 audio_unit_io_node::device_sample_rate() const
{
#if TARGET_OS_IPHONE
    return [AVAudioSession sharedInstance].sampleRate;
#elif TARGET_OS_MAC
    if (const auto &dev = device()) {
        return dev->nominal_sample_rate();
    }
    return 0;
#endif
}

UInt32 audio_unit_io_node::output_device_channel_count() const
{
#if TARGET_OS_IPHONE
    return static_cast<UInt32>([AVAudioSession sharedInstance].outputNumberOfChannels);
#elif TARGET_OS_MAC
    if (const auto &dev = device()) {
        return dev->output_channel_count();
    }
    return 0;
#endif
}

UInt32 audio_unit_io_node::input_device_channel_count() const
{
#if TARGET_OS_IPHONE
    return static_cast<UInt32>([AVAudioSession sharedInstance].inputNumberOfChannels);
#elif TARGET_OS_MAC
    if (const auto &dev = device()) {
        return dev->input_channel_count();
    }
    return 0;
#endif
}

#if (TARGET_OS_MAC && !TARGET_OS_IPHONE)

void audio_unit_io_node::set_device(const audio_device_sptr &device)
{
    if (!device) {
        throw std::invalid_argument(std::string(__PRETTY_FUNCTION__) + " : argument is null.");
    }

    audio_unit()->set_current_device(device->audio_device_id());
}

audio_device_sptr audio_unit_io_node::device() const
{
    return audio_device::device_for_id(audio_unit()->current_device());
}

#endif

void audio_unit_io_node::update_connections()
{
    super_class::update_connections();

    auto unit = audio_unit();

    auto update_channel_map = [](channel_map_t &map, const yas::audio_format_sptr format, const UInt32 dev_ch_count) {
        if (map.size() > 0) {
            if (format) {
                const UInt32 ch_count = format->channel_count();
                if (map.size() != ch_count) {
                    map.resize(ch_count, -1);
                }
                for (auto &value : map) {
                    if (value >= dev_ch_count) {
                        value = -1;
                    }
                }
            }
        }
    };

    const auto output_idx = yas::to_uint32(yas::direction::output);
    auto &output_map = _impl->channel_map[output_idx];
    update_channel_map(output_map, input_format(output_idx), output_device_channel_count());

    const auto input_idx = yas::to_uint32(yas::direction::input);
    auto &input_map = _impl->channel_map[input_idx];
    update_channel_map(input_map, output_format(input_idx), input_device_channel_count());

    unit->set_channel_map(output_map, kAudioUnitScope_Output, output_idx);
    unit->set_channel_map(input_map, kAudioUnitScope_Output, input_idx);
}

#pragma mark - audio_unit_output_node

audio_unit_output_node_sptr audio_unit_output_node::create()
{
    auto node = audio_unit_output_node_sptr(new audio_unit_output_node());
    prepare_for_create(node);
    return node;
}

void audio_unit_output_node::prepare_audio_unit()
{
    auto unit = audio_unit();
    unit->set_enable_output(true);
    unit->set_enable_input(false);
    unit->set_maximum_frames_per_slice(4096);
}

UInt32 audio_unit_output_node::input_bus_count() const
{
    return 1;
}

UInt32 audio_unit_output_node::output_bus_count() const
{
    return 0;
}

void audio_unit_output_node::set_channel_map(const channel_map_t &map)
{
    super_class::set_channel_map(map, yas::direction::output);
}

const channel_map_t &audio_unit_output_node::channel_map() const
{
    return super_class::channel_map(yas::direction::output);
}

#pragma mark - audio_unit_input_node

class audio_unit_input_node::impl
{
   public:
    audio_pcm_buffer_sptr input_buffer;
    audio_time_sptr render_time;
};

audio_unit_input_node_sptr audio_unit_input_node::create()
{
    auto node = audio_unit_input_node_sptr(new audio_unit_input_node());
    prepare_for_create(node);
    node->_weak_this = node;
    return node;
}

audio_unit_input_node::audio_unit_input_node() : audio_unit_io_node(), _impl(std::make_unique<impl>())
{
}

void audio_unit_input_node::prepare_audio_unit()
{
    auto unit = audio_unit();
    unit->set_enable_output(false);
    unit->set_enable_input(true);
    unit->set_maximum_frames_per_slice(4096);
}

UInt32 audio_unit_input_node::input_bus_count() const
{
    return 0;
}

UInt32 audio_unit_input_node::output_bus_count() const
{
    return 1;
}

void audio_unit_input_node::set_channel_map(const channel_map_t &map)
{
    super_class::set_channel_map(map, yas::direction::input);
}

const channel_map_t &audio_unit_input_node::channel_map() const
{
    return super_class::channel_map(yas::direction::input);
}

void audio_unit_input_node::update_connections()
{
    super_class::update_connections();

    auto unit = audio_unit();

    if (auto out_connection = output_connection(1)) {
        unit->attach_input_callback();

        auto input_buffer = audio_pcm_buffer::create(out_connection->format(), 4096);
        _impl->input_buffer = input_buffer;

        unit->set_input_callback([weak_node = _weak_this, input_buffer](render_parameters & render_parameters) {
            auto input_node = weak_node.lock();
            if (input_node && render_parameters.in_number_frames <= input_buffer->frame_capacity()) {
                input_buffer->set_frame_length(render_parameters.in_number_frames);
                render_parameters.io_data = input_buffer->audio_buffer_list();

                if (const auto core = input_node->node_core()) {
                    if (const auto connection = core->output_connection(1)) {
                        auto format = connection->format();
                        auto time = audio_time::create(*render_parameters.io_time_stamp, format->sample_rate());
                        input_node->set_render_time_on_render(time);

                        if (auto io_unit = input_node->audio_unit()) {
                            render_parameters.in_bus_number = 1;
                            io_unit->audio_unit_render(render_parameters);
                        }

                        auto destination_node = connection->destination_node();

                        if (auto *input_tap_node = dynamic_cast<audio_input_tap_node *>(destination_node.get())) {
                            input_tap_node->render(input_buffer, 0, time);
                        }
                    }
                }
            }
        });
    } else {
        unit->detach_input_callback();
        unit->set_input_callback(nullptr);
        _impl->input_buffer = nullptr;
    }
}
