//
//  yas_audio_connection_protocol.cpp
//

#include "yas_audio_connection_protocol.h"

using namespace yas;

audio::engine::node_removable::node_removable(std::shared_ptr<impl> impl) : protocol(impl) {
}

audio::engine::node_removable::node_removable(std::nullptr_t) : protocol(nullptr) {
}

void audio::engine::node_removable::remove_nodes() {
    impl_ptr<impl>()->remove_nodes();
}

void audio::engine::node_removable::remove_source_node() {
    impl_ptr<impl>()->remove_source_node();
}

void audio::engine::node_removable::remove_destination_node() {
    impl_ptr<impl>()->remove_destination_node();
}
