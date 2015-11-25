
//          Copyright Morten Bendiksen 2004 - 2006.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_ATTRIBUTE_SELECTOR_HPP
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_ATTRIBUTE_SELECTOR_HPP

#include "context.hpp"
#include "selector_common.hpp"
#include "attribute_iterator.hpp"

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

// Predicate that is used to filter out attributes which does not have
// the given name
struct filtered_attribute_name {
    explicit filtered_attribute_name(std::string name): name(std::move(name)) {
    }

    filtered_attribute_name(filtered_attribute_name&& other)
        : name(std::move(other.name)) {
    }

    filtered_attribute_name(filtered_attribute_name const& other)
        : name(other.name) {
    }

    filtered_attribute_name& operator=(filtered_attribute_name const& other) {
        name = other.name;
        return *this;
    }

    typedef bool return_type;

    bool operator()(std::pair<std::string, std::string> const& pair) const {
        return pair.first == name;
    }

    std::string name;
};

// Predicate that is used to filter out attributes which does not have
// the given name and value
struct filtered_attribute_name_and_value {

    filtered_attribute_name_and_value() {}

    explicit filtered_attribute_name_and_value(std::string name, std::string value)
        : name(std::move(name)), value(std::move(value)) {
    }

    filtered_attribute_name_and_value(filtered_attribute_name_and_value&& other)
        : name(std::move(other.name)), value(std::move(other.value)) {
    }

    filtered_attribute_name_and_value(filtered_attribute_name_and_value const& other)
        : name(other.name), value(other.value) {
    }

    filtered_attribute_name_and_value& operator=(filtered_attribute_name_and_value const& other) {
        name = other.name;
        value = other.value;
        return *this;
    }

    typedef bool return_type;

    template <typename C>
    bool operator()(C const& s) const {
        return s.attribute(name) == value;
    }

    std::string name;
    std::string value;
};

// type for attribute selector
class _attribute {
public:
    filtered_attribute_name operator()(std::string name) const {
        return filtered_attribute_name(name);
    }

    filtered_attribute_name_and_value operator()(
            std::string name, std::string value) const {
        return filtered_attribute_name_and_value(name, value);
    }
};

namespace {
    /// selector object for attribute.  Can be used as a function
    /// taking a name of the target attribute, or the name and value.
    /// 'range | attribute' gives all attributes defined on the
    /// input range as pairs of strings
    /// 'range | attribute("foo")' gives the values of the
    /// attributes with the name 'foo'
    /// 'range | attribute("foo", "bar")' gives the nodes that
    /// have the attributes 'foo' set to the value 'bar'
    const _attribute attribute;
}

// Implements the pipe operator for attributes. E.g.
// 'range | attributes'
template <typename Range>
boost::iterator_range<attribute_iterator<typename Range::iterator> >
operator|(Range const& range, _attribute)
{
    return make_attributes(range);
}

// Used to transform a range of pairs of strings to a range of strings.
// The strings will be the second value from the pairs.
// Used to extract only the values of a range of attributes.
struct second_of_pair {
    typedef std::string result_type;

    std::string operator()(std::pair<std::string, std::string> const& pair) const{
        return pair.second;
    }
    std::string operator()(std::pair<std::string, std::string>&& pair) const{
        return std::move(pair.second);
    }
};

// Implements the pipe operator for attributes filtered on name. E.g.
// 'range | attributes("foo")'
template <typename Range>
boost::range_detail::transformed_range
<second_of_pair,
 const boost::range_detail::filtered_range
 <filtered_attribute_name,
  const boost::iterator_range<attribute_iterator<typename Range::iterator> > > >
operator|(Range const& range, filtered_attribute_name f)
{
    return make_attributes(range) | boost::adaptors::filtered(f) |
            boost::adaptors::transformed(second_of_pair());
}

// Implements the pipe operator for attributes filtered on name and value. E.g.
// 'range | attributes("foo", "bar")'
template <typename Range>
boost::range_detail::filtered_range<filtered_attribute_name_and_value,
                                    const Range>
operator|(Range const& range, filtered_attribute_name_and_value f)
{
    return range | boost::adaptors::filtered(f);
}

// enables the selector for attributes filtered on name and value
template <>
struct is_expr<filtered_attribute_name_and_value>: std::true_type {
};

// enables the selector for attributes filtered on name
template <>
struct is_expr<filtered_attribute_name>: std::true_type {
};

// enables the selector for attributes
template <>
struct is_expr<_attribute>: std::true_type {
};

}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_ATTRIBUTE_SELECTOR_HPP
