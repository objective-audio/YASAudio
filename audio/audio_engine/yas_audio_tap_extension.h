//
//  yas_audio_tap_extension.h
//

#pragma once

#include "yas_audio_node.h"

namespace yas {
namespace audio {
    class tap_extension : public base {
       public:
        class kernel;
        class impl;

        struct args {
            bool is_input = false;
        };

        tap_extension();
        tap_extension(args);
        tap_extension(std::nullptr_t);

        virtual ~tap_extension() final;

        void set_render_handler(audio::node::render_f);

        audio::node const &node() const;
        audio::node &node();

        audio::connection input_connection_on_render(uint32_t const bus_idx) const;
        audio::connection output_connection_on_render(uint32_t const bus_idx) const;
        audio::connection_smap input_connections_on_render() const;
        audio::connection_smap output_connections_on_render() const;

#if YAS_TEST
        void render_source(audio::node::render_args args);
#endif
    };
}
}