//
//  yas_audio_graph_tap.h
//

#pragma once

#include <audio/yas_audio_graph_node.h>

namespace yas::audio {
struct graph_tap final {
    struct args {
        bool is_input = false;
    };

    void set_render_handler(audio::node_render_f);

    audio::graph_node_ptr const &node() const;

    static graph_tap_ptr make_shared();
    static graph_tap_ptr make_shared(graph_tap::args);

   private:
    graph_node_ptr _node;
    std::optional<audio::node_render_f> _render_handler;
    chaining::observer_pool _pool;

    explicit graph_tap(args &&);

    graph_tap(graph_tap const &) = delete;
    graph_tap(graph_tap &&) = delete;
    graph_tap &operator=(graph_tap const &) = delete;
    graph_tap &operator=(graph_tap &&) = delete;
};
}  // namespace yas::audio
