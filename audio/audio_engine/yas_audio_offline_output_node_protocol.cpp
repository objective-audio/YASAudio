//
//  yas_audio_offline_output_node_protocol.cpp
//

#include "yas_audio_offline_output_node_protocol.h"

using namespace yas;

audio::manageable_offline_output_unit::manageable_offline_output_unit(std::shared_ptr<impl> impl)
    : protocol(std::move(impl)) {
}

audio::manageable_offline_output_unit::manageable_offline_output_unit(std::nullptr_t) : protocol(nullptr) {
}

audio::offline_start_result_t audio::manageable_offline_output_unit::start(offline_render_f &&render_func,
                                                                           offline_completion_f &&completion_func) {
    return impl_ptr<impl>()->start(std::move(render_func), std::move(completion_func));
}

void audio::manageable_offline_output_unit::stop() {
    impl_ptr<impl>()->stop();
}
