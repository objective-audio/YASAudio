//
//  yas_audio_io_device.h
//

#pragma once

#include "yas_audio_io_core.h"

namespace yas::audio {
struct io_device {
    enum class method { lost, updated };

    [[nodiscard]] virtual std::optional<audio::format> input_format() const = 0;
    [[nodiscard]] virtual std::optional<audio::format> output_format() const = 0;

    [[nodiscard]] virtual io_core_ptr make_io_core() const = 0;

    [[nodiscard]] virtual chaining::chain_unsync_t<io_device::method> io_device_chain() = 0;

    [[nodiscard]] uint32_t input_channel_count() const;
    [[nodiscard]] uint32_t output_channel_count() const;

    static std::optional<io_device_ptr> default_device();
};
}  // namespace yas::audio
