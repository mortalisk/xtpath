
//          Copyright Morten Bendiksen 2004 - 2006.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_NAMESPACE_SELECTOR_HPP
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_NAMESPACE_SELECTOR_HPP

#include "scopedmap.hpp"
#include <boost/range/adaptor/transformed.hpp>

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

// A selector that represents the selection of a certain namespace
struct filtered_namespace {
    explicit filtered_namespace(std::string s): ns(std::move(s)) {
    }

    filtered_namespace(filtered_namespace const& other)
        : ns(other.ns) {
    }

    std::string ns;
};

// The type for the namespace selector
struct _namespace {
    filtered_namespace operator()(std::string ns) const {
        return filtered_namespace(ns);
    }
};

namespace {
    /// The namespace selector. Gives the namespaces
    /// of the nodes in the input range as pairs of strings
    /// if used as is, i.e. 'range | ns'
    /// Can be used as a function taking the target namespace,
    /// i.e. 'range | ns("http://foo")'. Will then give the nodes
    /// that are in the given namespace.
    const _namespace ns;
}

// Transforms a range of nodes to a range of the namespaces
// they are in as strings.
template<typename Element>
struct node_to_namespace {
    typedef std::string result_type;
     std::string operator()(Element element) const {
        std::string name = element.name();
        std::string prefix("");
        std::string::size_type colonPosition = name.find(':');
        if (colonPosition != std::string::npos) {
            prefix = name.substr(0,colonPosition);
        }
        auto ns = element.ns().get(prefix);
        if (ns)
            return *ns;
        else
            return "";
    }
};

// A predicate for filtering out nodes that do not
// have the given namespace
class namespace_predicate {
public:
    bool operator()(std::string i) const {
       return i == name;
    }

    explicit namespace_predicate(std::string&& name)
        : name(std::move(name)) {
    }

private:
    std::string name;

};

// enables the namespace selector in sub expressions
template <>
struct is_expr<_namespace>: std::true_type {
};

// enables filtering on namespaces in sub expressions
template <>
struct is_expr<filtered_namespace>: std::true_type {
};


// implements the pipe operator for the namespace selector
// E.g. 'range | ns'
template<typename Range>
boost::range_detail::transformed_range
<node_to_namespace<typename Range::iterator::reference>,
 const Range>
operator|(Range const& range, _namespace const&)
{
    return range |
            boost::adaptors::transformed(node_to_namespace<typename Range::iterator::reference>());
}

// implements the pipe operator for the namespace selector
// filtered on a specific namespace
// E.g. 'range | ns("http://example.com")'
template<typename Range>
boost::range_detail::filtered_range
<namespace_predicate,
 const boost::range_detail::transformed_range
 <node_to_namespace<typename Range::iterator::reference>,
  const Range> >
operator|(Range const& range, filtered_namespace f)
{
    return range | ns |
            boost::adaptors::filtered(namespace_predicate(std::move(f.ns)));
}

}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_NAMESPACE_SELECTOR_HPP
