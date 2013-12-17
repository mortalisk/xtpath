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
#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_NAME_SELECTOR_HPP
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_NAME_SELECTOR_HPP

#include <boost/range/adaptor/transformed.hpp>

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

class _name {

};

namespace {
    const _name name;
}

template<typename Element>
struct node_to_name {
    typedef std::string result_type;
     std::string operator()(Element element) const {
        std::string name = element.get_node().name();
        std::string::size_type colonPosition = name.find(':');
        if (colonPosition != std::string::npos) {
            name = name.substr(colonPosition+1);
        }
        return name;
    }
};

template <>
struct is_expr<_name>: std::true_type {
};

template<typename Range>
auto
operator|(Range const& range, _name const&)
->  decltype(range |transformed(node_to_name<typename Range::iterator::reference>()))
{
    return range |transformed(node_to_name<typename Range::iterator::reference>());
}

}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_NAME_SELECTOR_HPP
