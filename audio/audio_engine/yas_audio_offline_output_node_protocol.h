//
//  yas_audio_offline_output_node_protocol.h
//

#pragma once

#include <functional>
#include "yas_protocol.h"
#include "yas_result.h"

namespace yas {
namespace audio {
    class pcm_buffer;
    class time;

    enum class offline_start_error_t {
        already_running,
        prepare_failure,
        connection_not_found,
    };

    using offline_render_f = std::function<void(audio::pcm_buffer &buffer, audio::time const &when, bool &out_stop)>;
    using offline_completion_f = std::function<void(bool const cancelled)>;
    using offline_start_result_t = result<std::nullptr_t, offline_start_error_t>;

    struct manageable_offline_output_unit : protocol {
        struct impl : protocol::impl {
            virtual offline_start_result_t start(offline_render_f &&, offline_completion_f &&) = 0;
            virtual void stop() = 0;
        };

        explicit manageable_offline_output_unit(std::shared_ptr<impl> impl) : protocol(impl) {
        }

        offline_start_result_t start(offline_render_f &&callback_func, offline_completion_f &&completion_func) {
            return impl_ptr<impl>()->start(std::move(callback_func), std::move(completion_func));
        }

        void stop() {
            impl_ptr<impl>()->stop();
        }
    };
}
}