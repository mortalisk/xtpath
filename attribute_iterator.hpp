/*
 *   Copyright 2013 Morten Bendiksen (morten.bendiksen@gmail.com)
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */
#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_ATTRIBUTE_ITERATOR_HPP
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_ATTRIBUTE_ITERATOR_HPP

#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <memory>

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

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

    attribute_iterator(boost::iterator_range<ParentIterator> r)
        :parent_it(r.begin()), parent_end(r.end()) {
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
            (attribute_iterator<typename Range::iterator>(r),
             attribute_iterator<typename Range::iterator>(r.end()));
}

}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_ATTRIBUTE_ITERATOR_HPP
