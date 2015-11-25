
//          Copyright Morten Bendiksen 2004 - 2006.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_PARENT_SELECTOR_HPP
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_PARENT_SELECTOR_HPP

#include "selector_common.hpp"
#include "parent_iterator.hpp"

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

// represents the parent selector filtered on the name of
// the parent
struct filtered_parent {
    explicit filtered_parent(std::string s): name(std::move(s)) {
    }

    filtered_parent(filtered_parent const& other)
        : name(other.name) {
    }

    std::string name;
};

// the type of the parent selector object
class _parent {
public:
    filtered_parent operator()(std::string name) const {
        return filtered_parent(name);
    }
};

namespace {
    /// The parent selctor object gives the parents of each node
    /// in the input range if used on its own, i.e. 'range | parent'
    /// If given a targen parent name as argument, will give only the
    /// parents with that name, i.e. 'range | parent("foo")'
    const _parent parent;
}

// Implements the pipe operator for the parent selector. E.g.
// 'range | parent'
template <typename Range>
boost::iterator_range<parent_iterator<typename Range::iterator> >
operator|(Range const& range,
          _parent const&)
{
    return make_parent(range);
}

// Implements the pipe operator for the parent selector filtered on
// name. E.g. 'range | parent("foo")'
template <typename Range>
boost::range_detail::filtered_range
<name_predicate<typename Range::iterator::reference>,
 const boost::iterator_range<parent_iterator<typename Range::iterator> > >
operator|(Range const& range,
          filtered_parent f)
{
    return make_parent(range)
            | filtered(name_predicate<typename Range::iterator::reference>(f.name));
}


// enables the parent selector in sub expressions
template <>
struct is_expr<_parent>: std::true_type {
};

// enables the filtered parent selector in sub expressions
template <>
struct is_expr<filtered_parent>: std::true_type {
};


}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_PARENT_SELECTOR_HPP
