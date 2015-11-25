
//          Copyright Morten Bendiksen 2004 - 2006.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_TEXT_SELECTOR_HPP
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_TEXT_SELECTOR_HPP

#include <numeric>

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

// Predicate for filtering out nodes that does not contain
// the given name.
template <typename Context>
struct node_contains_text {
    typedef bool result_type;
    bool operator()(Context const& c) const {
        return c.text().find(text) != std::string::npos;
    }
    std::string text;

    node_contains_text(std::string text) :text(text) {}
};

// the type for the text selector
struct _text {

};

/// The selector for filtering out nodes that does not cointain
/// the give text. 'range | text_contains("foo")' will give
/// all nodes from the range which has as text content a string
/// that contains the substring "foo"
struct text_contains {
    std::string text;
    text_contains(std::string text) : text(text)
    {}

};

// the type for the first_text selector
struct _first_text {

};

// the type for the xml_string selector
struct _xml_string {

};

namespace {
    /// The text selector. Gives the text content of the nodes in the input as
    /// a range of strings, e.g. 'range | text'
    const _text text;
    /// The first_text selector. Gives the text content of the first node in the input as
    /// a string, e.g. 'range | first_text'
    const _first_text first_text;
    /// The xml_string selector. Gives the xml representation of a node as a string. I.e.
    /// ' range | first | xml_string' gives one string representing the first node in the
    /// range.
    const _xml_string xml_string;
}

// Used to transform a context node to a string containing
// the nodes contained text.
template <typename Context>
struct node_to_text {
    typedef std::string result_type;
    std::string operator()(Context const& c) const {
        return c.text();
    }
};

/// The concatenate selector will concatenate the strings in the
/// input range to one string. It takes a string as parameter which
/// will be intertwined between each input string in the output
/// string. I.e. given a range with nodes named 'a', 'b' and 'c',
/// 'range | name | concatenate("-")' would give the string "a-b-c"
struct concatenate {
    concatenate(std::string delimiter) : delimiter(delimiter) {}
    std::string delimiter;
};

// Implements the pipe operator for the text selector
template<typename Range>
boost::range_detail::transformed_range
<node_to_text<typename Range::iterator::value_type>,
 const Range>
operator|(Range const& range, _text)
{
    return range | boost::adaptors::transformed(node_to_text<typename Range::iterator::value_type>());
}

// Implements the pipe operator for the first_text selector
template<typename Range>
std::string
operator|(Range const& range, _first_text) {
    auto r = range | text;
    if (r.begin() != r.end()) {
        return *(r.begin());
    } else {
        return "";
    }
}

// Implements the pipe operator for the text_contains selector
template<typename Range>
boost::range_detail::filtered_range
<node_contains_text<typename Range::iterator::value_type>,
 const Range>
operator|(Range const& range, text_contains p)
{
    return range | boost::adaptors::filtered(
                node_contains_text<typename Range::iterator::value_type>(p.text));
}

// Implements the pipe operator for the concatenate selector
template<typename Range>
std::string
operator|(Range const& range, concatenate c) {
    return
            std::accumulate(
                range.begin(), range.end(), std::string(""),
                [c](std::string& v, std::string const& x)
                { return v +(v.size()==0?"":c.delimiter)+x; }   );
}

// Implements the pipe operator for the xml_string selector
template <typename Context>
std::string
operator|(Context const& c, _xml_string) {
    return c.to_text();
}

}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_TEXT_SELECTOR_HPP
