#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_DESCENDANT_ITERATOR
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_DESCENDANT_ITERATOR

#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <memory>

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

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

    descendant_iterator(ParentIterator const& end)
        : parent_it(end), parent_end(end) {};

    descendant_iterator(boost::iterator_range<ParentIterator> r)
        :parent_it(r.begin()), parent_end(r.end()), depth(0) {
        if (parent_it != parent_end) {
            c.reset(new typename super::value_type(*parent_it));
            if (c->has_children()) {
                c->first_child();
                depth++;
            } else {
                find_next_leaf();
            }
        }

    };

    descendant_iterator(descendant_iterator<ParentIterator> const& other):
        parent_it(other.parent_it), parent_end(other.parent_end), depth(other.depth) {

        if (other.c)
            c.reset(new typename super::value_type(*other.c));
    }

    void increment() {
        if (c->has_children()) {
            c->first_child();
            ++depth;
        }else if (c->has_next_sibling()){
            c->next_sibling();
        }else {
            find_next_leaf();
        }

    }

    void find_next_leaf() {

        while(!c->has_next_sibling() && depth > 0) {
            c->parent();
            --depth;
        }
        if (depth > 0) {
            c->next_sibling();
        } else {
            ++parent_it;
            while (parent_it != parent_end) {
                c.reset(new typename super::value_type(*parent_it));
                if (!c->has_children()) {
                    parent_it++;
                } else {
                    c->first_child();
                    ++depth;
                    return;
                }
            }
            c.reset();
        }

    }

    bool equal(descendant_iterator const& other) const {
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
boost::iterator_range<descendant_iterator<typename Range::iterator> >
make_descendant(Range const& r) {
    return boost::make_iterator_range
            (descendant_iterator<typename Range::iterator>(r),
             descendant_iterator<typename Range::iterator>(r.end()));
}

}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_DESCENDANT_ITERATOR
