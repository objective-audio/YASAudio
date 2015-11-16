//
//  yas_audio_connection.h
//  Copyright (c) 2015 Yuki Yasoshima.
//

#pragma once

#include "yas_audio_connection_protocol.h"
#include "yas_audio_format.h"
#include "yas_base.h"
#include <memory>

namespace yas
{
    class audio_node;

    class audio_connection : public base, public audio_connection_from_engine
    {
        using super_class = base;
        class impl;

       public:
        audio_connection(std::nullptr_t);
        ~audio_connection();

        audio_connection(const audio_connection &) = default;
        audio_connection(audio_connection &&) = default;
        audio_connection &operator=(const audio_connection &) = default;
        audio_connection &operator=(audio_connection &&) = default;

        UInt32 source_bus() const;
        UInt32 destination_bus() const;
        audio_node source_node() const;
        audio_node destination_node() const;
        audio_format &format() const;

       protected:
        audio_connection(audio_node &source_node, const UInt32 source_bus, audio_node &destination_node,
                         const UInt32 destination_bus, const audio_format &format);

        void _remove_nodes() const override;
        void _remove_source_node() const override;
        void _remove_destination_node() const override;
#if YAS_TEST
       public:
        class private_access;
        friend private_access;
#endif
    };
}

template <>
struct std::hash<yas::audio_connection> {
    std::size_t operator()(yas::audio_connection const &key) const
    {
        return std::hash<uintptr_t>()(key.identifier());
    }
};

#if YAS_TEST
#include "yas_audio_connection_private_access.h"
#endif