
//          Copyright Morten Bendiksen 2004 - 2006.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef MEDIASEQUENCER_PLUGIN_UTIL_XPATH_XPATH_HPP
#define MEDIASEQUENCER_PLUGIN_UTIL_XPATH_XPATH_HPP


#include "context.hpp"
#include "singleton_iterator.hpp"
#include "child_selector.hpp"
#include "ancestor_selector.hpp"
#include "descendant_selector.hpp"
#include "parent_selector.hpp"
#include "name_selector.hpp"
#include "namespace_selector.hpp"
#include "attribute_selector.hpp"
#include "text_selector.hpp"

#include <boost/range/join.hpp>

namespace mediasequencer { namespace plugin {
/// Namespace for helpfull libraries
namespace util {
/// Namespace for xtpath. See \ref xtpath for usage.
namespace xpath {

// Used to capture the left and right side of the pipe operator
// inide a subexpression (i.e. one used inside where() or where_not())
template <typename Left, typename Right>
struct piped_expression {
    Left left;
    Right right;

    piped_expression(Left left, Right right): left(left), right(right) {

    }
};

// Used to capture the left and right side of the double pipe operator
// inide a subexpression (i.e. one used inside where() or where_not())
template <typename Left, typename Right>
struct or_expression {
    Left left;
    Right right;

    or_expression(Left left, Right right): left(left), right(right) {

    }
};


// Implements the double pipe operator for subexpressions, e.g.
// range | where ( child("foo") || attribute("bar")). Returns an
// or_expression which captures the left and right side of the
// subexpression.
template<typename Left, typename Right,
         typename = typename std::enable_if<is_expr<Left>::value>::type>
or_expression<Left, Right>
operator || (Left const& left, Right const& right) {
    return or_expression<Left, Right>(left, right);
}

// Used to evaluate subexpressions that were created with the
// double pipe operator, i.e. captured in an or_expression. Called
// from the where_predicate or where_not_predicate.
template<typename Range, typename Left, typename Right>
boost::range::joined_range
<const decltype(std::declval<const Range>()|std::declval<const Left>()),
 const decltype(std::declval<const Range>()|std::declval<const Right>())>
operator | (Range const& range, or_expression<Left, Right> const& or_expression)
{
    return join(range|or_expression.left, range|or_expression.right);

}

// Enables expressions with the pipe operator to be considered
// as subexpressions (e.g. to be passed to where() and where_not())
template <typename Left, typename Right>
struct is_expr<piped_expression<Left, Right> >: std::true_type {
};

// Enables expressions with the double pipe operator to be considered
// as subexpressions (e.g. to be passed to where() and where_not())
template <typename Left, typename Right>
struct is_expr<or_expression<Left, Right> >: std::true_type {
};

// constructs piped_expressions from sub expressions using the pipe
// operator
template <typename Left, typename Right,
          typename = typename std::enable_if<is_expr<Left>::value>::type>
piped_expression<Left, Right> operator|(Left left, Right right) {
    return piped_expression<Left, Right>(left, right);
}

// used to identify where() expressions as well as capture the subexpression
template <typename Expression>
class _where {
public:
    Expression e;
    _where(Expression e): e(e) {

    }

};

// used to identify where_not() expressions as well as capture the subexpression
template <typename Expression>
class _where_not {
public:
    Expression e;
    _where_not(Expression e): e(e) {

    }

};

/// Constructs a subexpression that can be used to filter
/// a range to remove entries that do not satisfy the subexpression.
/// E.g. 'range | where (child("foo"))', will filter away nodes
/// in the range that do not have a child with the name "foo".
// Returns an instance of _where that captures the expression,
// and is used later to construct the where_predicate given to
// the filtered_range.
template <typename Expression>
_where<Expression> where(Expression e) {
    return _where<Expression>(e);
}

/// Constructs a subexpression that can be used to filter
/// a range to remove entries that satisfy the subexpression.
/// E.g. 'range | where_not (child("foo"))', will filter away nodes
/// in the range that have a child with the name "foo".
// Returns an instance of _where_not that captures the expression,
// and is used later to construct the where_not_predicate given to
// the filtered_range.
template <typename Expression>
_where_not<Expression> where_not(Expression e) {
    return _where_not<Expression>(e);
}

// The predicate given to the boost filtered_range, when
// evaluating a where() subexpression.
template <typename Expression, typename Input>
class where_predicate {
public:
    Expression e;
    where_predicate(Expression e) : e(e){

    }

    bool operator()(Input i) const {
        auto range =  singleton(i) |e ;
        return range.begin() != range.end();
    }
};

// The predicate given to the boost filtered_range, when
// evaluating a where_not() subexpression.
template <typename Expression, typename Input>
class where_not_predicate {
public:
    Expression e;
    where_not_predicate(Expression e) : e(e){

    }

    bool operator()(Input i) const {
        auto range =  singleton(i) |e ;
        return range.begin() == range.end();
    }
};

// Used to evaluate subexpressions that were created with the
// pipe operator, i.e. captured in a piped_expression. Called
// from the where_predicate or where_not_predicate.
template<typename Range, typename Left, typename Right,
         typename = typename boost::range_iterator<Range>::type>
auto
operator|(Range const& r, piped_expression<Left, Right> e)
-> decltype((r | std::move(e.left)) | std::move(e.right))
{
    return (r | std::move(e.left)) | std::move(e.right);
}

// Implements the | operator for expressions using
// 'where' sub-expressions, e.g. 'range | where(child("foo"))'
// Captures the expression inside the where() in a where_predicate,
// Returns a filtered_range which uses the where_predicate as the
// predicate to filter on.
template <typename Range, typename Expression>
boost::range_detail::filtered_range
<where_predicate<Expression, typename Range::iterator::reference>,
 const Range>
operator|(Range const& r, _where<Expression> w)
{
    return r | boost::adaptors::filtered(
                where_predicate<Expression, typename Range::iterator::reference>(w.e));

}

// Implements the | operator for expressions using
// 'where_not' sub-expressions, e.g. 'range | where_not(child("foo"))'
// Captures the expression inside the where() in a where_not_predicate,
// Returns a filtered_range which uses the where_not_predicate as the
// predicate to filter on.
template <typename Range, typename Expression>
boost::range_detail::filtered_range
<where_not_predicate<Expression, typename Range::iterator::reference>,
 const Range>
operator|(Range const& r, _where_not<Expression> w)
{
    return r | boost::adaptors::filtered(
                where_not_predicate<Expression, typename Range::iterator::reference>(w.e));

}

// transformation used for boost::transformed_range, to transform from
// a range of context to a range of the underlying DOM nodes.
template <typename Context>
struct contextToNode {
    typename Context::node_type operator()(Context const& context) const {
        return context.get_node();
    }
    typedef typename Context::node_type result_type;
};

// Type for the toNode selector
struct _toNode { };

// Type for the first selector
struct _first { };

namespace {
    /// Selects the node from the underlying DOM implementation,
    /// If this is used, your code will likely not be
    /// directly usable with other DOM implementations without
    /// a few changes.
    const _toNode toNode;

    /// Selects the first instance in the range, or the default
    /// constructed object of the value_type from the range.
    /// Example: 'range | first'. Returns a context-node if the
    /// child, ancestor, parent or similar selectors were the
    /// previous selector. Returns a string if any of the text
    /// selectors were used.
    const _first first;
}

// Implements the '|' operator in sub-expressions with the 'toNode'
// selector, e.g. 'range | toNode'. Returns a transformed_range with
// contextToNode as the transformation.
template <typename Range>
boost::range_detail::transformed_range
<contextToNode<typename Range::iterator::value_type>,
 const Range>
operator|(Range const& range, _toNode toNode)
{
    return range | transformed(
                contextToNode<typename Range::iterator::value_type>());
}


// Implements the '|' operator in sub-expressions with the 'first'
// selector, e.g. 'range | first'.
template <typename Range>
typename Range::iterator::value_type
operator|(Range const& range, _first)
{
    return
            range.begin() != range.end() ?
                *(range.begin()) :
                typename Range::iterator::value_type();
}

}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_XPATH_HPP
