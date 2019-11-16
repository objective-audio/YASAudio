//
//  yas_audio_engine_node.h
//

#pragma once

#include <chaining/yas_chaining_umbrella.h>
#include <optional>
#include <ostream>
#include "yas_audio_engine_connection.h"
#include "yas_audio_engine_node_protocol.h"
#include "yas_audio_engine_ptr.h"
#include "yas_audio_format.h"
#include "yas_audio_pcm_buffer.h"
#include "yas_audio_types.h"

namespace yas {
template <typename T, typename U>
class result;
}

namespace yas::audio::engine {
struct node : connectable_node, manageable_node {
    enum class method {
        will_reset,
        update_connections,
    };

    using chaining_pair_t = std::pair<method, node_ptr>;

    struct render_args {
        audio::pcm_buffer_ptr const &buffer;
        uint32_t const bus_idx;
        audio::time const &when;
    };

    using prepare_kernel_f = std::function<void(kernel &)>;
    using render_f = std::function<void(render_args)>;

    virtual ~node();

    void reset();

    connection_ptr input_connection(uint32_t const bus_idx) const override;
    connection_ptr output_connection(uint32_t const bus_idx) const override;
    connection_wmap const &input_connections() const override;
    connection_wmap const &output_connections() const override;

    std::optional<audio::format> input_format(uint32_t const bus_idx) const;
    std::optional<audio::format> output_format(uint32_t const bus_idx) const;
    bus_result_t next_available_input_bus() const;
    bus_result_t next_available_output_bus() const;
    bool is_available_input_bus(uint32_t const bus_idx) const;
    bool is_available_output_bus(uint32_t const bus_idx) const;
    audio::engine::manager_ptr manager() const override;
    std::optional<audio::time> last_render_time() const;

    uint32_t input_bus_count() const;
    uint32_t output_bus_count() const;
    bool is_input_renderable() const;

    void set_prepare_kernel_handler(prepare_kernel_f);
    void set_render_handler(render_f);

    std::optional<kernel_ptr> kernel() const;

    void render(render_args);
    void set_render_time_on_render(audio::time const &time);

    [[nodiscard]] chaining::chain_unsync_t<chaining_pair_t> chain() const;
    [[nodiscard]] chaining::chain_relayed_unsync_t<node_ptr, chaining_pair_t> chain(method const) const;

   private:
    std::weak_ptr<node> _weak_node;
    std::weak_ptr<audio::engine::manager> _weak_manager;
    uint32_t _input_bus_count = 0;
    uint32_t _output_bus_count = 0;
    bool _is_input_renderable = false;
    std::optional<uint32_t> _override_output_bus_idx = std::nullopt;
    audio::engine::connection_wmap _input_connections;
    audio::engine::connection_wmap _output_connections;
    graph_editing_f _add_to_graph_handler;
    graph_editing_f _remove_from_graph_handler;
    prepare_kernel_f _prepare_kernel_handler;
    audio::engine::node::render_f _render_handler;
    chaining::notifier_ptr<chaining_pair_t> _notifier = chaining::notifier<chaining_pair_t>::make_shared();

    struct core;
    std::unique_ptr<core> _core;

    explicit node(node_args &&);

    void _prepare(node_ptr const &);
    void _prepare_kernel(kernel_ptr const &kernel);

    void add_connection(audio::engine::connection_ptr const &) override;
    void remove_input_connection(uint32_t const dst_bus) override;
    void remove_output_connection(uint32_t const src_bus) override;

    void set_manager(audio::engine::manager_wptr const &) override;
    void update_kernel() override;
    void update_connections() override;
    void set_add_to_graph_handler(graph_editing_f &&) override;
    void set_remove_from_graph_handler(graph_editing_f &&) override;
    graph_editing_f const &add_to_graph_handler() const override;
    graph_editing_f const &remove_from_graph_handler() const override;

    node(node &&) = delete;
    node &operator=(node &&) = delete;
    node(node const &) = delete;
    node &operator=(node const &) = delete;

   public:
    static node_ptr make_shared(node_args);
};
}  // namespace yas::audio::engine

namespace yas {
std::string to_string(audio::engine::node::method const &);
}

std::ostream &operator<<(std::ostream &, yas::audio::engine::node::method const &);

#include "yas_audio_engine_kernel.h"
