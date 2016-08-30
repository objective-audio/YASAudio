//
//  yas_audio_route_extension.h
//

#pragma once

#include "yas_audio_route.h"
#include "yas_base.h"

namespace yas {
namespace audio {
    class node;

    class route_extension : public base {
        class kernel;
        class impl;

       public:
        route_extension();
        route_extension(std::nullptr_t);

        virtual ~route_extension() final;

        audio::route_set_t const &routes() const;
        void add_route(audio::route);
        void remove_route(audio::route const &);
        void remove_route_for_source(audio::route::point const &);
        void remove_route_for_destination(audio::route::point const &);
        void set_routes(audio::route_set_t routes);
        void clear_routes();

        audio::node const &node() const;
        audio::node &node();
    };
}
}