
//          Copyright Morten Bendiksen 2004 - 2006.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_NAME_SELECTOR_HPP
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_NAME_SELECTOR_HPP

#include <boost/range/adaptor/transformed.hpp>

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

// The type for the name selector
class _name {

};

namespace {
    /// The name selector gives the name of all nodes
    /// in the input range in a range of strings
    const _name name;
}

// Used in a boost transformed_range to transform from
// a range of context nodes to a range of strings
// that are the names of the nodes
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

// enables the name selector in sub expressions
template <>
struct is_expr<_name>: std::true_type {
};

// Implements the pipe operator for the name selector.
// E.g. 'range | name'
template<typename Range>
boost::range_detail::transformed_range
<node_to_name<typename Range::iterator::reference>,
 const Range>
operator|(Range const& range, _name const&)
{
    return range |transformed(node_to_name<typename Range::iterator::reference>());
}

}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_NAME_SELECTOR_HPP
