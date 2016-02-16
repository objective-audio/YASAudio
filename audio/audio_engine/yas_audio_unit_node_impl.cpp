//
//  yas_audio_unit_node_impl.cpp
//

#include "yas_audio_graph.h"
#include "yas_audio_time.h"
#include "yas_audio_unit.h"
#include "yas_audio_unit_node.h"
#include "yas_audio_unit_parameter.h"

using namespace yas;

class audio::unit_node::impl::core {
   public:
    AudioComponentDescription acd;
    std::unordered_map<AudioUnitScope, unit::parameter_map_t> parameters;
    unit _au;

    core() : acd(), parameters(), _au(nullptr), _mutex() {
    }

    void set_au(audio::unit const &au) {
        std::lock_guard<std::recursive_mutex> lock(_mutex);
        _au = au;
    }

    unit au() const {
        std::lock_guard<std::recursive_mutex> lock(_mutex);
        return _au;
    }

   private:
    mutable std::recursive_mutex _mutex;
};

#pragma mark - impl

audio::unit_node::impl::impl() : node::impl(), _core(std::make_unique<core>()) {
}

audio::unit_node::impl::~impl() = default;

void audio::unit_node::impl::prepare(unit_node const &node, AudioComponentDescription const &acd) {
    _core->acd = acd;

    unit unit(acd);
    _core->set_au(unit);

    _core->parameters.clear();
    _core->parameters.insert(std::make_pair(kAudioUnitScope_Global, unit.create_parameters(kAudioUnitScope_Global)));
    _core->parameters.insert(std::make_pair(kAudioUnitScope_Input, unit.create_parameters(kAudioUnitScope_Input)));
    _core->parameters.insert(std::make_pair(kAudioUnitScope_Output, unit.create_parameters(kAudioUnitScope_Output)));
}

void audio::unit_node::impl::reset() {
    auto unit = au();
    unit.reset();

    auto prev_parameters = std::move(_core->parameters);

    _core->parameters.insert(std::make_pair(kAudioUnitScope_Global, unit.create_parameters(kAudioUnitScope_Global)));
    _core->parameters.insert(std::make_pair(kAudioUnitScope_Input, unit.create_parameters(kAudioUnitScope_Input)));
    _core->parameters.insert(std::make_pair(kAudioUnitScope_Output, unit.create_parameters(kAudioUnitScope_Output)));

    for (AudioUnitScope const scope : {kAudioUnitScope_Global, kAudioUnitScope_Input, kAudioUnitScope_Output}) {
        for (auto const &param_pair : prev_parameters.at(scope)) {
            auto const &parameter_id = param_pair.first;
            auto const &parameter = param_pair.second;
            auto const default_value = parameter.default_value();
            for (auto &value_pair : parameter.values()) {
                auto const &element = value_pair.first;
                unit.set_parameter_value(default_value, parameter_id, scope, element);
            }
        }
    }

    super_class::reset();
}

audio::unit audio::unit_node::impl::au() const {
    return _core->au();
}

std::unordered_map<AudioUnitParameterID, audio::unit::parameter_map_t> const &audio::unit_node::impl::parameters()
    const {
    return _core->parameters;
}

audio::unit::parameter_map_t const &audio::unit_node::impl::global_parameters() const {
    return _core->parameters.at(kAudioUnitScope_Global);
}

audio::unit::parameter_map_t const &audio::unit_node::impl::input_parameters() const {
    return _core->parameters.at(kAudioUnitScope_Input);
}

audio::unit::parameter_map_t const &audio::unit_node::impl::output_parameters() const {
    return _core->parameters.at(kAudioUnitScope_Output);
}

UInt32 audio::unit_node::impl::input_element_count() const {
    return _core->au().element_count(kAudioUnitScope_Input);
}

UInt32 audio::unit_node::impl::output_element_count() const {
    return _core->au().element_count(kAudioUnitScope_Output);
}

void audio::unit_node::impl::set_global_parameter_value(AudioUnitParameterID const parameter_id, Float32 const value) {
    auto &global_parameters = _core->parameters.at(kAudioUnitScope_Global);
    if (global_parameters.count(parameter_id) > 0) {
        auto &parameter = global_parameters.at(parameter_id);
        parameter.set_value(value, 0);
        if (auto &audio_unit = _core->_au) {
            audio_unit.set_parameter_value(value, parameter_id, kAudioUnitScope_Global, 0);
        }
    }
}

Float32 audio::unit_node::impl::global_parameter_value(AudioUnitParameterID const parameter_id) const {
    if (auto &audio_unit = _core->_au) {
        return audio_unit.parameter_value(parameter_id, kAudioUnitScope_Global, 0);
    }
    return 0;
}

void audio::unit_node::impl::set_input_parameter_value(AudioUnitParameterID const parameter_id, Float32 const value,
                                                       AudioUnitElement const element) {
    auto &input_parameters = _core->parameters.at(kAudioUnitScope_Input);
    if (input_parameters.count(parameter_id) > 0) {
        auto &parameter = input_parameters.at(parameter_id);
        parameter.set_value(value, element);
        if (auto &audio_unit = _core->_au) {
            audio_unit.set_parameter_value(value, parameter_id, kAudioUnitScope_Input, element);
        }
    }
}

Float32 audio::unit_node::impl::input_parameter_value(AudioUnitParameterID const parameter_id,
                                                      AudioUnitElement const element) const {
    if (auto &audio_unit = _core->_au) {
        return audio_unit.parameter_value(parameter_id, kAudioUnitScope_Input, element);
    }
    return 0;
}

void audio::unit_node::impl::set_output_parameter_value(AudioUnitParameterID const parameter_id, Float32 const value,
                                                        AudioUnitElement const element) {
    auto &output_parameters = _core->parameters.at(kAudioUnitScope_Output);
    if (output_parameters.count(parameter_id) > 0) {
        auto &parameter = output_parameters.at(parameter_id);
        parameter.set_value(value, element);
        if (auto &audio_unit = _core->_au) {
            audio_unit.set_parameter_value(value, parameter_id, kAudioUnitScope_Output, element);
        }
    }
}

Float32 audio::unit_node::impl::output_parameter_value(AudioUnitParameterID const parameter_id,
                                                       AudioUnitElement const element) const {
    if (auto &audio_unit = _core->_au) {
        return audio_unit.parameter_value(parameter_id, kAudioUnitScope_Output, element);
    }
    return 0;
}

UInt32 audio::unit_node::impl::input_bus_count() const {
    return 1;
}

UInt32 audio::unit_node::impl::output_bus_count() const {
    return 1;
}

void audio::unit_node::impl::update_connections() {
    if (auto audio_unit = _core->au()) {
        auto input_bus_count = input_element_count();
        if (input_bus_count > 0) {
            auto weak_node = to_weak(cast<unit_node>());
            audio_unit.set_render_callback([weak_node](yas::audio::render_parameters &render_parameters) {
                if (auto node = weak_node.lock()) {
                    if (auto kernel = node.impl_ptr<impl>()->kernel_cast()) {
                        if (auto connection = kernel->input_connection(render_parameters.in_bus_number)) {
                            if (auto source_node = connection.source_node()) {
                                pcm_buffer buffer{connection.format(), render_parameters.io_data};
                                time when(*render_parameters.io_time_stamp, connection.format().sample_rate());
                                source_node.render(buffer, connection.source_bus(), when);
                            }
                        }
                    }
                }
            });

            for (UInt32 bus_idx = 0; bus_idx < input_bus_count; ++bus_idx) {
                if (auto connection = input_connection(bus_idx)) {
                    audio_unit.set_input_format(connection.format().stream_description(), bus_idx);
                    audio_unit.attach_render_callback(bus_idx);
                } else {
                    audio_unit.detach_render_callback(bus_idx);
                }
            }
        } else {
            audio_unit.set_render_callback(nullptr);
        }

        auto output_bus_count = output_element_count();
        if (output_bus_count > 0) {
            for (UInt32 bus_idx = 0; bus_idx < output_bus_count; ++bus_idx) {
                if (auto connection = output_connection(bus_idx)) {
                    audio_unit.set_output_format(connection.format().stream_description(), bus_idx);
                }
            }
        }
    }
}

void audio::unit_node::impl::prepare_audio_unit() {
    if (auto &audio_unit = _core->_au) {
        audio_unit.set_maximum_frames_per_slice(4096);
    }
}

void audio::unit_node::impl::prepare_parameters() {
    if (auto audio_unit = _core->_au) {
        for (auto &parameters_pair : _core->parameters) {
            auto &scope = parameters_pair.first;
            for (auto &parameter_pair : _core->parameters.at(scope)) {
                auto &parameter = parameter_pair.second;
                for (auto &value_pair : parameter.values()) {
                    auto &element = value_pair.first;
                    auto &value = value_pair.second;
                    audio_unit.set_parameter_value(value, parameter.parameter_id(), scope, element);
                }
            }
        }
    }
}

void audio::unit_node::impl::reload_audio_unit() {
    _core->set_au(unit(_core->acd));
}

void audio::unit_node::impl::render(pcm_buffer &buffer, UInt32 const bus_idx, time const &when) {
    super_class::render(buffer, bus_idx, when);

    if (auto audio_unit = _core->au()) {
        AudioUnitRenderActionFlags action_flags = 0;
        AudioTimeStamp const time_stamp = when.audio_time_stamp();

        render_parameters render_parameters{.in_render_type = render_type::normal,
                                            .io_action_flags = &action_flags,
                                            .io_time_stamp = &time_stamp,
                                            .in_bus_number = bus_idx,
                                            .in_number_frames = buffer.frame_length(),
                                            .io_data = buffer.audio_buffer_list()};

        if (auto err = audio_unit.audio_unit_render(render_parameters).error_opt()) {
            std::cout << "audio unit render error : " << std::to_string(*err) << " - " << yas::to_string(*err)
                      << std::endl;
        }
    }
}
