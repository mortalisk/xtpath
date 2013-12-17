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
#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_ATTRIBUTE_SELECTOR_HPP
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_ATTRIBUTE_SELECTOR_HPP

#include "context.hpp"
#include "selector_common.hpp"
#include "attribute_iterator.hpp"

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

struct filtered_attribute_name {
    explicit filtered_attribute_name(std::string name): name(std::move(name)) {
    }

    filtered_attribute_name(filtered_attribute_name&& other)
        : name(std::move(other.name)) {
    }

    filtered_attribute_name(filtered_attribute_name const& other)
        : name(other.name) {
    }

    typedef bool return_type;

    bool operator()(std::pair<std::string, std::string> const& pair){
        return pair.first == name;
    }

    std::string name;
};

struct filtered_attribute_name_and_value {
    explicit filtered_attribute_name_and_value(std::string name, std::string value)
        : name(std::move(name)), value(std::move(value)) {
    }

    filtered_attribute_name_and_value(filtered_attribute_name_and_value&& other)
        : name(std::move(other.name)), value(std::move(other.value)) {
    }

    filtered_attribute_name_and_value(filtered_attribute_name_and_value const& other)
        : name(other.name), value(other.value) {
    }

    filtered_attribute_name_and_value& operator=(filtered_attribute_name_and_value const& other) {
        name = other.name;
        value = other.value;
        return *this;
    }

    typedef bool return_type;

    template <typename T, typename U>
    bool operator()(_context<T, U> const& s) const {
        return s.attribute(name) == value;
    }

    std::string name;
    std::string value;
};

class _attribute {
public:
    filtered_attribute_name operator()(std::string name) const {
        return filtered_attribute_name(name);
    }

    filtered_attribute_name_and_value operator()(
            std::string name, std::string value) const {
        return filtered_attribute_name_and_value(name, value);
    }
};

namespace {
    const _attribute attribute;
}

template <typename Range>
auto
operator|(Range range, _attribute)
-> decltype(make_attributes(range))
{
    return make_attributes(range);
}

struct second_of_pair {
    typedef std::string result_type;

    std::string operator()(std::pair<std::string, std::string> const& pair) const{
        return pair.second;
    }
    std::string operator()(std::pair<std::string, std::string>&& pair) const{
        return std::move(pair.second);
    }
};

template <typename Range>
auto
operator|(Range range, filtered_attribute_name f)
-> decltype(make_attributes(range) | boost::adaptors::filtered(f) |
            boost::adaptors::transformed(second_of_pair()))
{
    return make_attributes(range) | boost::adaptors::filtered(f) |
            boost::adaptors::transformed(second_of_pair());
}

template <typename Range>
auto
operator|(Range range, filtered_attribute_name_and_value f)
-> decltype(range | boost::adaptors::filtered(f))
{
    return range | boost::adaptors::filtered(f);
}

}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_ATTRIBUTE_SELECTOR_HPP
