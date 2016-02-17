//
//  yas_audio_unit.h
//

#pragma once

#include <AudioToolbox/AudioToolbox.h>
#include <exception>
#include <experimental/optional>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include "yas_audio_types.h"
#include "yas_audio_unit_protocol.h"
#include "yas_base.h"
#include "yas_exception.h"
#include "yas_result.h"

namespace yas {
namespace audio {
    class unit : public base, public unit_from_graph {
        using super_class = base;
        class impl;

       public:
        class parameter;
        using parameter_map_t = std::unordered_map<AudioUnitParameterID, parameter>;

        using render_f = std::function<void(render_parameters &)>;
        using au_result_t = yas::result<std::nullptr_t, OSStatus>;

        static OSType sub_type_default_io();

        unit(std::nullptr_t);
        explicit unit(AudioComponentDescription const &acd);
        unit(OSType const type, OSType const subType);

        ~unit() = default;

        unit(unit const &) = default;
        unit(unit &&) = default;
        unit &operator=(unit const &) = default;
        unit &operator=(unit &&) = default;

        CFStringRef name() const;
        OSType type() const;
        OSType sub_type() const;
        bool is_output_unit() const;
        AudioUnit audio_unit_instance() const;

        void attach_render_callback(UInt32 const bus_idx);
        void detach_render_callback(UInt32 const bus_idx);
        void attach_render_notify();
        void detach_render_notify();
        void attach_input_callback();  // for io
        void detach_input_callback();  // for io

        void set_render_callback(render_f callback);
        void set_notify_callback(render_f callback);
        void set_input_callback(render_f callback);  // for io

        void set_input_format(AudioStreamBasicDescription const &asbd, UInt32 const bus_idx);
        void set_output_format(AudioStreamBasicDescription const &asbd, UInt32 const bus_idx);
        AudioStreamBasicDescription input_format(UInt32 const bus_idx) const;
        AudioStreamBasicDescription output_format(UInt32 const bus_idx) const;
        void set_maximum_frames_per_slice(UInt32 const frames);
        UInt32 maximum_frames_per_slice() const;
        bool is_initialized() const;

        void set_parameter_value(AudioUnitParameterValue const value, AudioUnitParameterID const parameter_id,
                                 AudioUnitScope const scope, AudioUnitElement const element);
        AudioUnitParameterValue parameter_value(AudioUnitParameterID const parameter_id, AudioUnitScope const scope,
                                                AudioUnitElement const element) const;

        parameter_map_t create_parameters(AudioUnitScope const scope) const;
        parameter create_parameter(AudioUnitParameterID const parameter_id, AudioUnitScope const scope) const;

        void set_element_count(UInt32 const count, AudioUnitScope const scope);  // for mixer
        UInt32 element_count(AudioUnitScope const scope) const;                  // for mixer

        void set_enable_output(bool const enable_output);  // for io
        bool is_enable_output() const;                     // for io
        void set_enable_input(bool const enable_input);    // for io
        bool is_enable_input() const;                      // for io
        bool has_output() const;                           // for io
        bool has_input() const;                            // for io
        bool is_running() const;                           // for io
        void set_channel_map(channel_map_t const &map, AudioUnitScope const scope,
                             AudioUnitElement const element);                                         // for io
        channel_map_t channel_map(AudioUnitScope const scope, AudioUnitElement const element) const;  // for io
        UInt32 channel_map_count(AudioUnitScope const scope, AudioUnitElement const element) const;   // for io
#if (TARGET_OS_MAC && !TARGET_OS_IPHONE)
        void set_current_device(AudioDeviceID const device);  // for io
        AudioDeviceID const current_device() const;           // for io
#endif

        void start();  // for io
        void stop();   // for io
        void reset();

        // render thread

        void callback_render(render_parameters &render_parameters);
        au_result_t audio_unit_render(render_parameters &render_parameters);

       private:
        // from graph

        void _initialize() override;
        void _uninitialize() override;
        void _set_graph_key(std::experimental::optional<UInt8> const &key) override;
        std::experimental::optional<UInt8> const &_graph_key() const override;
        void _set_key(std::experimental::optional<UInt16> const &key) override;
        std::experimental::optional<UInt16> const &_key() const override;

#if YAS_TEST
       public:
        class private_access;
        friend private_access;
#endif
    };
}

audio::unit::au_result_t to_result(OSStatus const err);
}

#include "yas_audio_unit_impl.h"
#include "yas_audio_unit_parameter.h"

#if YAS_TEST
#include "yas_audio_unit_private_access.h"
#endif