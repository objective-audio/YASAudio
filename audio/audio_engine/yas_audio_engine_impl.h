//
//  yas_audio_engine_impl.h
//

#pragma once

#include <unordered_set>
#include "yas_objc_container.h"
#if (TARGET_OS_MAC && !TARGET_OS_IPHONE)
#include "yas_audio_device.h"
#endif

class yas::audio::engine::impl : public yas::base::impl {
   public:
    impl();
    virtual ~impl();

    void prepare(engine const &);

    weak<engine> &weak_engine() const;
    objc::container<> &reset_observer() const;
    objc::container<> &route_change_observer() const;
    yas::subject<engine> &subject() const;
#if (TARGET_OS_MAC && !TARGET_OS_IPHONE)
    observer<audio::device::change_info> &device_observer();
#endif

    bool node_exists(node const &node);

    void attach_node(node &node);
    void detach_node(node &node);
    void detach_node_if_unused(node &node);

    bool prepare();

    audio::connection connect(node &source_node, node &destination_node, UInt32 const source_bus_idx,
                              UInt32 const destination_bus_idx, audio::format const &format);
    void disconnect(audio::connection &connection);
    void disconnect(node &node);
    void disconnect_node_with_predicate(std::function<bool(audio::connection const &)> predicate);

    void add_node_to_graph(node const &node);
    void remove_node_from_graph(node const &node);

    bool add_connection(audio::connection const &connection);
    void remove_connection_from_nodes(audio::connection const &connection);
    void update_node_connections(node &node);
    void update_all_node_connections();

    audio::connection_set input_connections_for_destination_node(node const &node) const;
    audio::connection_set output_connections_for_source_node(node const &node) const;

    void set_graph(yas::audio::graph const &graph);
    yas::audio::graph graph() const;
    void reload_graph();

    std::unordered_set<node> &nodes() const;
    audio::connection_set &connections() const;
    audio::offline_output_node &offline_output_node() const;

    engine::start_result_t start_render();
    engine::start_result_t start_offline_render(offline_render_f, offline_completion_f);
    void stop();

    void post_configuration_change() const;

   private:
    class core;
    std::unique_ptr<core> _core;
};
