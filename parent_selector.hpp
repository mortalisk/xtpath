#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_PARENT_SELECTOR_HPP
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_PARENT_SELECTOR_HPP

#include "selector_common.hpp"
#include "parent_iterator.hpp"

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

struct filtered_parent {
    explicit filtered_parent(std::string s): name(std::move(s)) {
    }

    filtered_parent(filtered_parent&& other)
        : name(std::move(other.name)) {

    }
    filtered_parent(filtered_parent const& other)
        : name(other.name) {
    }

    std::string name;
};

class _parent {
public:
    filtered_parent operator()(std::string name) const {
        return filtered_parent(name);
    }
};

namespace {
    const _parent parent;
}

template <typename Range>
auto
operator|(Range const& range,
          _parent const&)
-> decltype(make_parent(range))
{
    return make_parent(range);
}

template <typename Range>
auto
operator|(Range const& range,
          filtered_parent f)
-> decltype(make_parent(range)
            | filtered(name_predicate<typename Range::iterator::reference>(std::move(f.name))))
{
    return make_parent(range)
            | filtered(name_predicate<typename Range::iterator::reference>(std::move(f.name)));
}



template <>
struct is_expr<_parent>: std::true_type {
};

template <>
struct is_expr<filtered_parent>: std::true_type {
};


}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_PARENT_SELECTOR_HPP
