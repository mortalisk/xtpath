#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_TEXT_SELECTOR_HPP
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_TEXT_SELECTOR_HPP

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

template <typename Context>
struct node_contains_text {
    typedef bool result_type;
    bool operator()(Context const& c) const {
        return c.text().find(text) != std::string::npos;
    }
    std::string text;

    node_contains_text(std::string text) :text(text) {}
};


struct _text {

};

struct text_contains {
    std::string text;
    text_contains(std::string text) : text(text)
    {}

};

struct _first_text {

};

struct _xml_string {

};

namespace {
    const _text text;
    const _first_text first_text;
    const _xml_string xml_string;
}

template <typename Context>
struct node_to_text {
    typedef std::string result_type;
    std::string operator()(Context const& c) const {
        return c.text();
    }
};

struct concatenate {
    concatenate(std::string delimiter) : delimiter(delimiter) {}
    std::string delimiter;
};


template<typename Range>
auto
operator|(Range range, _text)
-> decltype(range | boost::adaptors::transformed(node_to_text<typename Range::iterator::value_type>()))
{
    return range | boost::adaptors::transformed(node_to_text<typename Range::iterator::value_type>());
}

template<typename Range>
std::string
operator|(Range range, _first_text) {
    auto r = range | text;
    if (r.begin() != r.end()) {
        return *(r.begin());
    } else {
        return "";
    }
}

template<typename Range>
auto
operator|(Range range, text_contains p)
-> decltype(range | boost::adaptors::filtered(
                node_contains_text<typename Range::iterator::value_type>(p.text)))
{
    return range | boost::adaptors::filtered(
                node_contains_text<typename Range::iterator::value_type>(p.text));
}

template<typename Range>
std::string
operator|(Range range, concatenate c) {
    return
            std::accumulate(
                range.begin(), range.end(), std::string(""),
                [c](std::string& v, std::string const& x)
                { return v +(v.size()==0?"":c.delimiter)+x; }   );
}

template <typename Context>
std::string
operator|(Context const& c, _xml_string) {
    return c.to_text();
}

}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_TEXT_SELECTOR_HPP
