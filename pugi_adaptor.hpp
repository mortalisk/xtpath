
//          Copyright Morten Bendiksen 2004 - 2006.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_PUGI_ADAPTOR_HPP
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_PUGI_ADAPTOR_HPP

#include "xpath.hpp"
#include <pugixml.hpp>
#include <boost/range.hpp>
#include <sstream>

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

/// Adapts the pugixml library for use with XTpath. Include this file,
/// then instansiate an XTpath context node with the context(pugi::xml_node)
/// function, which is defined here. Other adaptors can be defined by
/// copying this file and implementing everything in this file for the
/// desired type.
struct PugiXmlAdaptor {
    // Must be defined
    typedef pugi::xml_node node_type;

    // This implementations way to select the namespace attributes.
    struct attribute_is_namespace {
        bool operator() (pugi::xml_attribute const& a) const {
            std::string name(a.name());
            return (name.size() > 6 &&
                 name.substr(0,6) == "xmlns:") || name == "xmlns";
        };
        typedef bool result_type;
    };

    // This implementations way to convert namespace xml_attribute
    // to a pair of strings
    struct namespace_to_pair {
        std::pair<std::string, std::string> operator()
                (pugi::xml_attribute const& a) const {
            std::string name(a.name());
            if (name.size() == 5)
               return std::make_pair("", a.as_string());
            name.erase(name.begin(), name.begin()+6);
            return std::make_pair(std::move(name), a.as_string());
        };
        typedef std::pair<std::string, std::string> result_type;
    };

    // This implementations way to convert xml_attribute to a pair
    // of strings
    struct attribute_to_pair {
        std::pair<std::string, std::string> operator()
                (pugi::xml_attribute const& a) const {
            return std::make_pair(a.name(), a.as_string());
        };
        typedef std::pair<std::string, std::string> result_type;
    };

    // returns the underlying type that represents a null node
    static pugi::xml_node null() {
        return pugi::xml_node();
    }

    static pugi::xml_node first_child(pugi::xml_node const& node) {
        return node.first_child();
    }

    static pugi::xml_node next_sibling(pugi::xml_node const& node) {
        return node.next_sibling();
    }

    static pugi::xml_node parent(pugi::xml_node const& node) {
        return node.parent();
    }

    static bool has_children(pugi::xml_node const& node) {
        return !node.first_child().empty();
    }

    static bool has_next_sibling(pugi::xml_node const& node) {
        return !node.next_sibling().empty();
    }

    // returns true if the given node is the root node of the
    // tree it is in
    static bool is_root(pugi::xml_node const& node) {
        return node && node.root() == node.parent();
    }

    // returns true if the given node represents a null node
    static bool is_null(pugi::xml_node const& node) {
        return !node;
    }

    // serialises the given node to a string
    static std::string to_text(pugi::xml_node const& node) {
        std::ostringstream ss;
        node.print(ss);
        return ss.str();
    }

    // must be defined, gives the type of the range returned from
    // the attributes function
    using attribute_range =
        boost::range_detail::transformed_range
        <attribute_to_pair,
         const decltype(std::declval<pugi::xml_node>()
                        .attributes())>;

    // returns a range of all the attributes of the given node
    static
    attribute_range
    attributes(pugi::xml_node const& node)
    {
        return
                node.attributes()
                | transformed(attribute_to_pair());
    }

    // returns the attribute with the given name from the given node
    static std::string attribute(pugi::xml_node const& node, std::string const& name) {
        return node.attribute(name.c_str()).value();
    }

    // returns the text content of the given node
    static std::string text(pugi::xml_node const& node) {
        return node.child_value();
    }

    // returns all the namespace declarations defined as
    // attributes on the given node. I.e. those starting
    // with "xmlns:"
    static
    boost::range_detail::transformed_range
    <namespace_to_pair,
     const boost::range_detail::filtered_range
     <attribute_is_namespace,
      const decltype(std::declval<pugi::xml_node>()
                     .attributes())> >
    namespace_declarations(pugi::xml_node const& node)
    {
        return
                node.attributes()
                | filtered(attribute_is_namespace())
                | transformed(namespace_to_pair());
    }
};

// constructs a XTpath context node from the pugi::xml_node
_context<PugiXmlAdaptor> context(pugi::xml_node const &node) {
    return _context<PugiXmlAdaptor>(node);
}
}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_PUGI_ADAPTOR_HPP
