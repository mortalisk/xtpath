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
#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_SINGLETON_ITERATOR
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_SINGLETON_ITERATOR

#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/iterator_range.hpp>
#include <memory>

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

template <typename Element>
class singleton_iterator : public boost::iterator_facade<singleton_iterator<Element>,
       Element, boost::forward_traversal_tag> {
public:
    singleton_iterator() {};

    singleton_iterator(singleton_iterator const& other)
        : context(other.context) {};

    explicit singleton_iterator(Element&& context)
        : context(new Element(std::move(context))) {}
    explicit singleton_iterator(Element const& context)
        : context(new Element(context)) {}

private:
    friend class boost::iterator_core_access;

    void increment() {
        context.reset();
    }

    bool equal(singleton_iterator const& other) const {
        return this->context == other.context;
    }

    Element& dereference() const {
        return *context;
    }

    std::shared_ptr<Element> context;
};

template <typename Element>
boost::iterator_range
   <singleton_iterator<typename std::remove_reference<Element>::type> >
singleton(Element&& n) {
    return boost::make_iterator_range
            (singleton_iterator<typename std::remove_reference<Element>::type>(std::forward<Element>(n)),
             singleton_iterator<typename std::remove_reference<Element>::type>());

}

}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_SINGLETON_ITERATOR
