/*
 *   Copyright 2013 Morten Bendiksen (morten.bendiksen@gmail.com)
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */
#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_PUGI_ADAPTOR_HPP
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_PUGI_ADAPTOR_HPP

#include "xpath.hpp"
#include <pugixml.hpp>
#include <boost/range.hpp>
#include <sstream>

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

struct PugiXmlAdaptor {
    struct attribute_is_namespace {
        bool operator() (pugi::xml_attribute const& a) const {
            std::string name(a.name());
            return (name.size() > 6 &&
                 name.substr(0,6) == "xmlns:") || name == "xmlns";
        };
        typedef bool result_type;
    };


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

    struct attribute_to_pair {
        std::pair<std::string, std::string> operator()
                (pugi::xml_attribute const& a) const {
            return std::make_pair(a.name(), a.as_string());
        };
        typedef std::pair<std::string, std::string> result_type;
    };

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
        return node.first_child();
    }

    static bool has_next_sibling(pugi::xml_node const& node) {
        return node.next_sibling();
    }

    static bool is_root(pugi::xml_node const& node) {
        return node.root() == node.parent();
    }

    static bool is_null(pugi::xml_node const& node) {
        return !node;
    }

    static std::string to_text(pugi::xml_node const& node) {
        std::ostringstream ss;
        node.print(ss);
        return ss.str();
    }

    static
    auto
    attributes(pugi::xml_node const& node)
    -> decltype(node.attributes()
                | transformed(attribute_to_pair()))
    {
        return
                node.attributes()
                | transformed(attribute_to_pair());
    }

    static std::string attribute(pugi::xml_node const& node, std::string const& name) {
        return node.attribute(name.c_str()).value();
    }

    static std::string text(pugi::xml_node const& node) {
        return node.child_value();
    }

    static
    auto
    namespace_declarations(pugi::xml_node const& node)
    -> decltype(node.attributes()
                | filtered(attribute_is_namespace())
                | transformed(namespace_to_pair()))
    {
        return
                node.attributes()
                | filtered(attribute_is_namespace())
                | transformed(namespace_to_pair());
    }
};

_context<pugi::xml_node, PugiXmlAdaptor> context(pugi::xml_node const &node) {
    return _context<pugi::xml_node, PugiXmlAdaptor>(node);
}
}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_PUGI_ADAPTOR_HPP
