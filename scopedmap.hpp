#pragma once
#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_SCOPEDMAP_HPP
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_SCOPEDMAP_HPP

#include <boost/noncopyable.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/optional.hpp>

#include <unordered_map>
#include <algorithm>
#include <memory>
#include <assert.h>

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

template <class K, class V>
class scopedmap {
private:
    struct scope: boost::noncopyable {
        std::shared_ptr<scope> parent;
        unsigned identifier;
        std::vector<K> keys;
        std::unordered_map<K, std::vector<std::pair<unsigned, V> > > & map;

        scope(std::shared_ptr<scope> parent,
              unsigned identifier,
              std::unordered_map<K, std::vector<std::pair<unsigned, V> > > & map): parent(std::move(parent)),
            identifier(identifier), map(map){
        }
        static unsigned get_first(std::pair<unsigned, V> const& p) {
            return p.first;
        };
        ~scope() {
            for (auto& k : keys) {
                auto i = map.find(k);
                assert(i != map.end());
                auto begin = boost::make_transform_iterator(i->second.begin(), get_first);
                auto end = boost::make_transform_iterator(i->second.end(), get_first);
                auto j = std::lower_bound(begin, end, identifier);
                assert(j != end);
                i->second.erase(j.base());
            }
        }
    };

    std::shared_ptr<std::pair<unsigned, std::unordered_map<K, std::vector<std::pair<unsigned, V> > > > > shared;
    std::shared_ptr<scope> s;

public:
    scopedmap() :
        shared(new std::pair<unsigned, std::unordered_map<K, std::vector<std::pair<unsigned, V> > > >()),
        s(new scope(std::shared_ptr<scope>(), 0u, shared->second))
    { shared->first = 1u; }

    scopedmap(scopedmap const& other): shared(other.shared), s(other.s) {}
    scopedmap(scopedmap&& other): shared(std::move(other.shared)),s(std::move(other.s)) {}

    scopedmap& operator=(scopedmap&& other) {
        std::swap(shared, other.shared);
        std::swap(s, other.s);
        return *this;
    }

    scopedmap& operator=(scopedmap const& other) {
        shared = other.shared;
        s = other.s;
        return *this;
    }

private:
    scopedmap(
            std::shared_ptr<std::pair<unsigned, std::unordered_map<K, std::vector<std::pair<unsigned, V> > > > > shared,
            std::shared_ptr<scope> new_scope)
        : shared(std::move(shared)), s(std::move(new_scope))
           {
    }

public:
    template <typename Iterator>
    scopedmap add(Iterator begin, Iterator end) {
        if(begin == end)return *this;
        std::shared_ptr<scope> new_scope(new scope(s, (shared->first), (shared->second)));
        for (; begin != end; ++begin) {
            new_scope->keys.push_back(begin->first);
            shared->second[begin->first].push_back(std::make_pair(shared->first, begin->second));
        }
        (shared->first)++;
        return scopedmap(shared, std::move(new_scope));
    }

    boost::optional<const V&> const get(K const& k) const {
        scope* current = s.get();
        auto i = shared->second.find(k);

        if (i == shared->second.end())
            return boost::optional<const V&>();
        auto& vect = i->second;

        auto j = vect.rbegin();
        while (current && j != vect.rend()) {
            if (current->identifier > j->first) {
                current = current->parent.get();
            } else if (current->identifier < j->first) {
                ++j;
            } else {
                return boost::optional<const V&>(j->second);
            }
        }
        return boost::optional<const V&>();
    }
};

}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_SCOPEDMAP_HPP
