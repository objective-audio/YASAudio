//
//  yas_audio_unit_testable.h
//

#pragma once

#if YAS_TEST

namespace yas {
namespace audio {
    struct testable_unit {
        template <typename T>
        static void set_property_data(unit const &unit, std::vector<T> const &data,
                                      AudioUnitPropertyID const property_id, AudioUnitScope const scope,
                                      AudioUnitElement const element) {
            unit.impl_ptr<audio::unit::impl>()->set_property_data(data, property_id, scope, element);
        }

        template <typename T>
        static std::vector<T> property_data(unit const &unit, AudioUnitPropertyID const property_id,
                                            AudioUnitScope const scope, AudioUnitElement const element) {
            return unit.impl_ptr<audio::unit::impl>()->property_data<T>(property_id, scope, element);
        }
    };
}
}

#endif
