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
