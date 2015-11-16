//
//  yas_audio_device_io_node_protocol.h
//  Copyright (c) 2015 Yuki Yasoshima.
//

#pragma once

namespace yas
{
    class audio_device_io;

    class audio_device_io_node_from_engine
    {
       public:
        virtual ~audio_device_io_node_from_engine() = default;

        virtual void _add_device_io() const = 0;
        virtual void _remove_device_io() const = 0;
        virtual audio_device_io &_device_io() const = 0;
    };
}