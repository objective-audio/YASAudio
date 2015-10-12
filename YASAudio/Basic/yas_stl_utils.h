//
//  yas_stl_utils.h
//  Copyright (c) 2015 Yuki Yasoshima.
//

#pragma once

#include <map>
#include <set>
#include <memory>
#include <experimental/optional>

namespace yas
{
    template <typename T, typename U>
    std::experimental::optional<T> min_empty_key(const std::map<T, U> &map);

    template <typename T, typename P>
    T filter(const T &collection, P predicate);

    template <typename K, typename T>
    std::map<K, std::shared_ptr<T>> lock_values(const std::map<K, std::weak_ptr<T>> &map);

    template <typename T, typename P>
    void erase_if(T &collection, P predicate);

    template <typename T, typename F>
    void enumerate(T &collection, F function);
}

#include "yas_stl_utils_private.h"
