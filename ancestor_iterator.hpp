#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_ANCESTOR_ITERATOR
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_ANCESTOR_ITERATOR

#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <memory>

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

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

    ancestor_iterator(ParentIterator const& end)
        : parent_it(end), parent_end(end) {};

    ancestor_iterator(boost::iterator_range<ParentIterator> r)
        :parent_it(r.begin()), parent_end(r.end()), depth(0) {
        if (parent_it != parent_end) {
            c.reset(new typename super::value_type(*parent_it));
            find_next_parent();
        }

    };

    ancestor_iterator(ancestor_iterator<ParentIterator> const& other):
        parent_it(other.parent_it), parent_end(other.parent_end), depth(other.depth) {

        if (other.c)
            c.reset(new typename super::value_type(*other.c));
    }


    void increment() {
        if (!c->is_root()) {
            c->parent();
        }else {
            find_next_parent();
        }

    }

    void find_next_parent() {

        while(c->is_root()) {
            ++parent_it;
            if (parent_it != parent_end) {
                c.reset(new typename super::value_type (*parent_it));
            }
            else {
                c.reset();
                return;
            }
        }

        c->parent();

    }

    bool equal(ancestor_iterator const& other) const {
        if (!this->c || !other.c) {
            return (!this->c && !other.c);
        }
        return  *(this->c) == *(other.c) &&
                this->parent_it == other.parent_it &&
                this->parent_end == other.parent_end;
    }

    typename super::reference dereference() const {
        return *c;
    }
private:
    ParentIterator parent_it;
    ParentIterator parent_end;
    std::shared_ptr<typename super::value_type> c;
    int depth;
};



template <typename Range>
boost::iterator_range<ancestor_iterator<typename Range::iterator> >
make_ancestor(Range const& r) {
    return boost::make_iterator_range
            (ancestor_iterator<typename Range::iterator>(r),
             ancestor_iterator<typename Range::iterator>(r.end()));
}

}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_ANCESTOR_ITERATOR
