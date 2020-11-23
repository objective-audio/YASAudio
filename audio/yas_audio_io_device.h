//
//  yas_audio_io_device.h
//

#pragma once

#include <audio/yas_audio_interruptor.h>
#include <audio/yas_audio_io_core.h>

namespace yas::audio {
struct io_device {
    enum class method { lost, updated };

    [[nodiscard]] virtual std::optional<audio::format> input_format() const = 0;
    [[nodiscard]] virtual std::optional<audio::format> output_format() const = 0;

    [[nodiscard]] virtual io_core_ptr make_io_core() const = 0;

    [[nodiscard]] virtual std::optional<interruptor_ptr> const &interruptor() const = 0;

    [[nodiscard]] virtual chaining::chain_unsync_t<io_device::method> io_device_chain() = 0;

    [[nodiscard]] uint32_t input_channel_count() const;
    [[nodiscard]] uint32_t output_channel_count() const;

    [[nodiscard]] bool is_interrupting() const;
    [[nodiscard]] std::optional<chaining::chain_unsync_t<interruption_method>> interruption_chain() const;

    [[nodiscard]] static io_device_ptr cast(io_device_ptr const &device) {
        return device;
    }
};
}  // namespace yas::audio