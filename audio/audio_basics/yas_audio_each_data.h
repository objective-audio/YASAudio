//
//  yas_audio_each_data.h
//

#pragma once

#include "yas_each_data.h"

namespace yas::audio {
class pcm_buffer;

template <typename T>
each_data<T> make_each_data(pcm_buffer &buffer);

template <typename T>
const_each_data<T> make_each_data(pcm_buffer const &buffer);
}  // namespace yas::audio

#include "yas_audio_each_data_private.h"
