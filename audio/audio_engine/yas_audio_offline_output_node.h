//
//  yas_audio_offline_output_node.h
//

#pragma once

#include "yas_audio_offline_output_node_protocol.h"
#include "yas_base.h"

namespace yas {
namespace audio {
    class node;

    class offline_output_node : public base {
       public:
        class impl;

        offline_output_node();
        offline_output_node(std::nullptr_t);

        virtual ~offline_output_node() final;

        bool is_running() const;

        audio::node const &node() const;
        audio::node &node();

        manageable_offline_output_unit &manageable();

       private:
        offline_output_node(std::shared_ptr<impl> const &);

        manageable_offline_output_unit _manageable = nullptr;
    };
}

std::string to_string(audio::offline_start_error_t const &error);
}

std::ostream &operator<<(std::ostream &, yas::audio::offline_start_error_t const &);