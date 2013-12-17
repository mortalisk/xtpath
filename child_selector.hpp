/*
 *   Copyright 2013 Morten Bendiksen (morten.bendiksen@gmail.com)
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */
#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_CHILD_SELECTOR_HPP
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_CHILD_SELECTOR_HPP

#include "selector_common.hpp"
#include "child_iterator.hpp"

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

struct filtered_children {
    explicit filtered_children(std::string s): name(std::move(s)) {
    }

    filtered_children(filtered_children&& other)
        : name(std::move(other.name)) {

    }
    filtered_children(filtered_children const& other)
        : name(other.name) {
    }

    std::string name;
};

class _child {
public:
    filtered_children operator()(std::string name) const {
        return filtered_children(name);
    }
};

namespace {
    const _child child;
}

template <typename Range>
auto
operator|(Range const& range,
          _child const&)
-> decltype(make_children(range))
{
    return make_children(range);
}

template <typename Range>
auto
operator|(Range const& range,
          filtered_children f)
-> decltype(make_children(range)
            | boost::adaptors::filtered(name_predicate<typename Range::iterator::value_type>(std::move(f.name))))
{
    return make_children(range)
            | boost::adaptors::filtered(name_predicate<typename Range::iterator::value_type>(std::move(f.name)));
}


template <>
struct is_expr<_child>: std::true_type {
};

template <>
struct is_expr<filtered_children>: std::true_type {
};


}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_CHILD_SELECTOR_HPP
