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
#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_DESCENDANT_SELECTOR_HPP
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_DESCENDANT_SELECTOR_HPP


#include "selector_common.hpp"
#include "descendant_iterator.hpp"

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

struct filtered_descendant {
    explicit filtered_descendant(std::string s): name(std::move(s)) {
    }

    filtered_descendant(filtered_descendant&& other)
        : name(std::move(other.name)) {

    }
    filtered_descendant(filtered_descendant const& other)
        : name(other.name) {
    }

    std::string name;
};

class _descendant {
public:
    filtered_descendant operator()(std::string name) const {
        return filtered_descendant(name);
    }
};

namespace {
    const _descendant descendant;
}

template <typename Range>
auto
operator|(Range const& range,
          _descendant const&)
-> decltype(make_descendant(range))
{
    return make_descendant(range);
}

template <typename Range>
auto
operator|(Range const& range,
          filtered_descendant f)

-> decltype(make_descendant(range)
            | boost::adaptors::filtered(name_predicate<typename Range::iterator::reference>(std::move(f.name))))
{
    return make_descendant(range)
            | boost::adaptors::filtered(name_predicate<typename Range::iterator::reference>(std::move(f.name)));
}


template <>
struct is_expr<_descendant>: std::true_type {
};

template <>
struct is_expr<filtered_descendant>: std::true_type {
};



}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_DESCENDANT_SELECTOR_HPP
