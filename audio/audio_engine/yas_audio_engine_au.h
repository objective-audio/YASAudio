//
//  yas_audio_au.h
//

#pragma once

#include <unordered_map>
#include "yas_audio_engine_node_protocol.h"
#include "yas_audio_unit.h"
#include "yas_audio_engine_au_protocol.h"
#include "yas_base.h"
#include "yas_observing.h"

namespace yas {
namespace audio {
    class graph;

    namespace engine {
        class node;

        class au : public base {
           public:
            class impl;

            enum class method {
                will_update_connections,
                did_update_connections,
            };

            using subject_t = subject<method, au>;
            using observer_t = observer<method, au>;
            using prepare_unit_f = std::function<void(audio::unit &)>;

            struct args {
                audio::engine::node_args node_args;
                AudioComponentDescription acd;
            };

            au(OSType const type, OSType const sub_type);
            explicit au(AudioComponentDescription const &);
            au(args &&);
            au(std::nullptr_t);

            virtual ~au() final;

            void set_prepare_unit_handler(prepare_unit_f);

            audio::unit unit() const;
            std::unordered_map<AudioUnitScope, std::unordered_map<AudioUnitParameterID, audio::unit::parameter>> const &
            parameters() const;
            std::unordered_map<AudioUnitParameterID, audio::unit::parameter> const &global_parameters() const;
            std::unordered_map<AudioUnitParameterID, audio::unit::parameter> const &input_parameters() const;
            std::unordered_map<AudioUnitParameterID, audio::unit::parameter> const &output_parameters() const;

            uint32_t input_element_count() const;
            uint32_t output_element_count() const;

            void set_global_parameter_value(AudioUnitParameterID const parameter_id, float const value);
            float global_parameter_value(AudioUnitParameterID const parameter_id) const;
            void set_input_parameter_value(AudioUnitParameterID const parameter_id, float const value,
                                           AudioUnitElement const element);
            float input_parameter_value(AudioUnitParameterID const parameter_id, AudioUnitElement const element) const;
            void set_output_parameter_value(AudioUnitParameterID const parameter_id, float const value,
                                            AudioUnitElement const element);
            float output_parameter_value(AudioUnitParameterID const parameter_id, AudioUnitElement const element) const;

            subject_t &subject();

            audio::engine::node const &node() const;
            audio::engine::node &node();

            manageable_au &manageable();

           private:
            manageable_au _manageable = nullptr;
        };
    }
}
}