
//          Copyright Morten Bendiksen 2004 - 2006.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_ANCESTOR_SELECTOR_HPP
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_ANCESTOR_SELECTOR_HPP

#include "selector_common.hpp"
#include "ancestor_iterator.hpp"

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {


struct filtered_ancestor {
    explicit filtered_ancestor(std::string s): name(std::move(s)) {
    }

    filtered_ancestor(filtered_ancestor&& other)
        : name(std::move(other.name)) {

    }
    filtered_ancestor(filtered_ancestor const& other)
        : name(other.name) {
    }

    std::string name;
};

// the type for the ancestor selector
// also defines the function for filtering
// on name
class _ancestor {
public:
    filtered_ancestor operator()(std::string name) const {
        return filtered_ancestor(name);
    }
};

namespace {
    /// The ancestor selector object. Can be used as a function
    /// taking a name of the target ancestor. I.e. 'range | ancestor'
    /// or 'range | ancestor("foo")'
    const _ancestor ancestor;
}

// the implementation of the pipe operator that takes
// a range and an ancestor. E.g. 'range | ancestor'
template <typename Range>
boost::iterator_range<ancestor_iterator<typename Range::iterator> >
operator|(Range const& range,
          _ancestor const&)
{
    return make_ancestor(range);
}

// the implementation of the pipe operator that takes
// a range and a filtered_ancestor. E.g. 'range | ancestor("foo")'
template <typename Range>
boost::range_detail::filtered_range
<name_predicate<typename Range::iterator::reference>,
 const boost::iterator_range<ancestor_iterator<typename Range::iterator> > >
operator|(Range const& range,
          filtered_ancestor f)
{
    return make_ancestor(range)
            | boost::adaptors::filtered(name_predicate<typename Range::iterator::reference>(f.name));
}

// enables _ancestor type as a sub-expression
template <>
struct is_expr<_ancestor>: std::true_type {
};

// enables filtered_ancestor as a sub-expression
template <>
struct is_expr<filtered_ancestor>: std::true_type {
};


}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_ANCESTOR_SELECTOR_HPP
