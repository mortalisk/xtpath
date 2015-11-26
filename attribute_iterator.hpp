
//          Copyright Morten Bendiksen 2014 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_ATTRIBUTE_ITERATOR_HPP
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_ATTRIBUTE_ITERATOR_HPP

#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <memory>

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

// An iterator over a range of attributes. It is given a range
// of nodes and iterates through all attributes of all nodes in that
// other range.
template <typename ParentIterator>
class attribute_iterator : public boost::iterator_facade<
        attribute_iterator<ParentIterator>,
        std::pair<std::string, std::string>,
        boost::forward_traversal_tag,
        std::pair<std::string, std::string> > {
private:
    typedef typename ParentIterator::value_type::AttributeIterator AttributeIterator;
public:
    attribute_iterator() {}

    attribute_iterator(ParentIterator const& end)
        : parent_it(end), parent_end(end) {}

    attribute_iterator(ParentIterator begin, ParentIterator end)
        :parent_it(begin), parent_end(end) {
        if (parent_it != parent_end) {
            auto attributes = (*parent_it).attributes();
            attribute_it = attributes.begin();
            attribute_end = attributes.end();
            if (attribute_it == attribute_end) {
                go_to_next();
            }
        }
    }

    attribute_iterator(attribute_iterator<ParentIterator> const& other):
        parent_it(other.parent_it), parent_end(other.parent_end),
        attribute_it(other.attribute_it), attribute_end(other.attribute_end) {
    }


    void increment() {
        go_to_next();
    }

    void go_to_next() {
        if (attribute_it != attribute_end)
            ++attribute_it;
        while (attribute_it == attribute_end) {
            ++parent_it;
            if (parent_it != parent_end) {
                auto attributes = (*parent_it).attributes();
                attribute_it = attributes.begin();
                attribute_end = attributes.end();
            } else {
                attribute_it = AttributeIterator();
                attribute_end = AttributeIterator();
                return;
            }
        }
    }

    bool equal(attribute_iterator const& other) const {
        return  this->attribute_it == other.attribute_it &&
                this->attribute_end == other.attribute_end &&
                this->parent_it == other.parent_it &&
                this->parent_end == other.parent_end;
    }

    std::pair<std::string, std::string> dereference() const {
        return *attribute_it;
    }
private:
    ParentIterator parent_it;
    ParentIterator parent_end;
    AttributeIterator attribute_it;
    AttributeIterator attribute_end;
};

template <typename Range>
boost::iterator_range<attribute_iterator<typename Range::iterator> >
make_attributes(Range const& r) {
    return boost::make_iterator_range
            (attribute_iterator<typename Range::iterator>(r.begin(), r.end()),
             attribute_iterator<typename Range::iterator>(r.end()));
}

}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_ATTRIBUTE_ITERATOR_HPP
