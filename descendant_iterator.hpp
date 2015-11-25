
//          Copyright Morten Bendiksen 2004 - 2006.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_DESCENDANT_ITERATOR
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_DESCENDANT_ITERATOR

#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <memory>

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

// An iterator over a range of context nodes. It is given another range
// of nodes and iterates through all decendants of all nodes in that
// other range.
template <typename ParentIterator>
class descendant_iterator : public boost::iterator_facade<
        descendant_iterator<ParentIterator>,
        typename ParentIterator::value_type,
        boost::forward_traversal_tag> {
private:
    typedef boost::iterator_facade<
    descendant_iterator<ParentIterator>,
    typename ParentIterator::value_type,
    boost::forward_traversal_tag> super;
public:
    descendant_iterator() {}

    descendant_iterator(ParentIterator begin, ParentIterator end) {
        if (begin != end) {
            d.reset(new data(std::move(begin), std::move(end), 0));
            increment();
        }
    };

    descendant_iterator(descendant_iterator<ParentIterator> const& other){
        if (other.d) {
            d.reset(new data(*(other.d)));
        }
    }

    descendant_iterator(descendant_iterator<ParentIterator>&& other) {
        d = std::move(other.d);
    }

    descendant_iterator& operator=(descendant_iterator<ParentIterator> const& other) {
        if (other.d) {
            d.reset(new data(*(other.d)));
        } else {
            d.reset();
        }
        return *this;
    }

    descendant_iterator& operator=(descendant_iterator<ParentIterator>&& other) {
        d = std::move(other.d);
        return *this;
    }

    void increment() {
        if (!d->c->is_null()) {
            if (d->c->has_children()) {
                d->c->first_child();
                d->depth++;
                return;
            } else if (d->depth > 0 && d->c->has_next_sibling()) {
                d->c->next_sibling();
                return;
            } else {
                while (d->depth > 0) {
                    d->c->parent();
                    d->depth--;
                    if (d->depth > 0 && d->c->has_next_sibling()) {
                        d->c->next_sibling();
                        return;
                    }
                }
            }
        }

        if (d->depth == 0) {
            ++(d->parent_it);
            if (d->parent_it != d->parent_end) {
                d->c.reset(new typename super::value_type(*d->parent_it));
                d->depth = 0;
                increment();
            } else {
                d.reset();
            }
        }
    }

    bool equal(descendant_iterator const& other) const {
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
             ParentIterator parent_end,
             int depth)
            : parent_it(parent_it),
              parent_end(parent_end),
              c(std::make_shared<typename super::value_type>(*parent_it)),
              depth(depth) {
        }

        data(data const& other)
            : parent_it(other.parent_it),
              parent_end(other.parent_end),
              c(std::make_shared<typename super::value_type>(*(other.c))),
              depth(other.depth)
        {
        }

        ParentIterator parent_it;
        ParentIterator parent_end;
        std::shared_ptr<typename super::value_type> c;
        int depth;
    };

    std::unique_ptr<data> d;
};



template <typename Range>
boost::iterator_range<descendant_iterator<typename Range::iterator> >
make_descendant(Range const& r) {
    return boost::make_iterator_range
            (descendant_iterator<typename Range::iterator>(r.begin(), r.end()),
             descendant_iterator<typename Range::iterator>());
}

}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_DESCENDANT_ITERATOR
