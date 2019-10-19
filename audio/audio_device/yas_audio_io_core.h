//
//  yas_audio_io_core.h
//

#pragma once

#include <chaining/yas_chaining_umbrella.h>
#include "yas_audio_io_kernel.h"

namespace yas::audio {
struct io_core {
    enum class method { lost, updated };

    virtual ~io_core() = default;

    virtual void initialize() = 0;
    virtual void uninitialize() = 0;

    virtual void set_render_handler(io_render_f) = 0;
    virtual void set_maximum_frames_per_slice(uint32_t const) = 0;

    virtual bool start() = 0;
    virtual void stop() = 0;

    [[nodiscard]] virtual std::optional<pcm_buffer_ptr> const &input_buffer_on_render() const = 0;
    [[nodiscard]] virtual std::optional<time_ptr> const &input_time_on_render() const = 0;

    [[nodiscard]] virtual chaining::chain_unsync_t<io_core::method> chain() = 0;
};

using io_core_ptr = std::shared_ptr<io_core>;
}  // namespace yas::audio
