#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_ANCESTOR_SELECTOR_HPP
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_ANCESTOR_SELECTOR_HPP

#include "selector_common.hpp"
#include "ancestor_iterator.hpp"

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {


struct filtered_ancestor {
    explicit filtered_ancestor(std::string s): name(std::move(s)) {
    }

    filtered_ancestor(filtered_ancestor&& other)
        : name(std::move(other.name)) {

    }
    filtered_ancestor(filtered_ancestor const& other)
        : name(other.name) {
    }

    std::string name;
};

class _ancestor {
public:
    filtered_ancestor operator()(std::string name) const {
        return filtered_ancestor(name);
    }
};

namespace {
    const _ancestor ancestor;
}

template <typename Range>
auto
operator|(Range const& range,
          _ancestor const&)
-> decltype(make_ancestor(range))
{
    return make_ancestor(range);
}

template <typename Range>
auto
operator|(Range const& range,
          filtered_ancestor f)
-> decltype(make_ancestor(range)
            | boost::adaptors::filtered(name_predicate<typename Range::iterator::reference>(std::move(f.name))))
{
    return make_ancestor(range)
            | boost::adaptors::filtered(name_predicate<typename Range::iterator::reference>(std::move(f.name)));
}

template <>
struct is_expr<_ancestor>: std::true_type {
};

template <>
struct is_expr<filtered_ancestor>: std::true_type {
};


}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_ANCESTOR_SELECTOR_HPP
