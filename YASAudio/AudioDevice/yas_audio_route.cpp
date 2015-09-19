//
//  yas_audio_route.cpp
//  Copyright (c) 2015 Yuki Yasoshima.
//

#include "yas_audio_route.h"
#include "yas_audio_format.h"
#include "yas_cf_utils.h"
#include <exception>

using namespace yas;

audio_route::point::point(const UInt32 bus_idx, const UInt32 ch_idx) : bus(bus_idx), channel(ch_idx)
{
}

bool audio_route::point::operator==(const point &other) const
{
    return bus == other.bus && channel == other.channel;
}

bool audio_route::point::operator!=(const point &other) const
{
    return bus != other.bus || channel != other.channel;
}

audio_route::audio_route(const UInt32 src_bus_idx, const UInt32 src_ch_idx, const UInt32 dst_bus_idx,
                         const UInt32 dst_ch_idx)
    : source(src_bus_idx, src_ch_idx), destination(dst_bus_idx, dst_ch_idx)
{
}

audio_route::audio_route(const UInt32 bus_idx, const UInt32 ch_idx)
    : source(bus_idx, ch_idx), destination(bus_idx, ch_idx)
{
}

audio_route::audio_route(const point &src_point, const point &dst_point) : source(src_point), destination(dst_point)
{
}

bool audio_route::operator==(const audio_route &other) const
{
    return source == other.source && destination == other.destination;
}

bool audio_route::operator!=(const audio_route &other) const
{
    return source != other.source || destination != other.destination;
}

bool audio_route::operator<(const audio_route &other) const
{
    if (source.bus != other.source.bus) {
        return source.bus < other.source.bus;
    }

    if (destination.bus != other.destination.bus) {
        return destination.bus < other.destination.bus;
    }

    if (source.channel != other.source.channel) {
        return source.channel < other.source.channel;
    }

    return destination.channel < other.destination.channel;
}

#pragma mark -

channel_map_result yas::channel_map_from_routes(const audio_route_set &routes, const UInt32 src_bus_idx,
                                                const UInt32 src_ch_count, const UInt32 dst_bus_idx,
                                                const UInt32 dst_ch_count)
{
    channel_map_t channel_map(src_ch_count, -1);
    bool exists = false;

    for (const auto &route : routes) {
        if (route.source.bus == src_bus_idx && route.destination.bus == dst_bus_idx &&
            route.source.channel < src_ch_count && route.destination.channel < dst_ch_count) {
            channel_map.at(route.source.channel) = route.destination.channel;
            exists = true;
        }
    }

    if (exists) {
        return channel_map_result(std::move(channel_map));
    }

    return channel_map_result(nullptr);
}
