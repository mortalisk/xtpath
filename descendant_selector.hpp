
//          Copyright Morten Bendiksen 2004 - 2006.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_DESCENDANT_SELECTOR_HPP
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_DESCENDANT_SELECTOR_HPP


#include "selector_common.hpp"
#include "descendant_iterator.hpp"

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

struct filtered_descendant {
    explicit filtered_descendant(std::string s): name(std::move(s)) {
    }

    filtered_descendant(filtered_descendant&& other)
        : name(std::move(other.name)) {

    }
    filtered_descendant(filtered_descendant const& other)
        : name(other.name) {
    }

    std::string name;
};

// The type for the decentand selector
class _descendant {
public:
    filtered_descendant operator()(std::string name) const {
        return filtered_descendant(name);
    }
};

namespace {
    /// The descendant selector gives all descendants of all
    /// nodes in the input range, i.e. 'range | descendant'.
    /// Can be used as a function taking the target descentant
    /// name as input, i.e. 'range | descendant("foo")' to get
    /// only the descendants with the name "foo".
    const _descendant descendant;
}

// Implements the pipe operator for the descendant selector
// E.g. 'range | descendant'
template <typename Range>
boost::iterator_range<descendant_iterator<typename Range::iterator> >
operator|(Range const& range,
          _descendant const&)
{
    return make_descendant(range);
}

// Implements the pipe operator for the descendant selector filtered
// on name. E.g. 'range | descendant("foo")'
template <typename Range>
boost::range_detail::filtered_range
<name_predicate<typename Range::iterator::reference>,
 const boost::iterator_range<descendant_iterator<typename Range::iterator> > >
operator|(Range const& range,
          filtered_descendant f)
{
    return make_descendant(range)
            | boost::adaptors::filtered(name_predicate<typename Range::iterator::reference>(f.name));
}

// enables the descendant selector in sub expressions
template <>
struct is_expr<_descendant>: std::true_type {
};

// enables the descendant selector filtered on name in sub expressions
template <>
struct is_expr<filtered_descendant>: std::true_type {
};



}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_DESCENDANT_SELECTOR_HPP
