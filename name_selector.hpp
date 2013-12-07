#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_NAME_SELECTOR_HPP
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_NAME_SELECTOR_HPP

#include <boost/range/adaptor/transformed.hpp>

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

class _name {

};

namespace {
    const _name name;
}

template<typename Element>
struct node_to_name {
    typedef std::string result_type;
     std::string operator()(Element element) const {
        std::string name = element.get_node().name();
        std::string::size_type colonPosition = name.find(':');
        if (colonPosition != std::string::npos) {
            name = name.substr(colonPosition+1);
        }
        return name;
    }
};

template <>
struct is_expr<_name>: std::true_type {
};

template<typename Range>
auto
operator|(Range const& range, _name const&)
->  decltype(range |transformed(node_to_name<typename Range::iterator::reference>()))
{
    return range |transformed(node_to_name<typename Range::iterator::reference>());
}

}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_NAME_SELECTOR_HPP
