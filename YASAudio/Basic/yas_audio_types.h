//
//  yas_audio_types.h
//  Copyright (c) 2015 Yuki Yasoshima.
//

#pragma once

#include <memory>
#include <functional>
#include <AudioUnit/AUComponent.h>

namespace yas
{
    union render_id {
        void *v;
        struct {
            UInt8 graph;
            UInt16 unit;
        };
    };

    union audio_pointer {
        void *v;
        Float32 *f32;
        Float64 *f64;
        SInt16 *i16;
        SInt32 *i32;
        SInt8 *i8;
        UInt8 *u8;
    };

    enum class pcm_format : UInt32 {
        other = 0,
        float32,
        float64,
        int16,
        fixed824,
    };

    enum class render_type : UInt32 {
        normal = 0,
        input,
        notify,
        unknown,
    };

    struct render_parameters {
        render_type in_render_type;
        AudioUnitRenderActionFlags *io_action_flags;
        const AudioTimeStamp *io_time_stamp;
        UInt32 in_bus_number;
        UInt32 in_number_frames;
        AudioBufferList *io_data;
        render_id render_id;
    };

    class audio_time;
    using audio_time_ptr = std::shared_ptr<audio_time>;
}