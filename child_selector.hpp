
//          Copyright Morten Bendiksen 2014 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_CHILD_SELECTOR_HPP
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_CHILD_SELECTOR_HPP

#include "selector_common.hpp"
#include "child_iterator.hpp"

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

struct filtered_children {
    explicit filtered_children(std::string s): name(s) {
    }

    std::string name;
};

// Type for the child selector object
class _child {
public:
    filtered_children operator()(std::string name) const {
        return filtered_children(name);
    }
};

namespace {
    /// Selector object for iterating children. Can be used as
    /// is to get all children, i.e. 'range | child'
    /// or as a function taking the name of target children
    /// to get only the children with matching name
    /// i.e 'range | child("foo")'
    const _child child;
}

// Implements the pipe operator for the child selector. E.g.
// 'range | child'
template <typename Range>
boost::iterator_range<child_iterator<typename Range::iterator> >
operator|(Range const& range,
          _child const&)
{
    return make_children(range);
}

// Implements the pipe operator for the filtered_children type. E.g.
// 'range | child("foo")'
template <typename Range>
boost::range_detail::filtered_range
<name_predicate<typename Range::iterator::value_type>,
 const boost::iterator_range<child_iterator<typename Range::iterator> > >
operator|(Range const& range,
          filtered_children f)
{
    return make_children(range)
            | boost::adaptors::filtered(name_predicate<typename Range::iterator::value_type>(f.name));
}

// enables the _child type in sub-expressions
template <>
struct is_expr<_child>: std::true_type {
};
// enables the filtered_children type in sub-expressions
template <>
struct is_expr<filtered_children>: std::true_type {
};


}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_CHILD_SELECTOR_HPP
