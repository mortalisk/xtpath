
//          Copyright Morten Bendiksen 2014 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_CHILDREN_ITERATOR
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_CHILDREN_ITERATOR

#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <memory>

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

// An iterator over a range of context nodes. It is given another range
// of nodes and iterates through all children of all nodes in that
// other range.
template <typename ParentIterator>
class child_iterator : public boost::iterator_facade<
        child_iterator<ParentIterator>,
        typename ParentIterator::value_type,
        boost::forward_traversal_tag> {
private:
    typedef boost::iterator_facade<
    child_iterator<ParentIterator>,
    typename ParentIterator::value_type,
    boost::forward_traversal_tag> super;
public:
    child_iterator() {}

    child_iterator(ParentIterator begin, ParentIterator end) {
        if (begin != end) {
            d.reset(new data(std::move(begin), std::move(end)));
            reset();
        }
    }

    void reset() {
        assert(d);
        if (d->parent_it != d->parent_end) {
            d->c.reset(new typename super::value_type(*(d->parent_it)));
            if (!d->c->is_null() && d->c->has_children()) {
                d->c->first_child();
            } else {
                ++(d->parent_it);
                reset();
            }
        } else {
            d.reset();
        }
    }

    child_iterator(child_iterator<ParentIterator> const& other) {
        if (other.d) {
            d.reset(new data(*(other.d)));
        }
    }

    child_iterator(child_iterator<ParentIterator>&& other)
        : d(std::move(other.d)) {
    }

    child_iterator<ParentIterator>&
    operator=(child_iterator<ParentIterator> const& other) {
        if (other.d) {
            d.reset(new data(*(other.d)));
        } else {
            d.reset();
        }
        return *this;
    }

    child_iterator<ParentIterator>&
    operator=(child_iterator<ParentIterator>&& other) {
        d = std::move(other.d);
        return *this;
    }

    void increment() {
        if (d->c && !d->c->is_null() && d->c->has_next_sibling()) {
            d->c->next_sibling();
        } else {
            ++(d->parent_it);
            reset();
        }
    }

    bool equal(child_iterator const& other) const {
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
boost::iterator_range<child_iterator<typename Range::iterator> >
make_children(Range const& r) {
    return boost::make_iterator_range
            (child_iterator<typename Range::iterator>(r.begin(), r.end()),
             child_iterator<typename Range::iterator>());
}

}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_CHILDREN_ITERATOR
