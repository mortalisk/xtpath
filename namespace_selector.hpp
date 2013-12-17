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
#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_NAMESPACE_SELECTOR_HPP
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_NAMESPACE_SELECTOR_HPP

#include "scopedmap.hpp"
#include <boost/range/adaptor/transformed.hpp>

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

struct filtered_namespace {
    explicit filtered_namespace(std::string s): ns(std::move(s)) {
    }

    filtered_namespace(filtered_namespace&& other)
        : ns(std::move(other.ns)) {

    }
    filtered_namespace(filtered_namespace const& other)
        : ns(other.ns) {
    }

    std::string ns;
};

struct _namespace {
    filtered_namespace operator()(std::string ns) const {
        return filtered_namespace(ns);
    }
};

namespace {
    const _namespace ns;
}


template<typename Element>
struct node_to_namespace {
    typedef std::string result_type;
     std::string operator()(Element element) const {
        std::string name = element.name();
        std::string prefix("");
        std::string::size_type colonPosition = name.find(':');
        if (colonPosition != std::string::npos) {
            prefix = name.substr(0,colonPosition);
        }
        auto ns = element.ns().get(prefix);
        if (ns)
            return *ns;
        else
            return "";
    }
};

// A predicate for filtering on node namespace
class namespace_predicate {
public:
    bool operator()(std::string i) const {
       return i == name;
    }

    namespace_predicate(namespace_predicate&& other)
        : name(std::move(other.name)) {
    }

    namespace_predicate(namespace_predicate const& other)
        : name(other.name) {
    }

    explicit namespace_predicate(std::string&& name)
        : name(std::move(name)) {
    }

private:
    std::string name;

};

template <>
struct is_expr<_namespace>: std::true_type {
};

template <>
struct is_expr<filtered_namespace>: std::true_type {
};

template<typename Range>
auto
operator|(Range const& range, _namespace const&)
-> decltype(range |
            boost::adaptors::transformed(node_to_namespace<typename Range::iterator::reference>()))
{
    return range |
            boost::adaptors::transformed(node_to_namespace<typename Range::iterator::reference>());
}

template<typename Range>
auto
operator|(Range const& range, filtered_namespace f)
-> decltype(range | ns |
            boost::adaptors::filtered(namespace_predicate(std::move(f.ns))))
{
    return range | ns |
            boost::adaptors::filtered(namespace_predicate(std::move(f.ns)));
}

}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_NAMESPACE_SELECTOR_HPP
