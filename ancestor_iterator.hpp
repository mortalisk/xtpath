
//          Copyright Morten Bendiksen 2004 - 2006.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_ANCESTOR_ITERATOR
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_ANCESTOR_ITERATOR

#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <memory>

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

// An iterator over a range of context nodes. It is given another range
// of nodes and iterates through all ancestors of all nodes in that
// other range.
template <typename ParentIterator>
class ancestor_iterator : public boost::iterator_facade<
        ancestor_iterator<ParentIterator>,
        typename ParentIterator::value_type,
        boost::forward_traversal_tag> {
private:
    typedef boost::iterator_facade<
    ancestor_iterator<ParentIterator>,
    typename ParentIterator::value_type,
    boost::forward_traversal_tag> super;
public:
    ancestor_iterator() {}

    ancestor_iterator(ParentIterator begin, ParentIterator end) {
        if (begin != end) {
            d.reset(new data(std::move(begin), std::move(end)));
            increment();
        }

    }

    ancestor_iterator(ancestor_iterator<ParentIterator> const& other) {
        if (other.d) {
            d.reset(new data(*(other.d)));
        }
    }

    ancestor_iterator(ancestor_iterator<ParentIterator>&& other) {
        d = std::move(other.d);
    }

    ancestor_iterator& operator=(ancestor_iterator<ParentIterator> const& other) {
        if (other.d) {
            d.reset(new data(*(other.d)));
        } else {
            d.reset();
        }
        return *this;
    }

    ancestor_iterator& operator=(ancestor_iterator<ParentIterator>&& other) {
        d = std::move(other.d);
        return *this;
    }

    void increment() {
        if (d->c->is_root() || d->c->is_null()) {
            ++(d->parent_it);
            if (d->parent_it != d->parent_end) {
                d->c.reset(new typename super::value_type (*(d->parent_it)));
                increment();
            } else {
                d.reset();
                return;
            }
        } else {
            d->c->parent();
        }
    }

    bool equal(ancestor_iterator const& other) const {
        if (!d || !other.d) {
            return d.get() == other.d.get();
        }
        assert(d->c && other.d->c);
        return
            *(d->c) == *(other.d->c) &&
            d->parent_it == other.d->parent_it &&
            d->parent_end == other.d->parent_end;
    }

    typename super::reference dereference() const {
        return *(d->c);
    }
private:
    // the data is put in a unique_ptr, so that it is nullable
    // in order to preserve memory
    struct data {
        data(ParentIterator parent_it,
             ParentIterator parent_end)
            : parent_it(parent_it),
              parent_end(parent_end),
              c(std::make_shared<typename super::value_type>(*parent_it)){
        }

        data(data const& other)
            : parent_it(other.parent_it),
              parent_end(other.parent_end),
              c(std::make_shared<typename super::value_type>(*(other.c)))
        {
        }

        // An iterator in the input range. The iterator points to
        // a decendant of the node this iterator points to.
        ParentIterator parent_it;
        // Represents the end of the input range
        ParentIterator parent_end;
        // points to the context we are at in this iterator
        std::shared_ptr<typename super::value_type> c;
    };

    std::unique_ptr<data> d;
};



template <typename Range>
boost::iterator_range<ancestor_iterator<typename Range::iterator> >
make_ancestor(Range const& r) {
    return boost::make_iterator_range
            (ancestor_iterator<typename Range::iterator>(r.begin(), r.end()),
             ancestor_iterator<typename Range::iterator>());
}

}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_ANCESTOR_ITERATOR
