//
//  yas_audio_mac_io_core.h
//

#pragma once

#include <TargetConditionals.h>

#if (TARGET_OS_MAC && !TARGET_OS_IPHONE)

#include "yas_audio_io_core.h"

namespace yas::audio {
struct mac_io_core final : io_core {
    ~mac_io_core();

    void initialize() override;
    void uninitialize() override;

    void set_render_handler(std::optional<io_render_f>) override;
    void set_maximum_frames_per_slice(uint32_t const) override;

    bool start() override;
    void stop() override;

    [[nodiscard]] pcm_buffer const *input_buffer_on_render() const override;

    [[nodiscard]] static mac_io_core_ptr make_shared(mac_device_ptr const &);

   private:
    mac_device_ptr _device;
    std::optional<AudioDeviceIOProcID> _io_proc_id = std::nullopt;

    io_kernel_ptr _kernel = nullptr;
    time const *_input_time = nullptr;
    std::optional<io_render_f> _render_handler = std::nullopt;
    uint32_t _maximum_frames = 4096;

    bool _is_initialized = false;
    bool _is_started = false;

    mac_io_core(mac_device_ptr const &);

    void _create_io_proc();
    void _destroy_io_proc();
    void _reload_io_proc_if_started();
};
}  // namespace yas::audio

#endif
