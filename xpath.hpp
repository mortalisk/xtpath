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

#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic push
#include <boost/range/join.hpp>
#pragma GCC diagnostic pop

namespace mediasequencer { namespace plugin { namespace util { namespace xpath {

template <typename Left, typename Right>
struct piped_expression {
    Left left;
    Right right;

    piped_expression(Left left, Right right): left(left), right(right) {

    }
};

template <typename Left, typename Right>
struct or_expression {
    Left left;
    Right right;

    or_expression(Left left, Right right): left(left), right(right) {

    }
};

template<typename Left, typename Right>
or_expression<Left, Right>
operator || (Left left, Right right) {
    return or_expression<Left, Right>(left, right);
}

template<typename Range, typename Left, typename Right>
auto
operator | (Range range, or_expression<Left, Right> const& or_expression)
-> decltype(join(range|or_expression.left, range|or_expression.right))
{
    return join(range|or_expression.left, range|or_expression.right);

}

template <typename Left, typename Right>
struct is_expr<piped_expression<Left, Right> >: std::true_type {
};


template <typename Left, typename Right,
          typename = typename std::enable_if<is_expr<Left>::value>::type>
piped_expression<Left, Right> operator|(Left left, Right right) {
    return piped_expression<Left, Right>(left, right);
}

template <typename Expression>
class _where {
public:
    Expression e;
    _where(Expression e): e(e) {

    }

};

template <typename Expression>
class _where_not {
public:
    Expression e;
    _where_not(Expression e): e(e) {

    }

};

template <typename Expression>
_where<Expression> where(Expression e) {
    return _where<Expression>(e);
}

template <typename Expression>
_where_not<Expression> where_not(Expression e) {
    return _where_not<Expression>(e);
}

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

template<typename Range, typename Left, typename Right,
         typename = typename boost::range_iterator<Range>::type>
auto
operator|(Range r, piped_expression<Left, Right> e)
-> decltype((r | std::move(e.left)) | std::move(e.right))
{
    return (r | std::move(e.left)) | std::move(e.right);
}

template <typename Range, typename Expression>
auto
operator|(Range const& r, _where<Expression> w)
-> decltype(r | boost::adaptors::filtered(
                where_predicate<Expression, typename Range::iterator::reference>(w.e)))
{
    return r | boost::adaptors::filtered(
                where_predicate<Expression, typename Range::iterator::reference>(w.e));

}

template <typename Range, typename Expression>
auto
operator|(Range const& r, _where_not<Expression> w)
-> decltype(r | boost::adaptors::filtered(
                where_not_predicate<Expression, typename Range::iterator::reference>(w.e)))
{
    return r | boost::adaptors::filtered(
                where_not_predicate<Expression, typename Range::iterator::reference>(w.e));

}

template <typename Context>
struct contextToNode {
    typename Context::node_type operator()(Context const& context) const {
        return context.get_node();
    }
    typedef typename Context::node_type result_type;
};

struct _toNode {

};

struct _first {

};

namespace {
    const _toNode toNode;
    const _first first;
}

template <typename Range>
auto
operator|(Range const& range, _toNode toNode)
-> decltype(range | transformed(
                contextToNode<typename Range::iterator::value_type>()))
{
    return range | transformed(
                contextToNode<typename Range::iterator::value_type>());
}

template <typename Range>
typename Range::iterator::value_type
operator|(Range range, _first)
{
    return
            range.begin() != range.end() ?
                *(range.begin()) :
                typename Range::iterator::value_type();
}

}}}}

#endif // MEDIASEQUENCER_PLUGIN_UTIL_XPATH_XPATH_HPP
