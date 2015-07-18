//
//  yas_audio_device_stream.h
//  Copyright (c) 2015 Yuki Yasoshima.
//

#pragma once

#include <TargetConditionals.h>
#if (TARGET_OS_MAC && !TARGET_OS_IPHONE)

#include "yas_audio_types.h"
#include "yas_observing.h"
#include "yas_audio_format.h"
#include <AudioToolbox/AudioToolbox.h>
#include <memory>
#include <vector>
#include <set>

namespace yas
{
    class audio_device_stream;
    class audio_device;
    using audio_device_stream_ptr = std::shared_ptr<audio_device_stream>;
    using audio_device_ptr = std::shared_ptr<audio_device>;

    class audio_device_stream
    {
       public:
        enum class method : UInt32 {
            stream_did_change,
        };

        enum class property : UInt32 {
            virtual_format = 0,
            is_active,
            starting_channel,
        };

        enum class direction {
            output = 0,
            input = 1,
        };

        class property_info
        {
           public:
            const AudioObjectID object_id;
            const audio_device_stream::property property;
            const AudioObjectPropertyAddress address;

            property_info(const audio_device_stream::property property, const AudioObjectID object_id,
                          const AudioObjectPropertyAddress &address);

            bool operator<(const property_info &info) const;
        };

        static audio_device_stream_ptr create(const AudioStreamID, const AudioDeviceID);

        ~audio_device_stream();

        bool operator==(const audio_device_stream &);
        bool operator!=(const audio_device_stream &);

        AudioStreamID stream_id() const;
        audio_device_ptr device() const;
        bool is_active() const;
        direction direction() const;
        audio_format_ptr virtual_format() const;
        UInt32 starting_channel() const;

        using property_subject = subject<method, std::set<property_info>>;
        property_subject &subject() const;

       private:
        class impl;
        std::unique_ptr<impl> _impl;

        audio_device_stream(const AudioStreamID, const AudioDeviceID);

        audio_device_stream(const audio_device_stream &) = delete;
        audio_device_stream(audio_device_stream &&) = delete;
        audio_device_stream &operator=(const audio_device_stream &) = delete;
        audio_device_stream &operator=(audio_device_stream &&) = delete;

        template <typename T>
        std::unique_ptr<std::vector<T>> property_data(const AudioStreamID stream_id,
                                                      const AudioObjectPropertySelector selector) const;
    };
}

#include "yas_audio_device_stream_private.h"

#endif