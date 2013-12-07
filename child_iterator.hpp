#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_CHILDREN_ITERATOR
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_CHILDREN_ITERATOR

#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <memory>

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

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

    child_iterator(ParentIterator const& end)
        : parent_it(end), parent_end(end) {}

    child_iterator(boost::iterator_range<ParentIterator> r)
        :parent_it(r.begin()), parent_end(r.end()) {
        if (parent_it != parent_end) {
            c.reset(new typename super::value_type(*parent_it));
            c->first_child();
            find_next_if_no_node();
        }
    }

    child_iterator(child_iterator<ParentIterator> const& other):
        parent_it(other.parent_it), parent_end(other.parent_end) {

        if (other.c)
            c.reset(new typename super::value_type(*other.c));
    }


    void increment() {
        c->next_sibling();
        find_next_if_no_node();
    }

    void find_next_if_no_node() {
        while (!c->get_node()) {
            ++parent_it;
            if ( parent_it == parent_end)
                break;
            c.reset(new typename super::value_type(*parent_it));
            c->first_child();
        }
        if(!c->get_node()) {
            c.reset();
        }
    }

    bool equal(child_iterator const& other) const {
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
};

template <typename Range>
boost::iterator_range<child_iterator<typename Range::iterator> >
make_children(Range const& r) {
    return boost::make_iterator_range
            (child_iterator<typename Range::iterator>(r),
             child_iterator<typename Range::iterator>(r.end()));
}

}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_CHILDREN_ITERATOR
