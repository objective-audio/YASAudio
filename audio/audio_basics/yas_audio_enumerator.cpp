//
//  yas_audio_enumerator.cpp
//

#include <string>
#include "yas_audio_enumerator.h"
#include "yas_audio_format.h"
#include "yas_audio_pcm_buffer.h"

using namespace yas;

#pragma mark - enumerator

audio::enumerator::enumerator(flex_ptr const &pointer, UInt32 const byte_stride, UInt32 const length)
    : _pointer(pointer), _top_pointer(pointer), _byte_stride(byte_stride), _length(length), _index(0) {
    if (!pointer.v || byte_stride == 0 || length == 0) {
        throw std::invalid_argument(std::string(__PRETTY_FUNCTION__) + " : invalid argument.");
    }
}

audio::enumerator::enumerator(pcm_buffer const &buffer, UInt32 const channel)
    : enumerator(buffer.flex_ptr_at_channel(channel), buffer.format().buffer_frame_byte_count(),
                 buffer.frame_length()) {
}

const flex_ptr *audio::enumerator::pointer() const {
    return &_pointer;
}

const UInt32 *audio::enumerator::index() const {
    return &_index;
}

UInt32 audio::enumerator::length() const {
    return _length;
}

void audio::enumerator::move() {
    yas_audio_enumerator_move(*this);
}

void audio::enumerator::stop() {
    yas_audio_enumerator_stop(*this);
}

void audio::enumerator::set_position(UInt32 const index) {
    if (index >= _length) {
        throw std::out_of_range(std::string(__PRETTY_FUNCTION__) + " : out of range. position(" +
                                std::to_string(index) + ") length(" + std::to_string(_length) + ")");
        return;
    }
    _index = index;
    _pointer.v = _top_pointer.u8 + (_byte_stride * index);
}

void audio::enumerator::reset() {
    yas_audio_enumerator_reset(*this);
}

audio::enumerator &audio::enumerator::operator++() {
    yas_audio_enumerator_move(*this);
    return *this;
}

#pragma mark - frame enumerator

audio::frame_enumerator::frame_enumerator(pcm_buffer const &buffer)
    : _frame(0),
      _channel(0),
      _frame_length(buffer.frame_length()),
      _channel_count(buffer.format().channel_count()),
      _frame_byte_stride(buffer.format().buffer_frame_byte_count()),
      _pointers(std::vector<flex_ptr>(buffer.format().channel_count())),
      _top_pointers(std::vector<flex_ptr>(buffer.format().channel_count())),
      _pointers_size(buffer.format().channel_count() * sizeof(flex_ptr *)),
      _pointer(nullptr) {
    auto const &format = buffer.format();
    UInt32 const bufferCount = format.buffer_count();
    UInt32 const stride = format.stride();
    UInt32 const sampleByteCount = format.sample_byte_count();

    UInt32 channel = 0;
    for (UInt32 buf_idx = 0; buf_idx < bufferCount; buf_idx++) {
        flex_ptr pointer = buffer.flex_ptr_at_index(buf_idx);
        for (UInt32 ch_idx = 0; ch_idx < stride; ch_idx++) {
            _pointers[channel].v = _top_pointers[channel].v = pointer.v;
            pointer.u8 += sampleByteCount;
            channel++;
        }
    }

    _pointer.v = _pointers[0].v;
}

const flex_ptr *audio::frame_enumerator::pointer() const {
    return &_pointer;
}

const UInt32 *audio::frame_enumerator::frame() const {
    return &_frame;
}

const UInt32 *audio::frame_enumerator::channel() const {
    return &_channel;
}

UInt32 audio::frame_enumerator::frame_length() const {
    return _frame_length;
}

UInt32 audio::frame_enumerator::channel_count() const {
    return _channel_count;
}

void audio::frame_enumerator::move_frame() {
    yas_audio_frame_enumerator_move_frame(*this);
}

void audio::frame_enumerator::move_channel() {
    yas_audio_frame_enumerator_move_channel(*this);
}

void audio::frame_enumerator::move() {
    yas_audio_frame_enumerator_move(*this);
}

void audio::frame_enumerator::stop() {
    yas_audio_frame_enumerator_stop(*this);
}

void audio::frame_enumerator::set_frame_position(UInt32 const frame) {
    if (frame >= _frame_length) {
        throw std::out_of_range(std::string(__PRETTY_FUNCTION__) + " : out of range. frame(" + std::to_string(frame) +
                                ")");
    }

    _frame = frame;

    UInt32 const stride = _frame_byte_stride * frame;
    UInt32 index = _channel_count;
    while (index--) {
        _pointers[index].v = _top_pointers[index].u8 + stride;
    }

    if (_pointer.v) {
        _pointer.v = _pointers[_channel].v;
    }
}

void audio::frame_enumerator::set_channel_position(UInt32 const channel) {
    if (channel >= _channel_count) {
        throw std::out_of_range(std::string(__PRETTY_FUNCTION__) + " : out of range. channel(" +
                                std::to_string(channel) + ") count(" + std::to_string(_channel_count) + ")");
    }

    _channel = channel;
    _pointer.v = _pointers[_channel].v;
}

void audio::frame_enumerator::reset() {
    yas_audio_frame_enumerator_reset(*this);
}

audio::frame_enumerator &audio::frame_enumerator::operator++() {
    yas_audio_frame_enumerator_move(*this);
    return *this;
}