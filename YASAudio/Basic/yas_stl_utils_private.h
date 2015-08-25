//
//  yas_stl_utils_private.h
//  Copyright (c) 2015 Yuki Yasoshima.
//

#pragma once

namespace yas
{
    template <typename T, typename U>
    std::experimental::optional<T> min_empty_key(std::map<T, U> &map)
    {
        auto map_size = map.size();

        if (map_size == 0) {
            return 0;
        }

        if (map_size >= std::numeric_limits<T>::max()) {
            return std::experimental::nullopt;
        }

        int next = map.rbegin()->first + 1;
        if (next == map.size()) {
            return next;
        }

        next = 0;
        while (map.count(next) > 0) {
            ++next;
        }
        return next;
    }

    template <typename T>
    auto filter(const std::set<T> source, std::function<bool(const T &)> predicate) -> decltype(source)
    {
        std::set<T> filtered_set;

        for (auto &obj : source) {
            if (predicate(obj)) {
                filtered_set.insert(obj);
            }
        }

        return filtered_set;
    }

    template <typename K, typename T>
    std::map<K, std::shared_ptr<T>> lock_values(const std::map<K, std::weak_ptr<T>> &map)
    {
        std::map<K, std::shared_ptr<T>> unwrapped_map;

        for (auto &pair : map) {
            if (auto shared = pair.second.lock()) {
                unwrapped_map.insert(std::make_pair(pair.first, shared));
            }
        }

        return unwrapped_map;
    }
}