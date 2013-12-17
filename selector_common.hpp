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
#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_SELECTOR_COMMON_HPP
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_SELECTOR_COMMON_HPP

#include <iostream>

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {


// A predicate for filtering on node names
template <typename Input>
class name_predicate {
public:
    bool operator()(Input& i) const {
        std::string n(i.get_node().name());
        std::string::size_type colonPosition = n.find(':');
        if (colonPosition != std::string::npos) {
            n = n.substr(colonPosition+1);
        }
        return std::string(n) == name;
    }

    name_predicate(name_predicate&& other)
        : name(std::move(other.name)) {
    }

    name_predicate(name_predicate const& other)
        : name(other.name) {
    }

    name_predicate& operator=(name_predicate const& other) {
        name = other.name;
        return *this;
    }

    explicit name_predicate(std::string&& name)
        : name(std::move(name)) {
    }

private:
    std::string name;

};

// To enable all filter types to be used in "where(...)" expressions
template <typename T>
struct is_expr: public std::false_type {
};

}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_SELECTOR_COMMON_HPP
