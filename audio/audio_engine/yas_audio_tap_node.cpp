//
//  yas_audio_tap_node.cpp
//

#include "yas_audio_tap_node.h"

using namespace yas;

audio::tap_node::tap_node() : base(std::make_unique<impl>()) {
    impl_ptr<impl>()->prepare(*this);
}

audio::tap_node::tap_node(std::nullptr_t) : base(nullptr) {
}

audio::tap_node::tap_node(std::shared_ptr<impl> const &imp) : base(imp) {
    impl_ptr<impl>()->prepare(*this);
}

audio::tap_node::~tap_node() = default;

void audio::tap_node::set_render_function(render_f func) {
    impl_ptr<impl>()->set_render_function(std::move(func));
}

audio::node const &audio::tap_node::node() const {
    return impl_ptr<impl>()->node();
}

audio::node &audio::tap_node::node() {
    return impl_ptr<impl>()->node();
}

audio::connection audio::tap_node::input_connection_on_render(uint32_t const bus_idx) const {
    return impl_ptr<impl>()->input_connection_on_render(bus_idx);
}

audio::connection audio::tap_node::output_connection_on_render(uint32_t const bus_idx) const {
    return impl_ptr<impl>()->output_connection_on_render(bus_idx);
}

audio::connection_smap audio::tap_node::input_connections_on_render() const {
    return impl_ptr<impl>()->input_connections_on_render();
}

audio::connection_smap audio::tap_node::output_connections_on_render() const {
    return impl_ptr<impl>()->output_connections_on_render();
}

void audio::tap_node::render_source(pcm_buffer &buffer, uint32_t const bus_idx, time const &when) {
    impl_ptr<impl>()->render_source(buffer, bus_idx, when);
}

#pragma mark - input_tap_node

audio::input_tap_node::input_tap_node() : tap_node(std::make_unique<impl>()) {
    audio::tap_node::impl_ptr<impl>()->prepare(*this);
}

audio::input_tap_node::input_tap_node(std::nullptr_t) : tap_node(nullptr) {
}
