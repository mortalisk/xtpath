
//          Copyright Morten Bendiksen 2014 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_PARENT_ITERATOR
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_PARENT_ITERATOR

#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <memory>

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

// An iterator over a range of context nodes. It is given another range
// of nodes and iterates through the parents of all nodes in that
// other range.
template <typename ParentIterator>
class parent_iterator : public boost::iterator_facade<
        parent_iterator<ParentIterator>,
        typename ParentIterator::value_type,
        boost::forward_traversal_tag> {
private:
    typedef boost::iterator_facade<
    parent_iterator<ParentIterator>,
    typename ParentIterator::value_type,
    boost::forward_traversal_tag> super;
public:
    parent_iterator() {}

    parent_iterator(ParentIterator begin, ParentIterator end) {
        if (begin != end) {
            d.reset(new data(std::move(begin), std::move(end)));
            d->c.reset(new typename super::value_type(*(d->parent_it)));
            if (d->c->is_root()) {
                increment();
            } else {
                d->c->parent();
            }
        }

    };

    parent_iterator(parent_iterator<ParentIterator> const& other) {
        if (other.d) {
            d.reset(new data(*(other.d)));
        }
    }

    parent_iterator(parent_iterator<ParentIterator>&& other)
        : d(std::move(other.d)) {
    }

    parent_iterator& operator=(parent_iterator<ParentIterator> const& other) {
        if (other.d) {
            d.reset(new data(*(other.d)));
        } else {
            d.reset();
        }
        return *this;
    }

    parent_iterator& operator=(parent_iterator<ParentIterator>&& other) {
        d = std::move(other.d);
        return *this;
    }

    void increment() {
        ++(d->parent_it);
        if (d->parent_it != d->parent_end) {
            d->c.reset(new typename super::value_type(*(d->parent_it)));
            if (d->c->is_root()) {
                increment();
            } else {
                d->c->parent();
            }
        } else {
            d.reset();
        }
    }

    bool equal(parent_iterator const& other) const {
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
    struct data {
        data(ParentIterator parent_it,
             ParentIterator parent_end)
            : parent_it(parent_it),
              parent_end(parent_end),
              c(std::make_shared<typename super::value_type>(*parent_it)) {
        }
        data(data const& other)
            : parent_it(other.parent_it),
              parent_end(other.parent_end),
              c(std::make_shared<typename super::value_type>(*(other.c))) {
        }
        ParentIterator parent_it;
        ParentIterator parent_end;
        std::shared_ptr<typename super::value_type> c;
    };

    std::unique_ptr<data> d;
};



template <typename Range>
boost::iterator_range<parent_iterator<typename Range::iterator> >
make_parent(Range const& r) {
    return boost::make_iterator_range
            (parent_iterator<typename Range::iterator>(r.begin(), r.end()),
             parent_iterator<typename Range::iterator>());
}

}}}}

#endif //MEDIASEQUENCER_PLUGIN_UTIL_XPATH_PARENT_ITERATOR
