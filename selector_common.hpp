
//          Copyright Morten Bendiksen 2014 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_SELECTOR_COMMON_HPP
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_SELECTOR_COMMON_HPP

#include <iostream>

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {


// A predicate for filtering on node names. Used in several places
template <typename Input>
class name_predicate {
public:
    bool operator()(Input& i) const {
        std::string n(i.name());
        std::string::size_type colonPosition = n.find(':');
        if (colonPosition != std::string::npos) {
            n = n.substr(colonPosition+1);
        }
        return std::string(n) == name;
    }

    name_predicate(name_predicate const& other)
        : name(other.name) {
    }

    name_predicate& operator=(name_predicate const& other) {
        name = other.name;
        return *this;
    }

    explicit name_predicate(std::string name)
        : name(name) {
    }

    name_predicate() {}

private:
    std::string name;

};

// To enable all filter types to be used in "where(...)" expressions
// this template must be specialiced for the type which should inherit
// std::true_type
template <typename T>
struct is_expr: public std::false_type {
};

}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_SELECTOR_COMMON_HPP
