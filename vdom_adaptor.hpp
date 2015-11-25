
//          Copyright Morten Bendiksen 2004 - 2006.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_VDOM_ADAPTOR_HPP
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_VDOM_ADAPTOR_HPP


#include "xpath.hpp"

#include <mediasequencer/vdom/node.hpp>
#include <mediasequencer/vdom/treeutil.hpp>
#include <mediasequencer/vdom/xml_convert.hpp>
#include <soul/compact_map.hpp>

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

namespace vdom = mediasequencer::vdom;

/// Adapts the VDOM library for use with XTpath. Include this file,
/// then instansiate an XTpath context node with the context(vdom::node)
/// function, which is defined here.
struct VdomAdaptor {
    typedef mediasequencer::vdom::node node_type;
    struct attribute_is_namespace {
        bool operator() (std::pair<std::string, std::string> const& a) const {
            std::string const& name(a.first);
            return name == "xmlns" ||
                (name.size() > 6 &&
                 name.substr(0,6) == "xmlns:");
        };
        typedef bool result_type;
    };

    struct namespace_to_pair {
        std::pair<std::string, std::string> operator()
                (std::pair<std::string, std::string> const& a) const {
            return a.first.size() == 5 ? std::make_pair("", a.second) : std::make_pair(a.first.substr(6), a.second);
        };
        typedef std::pair<std::string, std::string> result_type;
    };

    static vdom::node null() {
        return vdom::node::null;
    }

    static vdom::node first_child(vdom::node const& node) {
        return node.is_null() ? vdom::node::null : node.first_child();
    }

    static vdom::node next_sibling(vdom::node const& node) {
        return node.next_sibling();
    }

    static vdom::node parent(vdom::node const& node) {
        return node.is_null() ? vdom::node::null : node.parent();
    }

    static bool has_children(vdom::node const& node) {
        return node.number_of_children() > 0;
    }

    static bool has_next_sibling(vdom::node const& node) {
        return !node.next_sibling().is_null();
    }

    static bool is_root(vdom::node const& node) {
        return !node.is_null() && node.parent().is_null();
    }

    static bool is_null(vdom::node const& node) {
        return node.is_null();
    }

    static std::string to_text(vdom::node const& node) {
        return mediasequencer::vdom::treeutil::element_to_xml(node);
    }

    static std::string text(vdom::node const& node) {

        auto child = node.first_child();
        while (! child.is_null()) {
            if (child.type() == vdom::node::TEXT) {
                return child.value();
            }
            child = child.next_sibling();
        }
        return "";
    }

    using attribute_range = mediasequencer::vdom::node::attribute_range;

    static
    attribute_range attributes(vdom::node const& node)
    {
        return node.plain_attribute_range();
    }

    static std::string attribute(vdom::node const& node, std::string const& name) {
        return node.get_attribute(name);
    }

    static
    boost::range_detail::transformed_range
    <namespace_to_pair,
     const boost::range_detail::filtered_range
     <attribute_is_namespace,
      const attribute_range> >
    namespace_declarations(vdom::node const& node)
    {

        return    node.plain_attribute_range()
                | filtered(attribute_is_namespace())
                | transformed(namespace_to_pair());
    }
};

namespace {
inline
_context<VdomAdaptor> context(mediasequencer::vdom::node const &n) {
    return _context<VdomAdaptor>(n);
}
}

}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_VDOM_ADAPTOR_HPP
