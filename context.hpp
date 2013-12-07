#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_CONTEXT_HPP
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_CONTEXT_HPP

#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include "singleton_iterator.hpp"
#include <deque>
#include "scopedmap.hpp"

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

using namespace boost::adaptors;


// a context object holds a node (i.e. pugi::xml_node)
// and uses an Adaptor to access it
template <typename NodeType, typename Adaptor>
class _context
{
public:
    typedef singleton_iterator<_context<NodeType, Adaptor> > iterator;
    typedef const singleton_iterator<_context<NodeType, Adaptor> > const_iterator;
    typedef NodeType node_type;
    typedef Adaptor adaptor;
    typedef decltype(Adaptor::attributes(NodeType())) AttributeRange;
    typedef typename AttributeRange::iterator AttributeIterator;
private:
    NodeType node;
    std::deque<scopedmap<std::string, std::string> > maps;

    void build_map(NodeType const& n) {
        if (Adaptor::is_null(n)) {
            maps.push_back(scopedmap<std::string, std::string>());
            return;
        }
        build_map(Adaptor::parent(n));

        push_map(n);
    }

    void push_map(NodeType const& n) {

        assert(!Adaptor::is_null(n));

        auto namespacePairs = Adaptor::namespace_declarations(n);

        maps.push_back(maps.back().add(namespacePairs.begin(), namespacePairs.end()));


    }

public:
    _context& operator=(_context const& other) {
        maps = other.maps;
        node = other.node;
        return *this;
    }

    _context& operator=(_context&& other) {
        maps = std::move(other.maps);
        node = std::move(other.node);
        return *this;
    }

    singleton_iterator<_context> begin() const {
        if (Adaptor::is_null(node)) {
            return singleton_iterator<_context>();
        }else {
            return singleton_iterator<_context>(*this);
        }
    }

    singleton_iterator<_context> end() const {
        return singleton_iterator<_context>();
    }

    explicit _context(NodeType const& n): node(n) {
        build_map(n);
    }

    _context(_context const& other): node(other.node), maps(other.maps) {
    }

    _context(_context && other)
        : node(std::move(other.node)),
          maps(std::move(other.maps)) {
    }

    _context() : node(Adaptor::null()) {}

    NodeType const& get_node() const {
        return node;
    }

    std::string to_text() const {
        if (Adaptor::is_null(node)) {
            return "";
        } else {
            return Adaptor::to_text(node);
        }
    }

    std::string text() const {
        return Adaptor::text(node);
    }

    bool operator==(_context const& other) const {
        return node == other.node;
    }

    void first_child() {
        node = Adaptor::first_child(node);
        if (node)
          push_map(node);
    }

    void next_sibling() {
        maps.pop_back();
        node = Adaptor::next_sibling(node);
        if (node)
          push_map(node);
    }

    void parent() {
        maps.pop_back();
        node = Adaptor::parent(node);
    }

    bool has_children() const {
        return Adaptor::has_children(node);
    }

    bool has_next_sibling() const {
        return Adaptor::has_next_sibling(node);
    }

    bool is_root() const {
        return Adaptor::is_root(node);
    }

    scopedmap<std::string, std::string> const& ns() const {
        return maps.back();
    }

    AttributeRange attributes() const {
        return Adaptor::attributes(node);
    }

    std::string attribute(std::string const& name) const{
        return Adaptor::attribute(node, name);
    }

    bool is_null() const {
        return Adaptor::is_null(node);
    }

    std::string name() const {
	return node.name();
    }

};

}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_CONTEXT_HPP
