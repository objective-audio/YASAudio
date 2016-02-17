//
//  yas_audio_node_protocol.h
//

#pragma once

#include "yas_audio_connection_protocol.h"

namespace yas {
namespace audio {
    class engine;

    class node_from_engine {
       public:
        virtual ~node_from_engine() = default;

        virtual audio::connection _input_connection(UInt32 const bus_idx) const = 0;
        virtual audio::connection _output_connection(UInt32 const bus_idx) const = 0;
        virtual audio::connection_wmap const &_input_connections() const = 0;
        virtual audio::connection_wmap const &_output_connections() const = 0;
        virtual void _add_connection(audio::connection const &connection) = 0;
        virtual void _remove_connection(audio::connection const &connection) = 0;
        virtual void _set_engine(audio::engine const &engine) = 0;
        virtual audio::engine _engine() const = 0;
        virtual void _update_kernel() = 0;
        virtual void _update_connections() = 0;
    };

    class node_from_connection {
       public:
        virtual ~node_from_connection() = default;

        virtual void _add_connection(audio::connection const &connection) = 0;
        virtual void _remove_connection(audio::connection const &connection) = 0;
    };
}
}