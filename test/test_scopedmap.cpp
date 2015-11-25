
//          Copyright Morten Bendiksen 2004 - 2006.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "../scopedmap.hpp"

#include <unordered_map>

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Hello
#include <boost/test/unit_test.hpp>


using namespace mediasequencer::plugin::util::xpath;

BOOST_AUTO_TEST_CASE(parent_map_does_not_contain_child_maps_elements)
{
    scopedmap<std::string, std::string> s;

    std::unordered_map<std::string, std::string> values1;
    values1["a"] = "b";
    values1["c"] = "d";
    scopedmap<std::string, std::string> s1 = s.add(values1.begin(), values1.end());

    std::unordered_map<std::string, std::string> values2;
    values2["e"] = "f";
    values2["g"] = "h";
    scopedmap<std::string, std::string> s2 = s1.add(values2.begin(), values2.end());

    std::unordered_map<std::string, std::string> values3;
    values3["i"] = "j";
    values3["k"] = "l";
    scopedmap<std::string, std::string> s3 = s.add(values3.begin(), values3.end());

    BOOST_CHECK(!s1.get("e"));
    BOOST_CHECK(!s1.get("g"));

    BOOST_CHECK(!s1.get("i"));
    BOOST_CHECK(!s1.get("k"));
}

BOOST_AUTO_TEST_CASE(child_map_contains_parent_maps_elements)
{
    scopedmap<std::string, std::string> s;

    std::unordered_map<std::string, std::string> values1;
    values1["a"] = "b";
    values1["c"] = "d";
    scopedmap<std::string, std::string> s1 = s.add(values1.begin(), values1.end());

    std::unordered_map<std::string, std::string> values2;
    values2["e"] = "f";
    values2["g"] = "h";
    scopedmap<std::string, std::string> s2 = s1.add(values2.begin(), values2.end());

    BOOST_CHECK_EQUAL(*s2.get(std::string("a")), std::string("b"));
    BOOST_CHECK_EQUAL(*s2.get(std::string("c")), std::string("d"));

}

BOOST_AUTO_TEST_CASE(leaf_maps_does_not_contain_each_others_elements)
{
    scopedmap<std::string, std::string> s;

    // root
    std::unordered_map<std::string, std::string> values;
    values["a"] = "b";
    values["c"] = "d";
    scopedmap<std::string, std::string> s0 = s.add(values.begin(), values.end());

    // line 1
    values.clear();
    values["e"] = "f";
    values["g"] = "h";
    scopedmap<std::string, std::string> s1 = s0.add(values.begin(), values.end());

    values.clear();
    values["i"] = "j";
    values["k"] = "l";
    scopedmap<std::string, std::string> s11 = s1.add(values.begin(), values.end());

    values.clear();
    values["m"] = "n";
    values["o"] = "p";
    scopedmap<std::string, std::string> s12 = s11.add(values.begin(), values.end());

    // line 2
    values.clear();
    values["q"] = "r";
    values["s"] = "t";
    scopedmap<std::string, std::string> s2 = s0.add(values.begin(), values.end());

    values.clear();
    values["u"] = "v";
    values["w"] = "x";
    scopedmap<std::string, std::string> s21 = s2.add(values.begin(), values.end());

    values.clear();
    values["y"] = "z";
    values["aa"] = "ab";
    scopedmap<std::string, std::string> s22 = s21.add(values.begin(), values.end());

    BOOST_CHECK(!s22.get(std::string("e")));
    BOOST_CHECK(!s22.get(std::string("g")));
    BOOST_CHECK(!s22.get(std::string("i")));
    BOOST_CHECK(!s22.get(std::string("k")));
    BOOST_CHECK(!s22.get(std::string("m")));
    BOOST_CHECK(!s22.get(std::string("o")));

    BOOST_CHECK(!s12.get(std::string("q")));
    BOOST_CHECK(!s12.get(std::string("s")));
    BOOST_CHECK(!s12.get(std::string("u")));
    BOOST_CHECK(!s12.get(std::string("w")));
    BOOST_CHECK(!s12.get(std::string("y")));
    BOOST_CHECK(!s12.get(std::string("aa")));

}

BOOST_AUTO_TEST_CASE(deleting_ancestor_before_adding_leaves_elements)
{
    scopedmap<std::string, std::string> s;

    std::unordered_map<std::string, std::string> values;
    values["a"] = "b";
    values["c"] = "d";
    scopedmap<std::string, std::string> s0 = s.add(values.begin(), values.end());

    scopedmap<std::string, std::string> s11;
    {

        values.clear();
        values["e"] = "f";
        values["g"] = "h";
        scopedmap<std::string, std::string> s1 = s0.add(values.begin(), values.end());

        values.clear();
        values["i"] = "j";
        values["k"] = "l";
        s11 = s1.add(values.begin(), values.end());
    }

    values.clear();
    values["m"] = "n";
    values["o"] = "p";
    scopedmap<std::string, std::string> s12 = s11.add(values.begin(), values.end());

    BOOST_CHECK_EQUAL(*s12.get(std::string("a")), std::string("b"));
    BOOST_CHECK_EQUAL(*s12.get(std::string("c")), std::string("d"));
    BOOST_CHECK_EQUAL(*s12.get(std::string("e")), std::string("f"));
    BOOST_CHECK_EQUAL(*s12.get(std::string("g")), std::string("h"));
    BOOST_CHECK_EQUAL(*s12.get(std::string("i")), std::string("j"));
    BOOST_CHECK_EQUAL(*s12.get(std::string("k")), std::string("l"));
    BOOST_CHECK_EQUAL(*s12.get(std::string("m")), std::string("n"));
    BOOST_CHECK_EQUAL(*s12.get(std::string("o")), std::string("p"));

}

BOOST_AUTO_TEST_CASE(deleting_middle_of_more_lines_does_not_screw_up_scopes)
{
    scopedmap<std::string, std::string> s;

    std::unordered_map<std::string, std::string> values;
    values["a"] = "b";
    values["c"] = "d";
    scopedmap<std::string, std::string> s0 = s.add(values.begin(), values.end());

    scopedmap<std::string, std::string> s11;
    scopedmap<std::string, std::string> s21;
    scopedmap<std::string, std::string> s22;
    {

        values.clear();
        values["a"] = "e";
        values["c"] = "f";
        scopedmap<std::string, std::string> s1 = s0.add(values.begin(), values.end());

        values.clear();
        values["a"] = "g";
        values["c"] = "h";
        s11 = s1.add(values.begin(), values.end());

        values.clear();
        values["a"] = "i";
        values["c"] = "j";
        scopedmap<std::string, std::string> s2 = s0.add(values.begin(), values.end());

        values.clear();
        values["a"] = "k";
        values["c"] = "l";
        s21 = s0.add(values.begin(), values.end());

        values.clear();
        values["a"] = "m";
        values["c"] = "n";
        s22 = s0.add(values.begin(), values.end());

    }

    values.clear();
    values["a"] = "o";
    values["c"] = "p";
    scopedmap<std::string, std::string> s12 = s11.add(values.begin(), values.end());


    values.clear();
    values["a"] = "q";
    values["c"] = "r";
    scopedmap<std::string, std::string> s23 = s22.add(values.begin(), values.end());

    BOOST_CHECK_EQUAL(*s0.get(std::string("a")), std::string("b"));
    BOOST_CHECK_EQUAL(*s0.get(std::string("c")), std::string("d"));
    BOOST_CHECK_EQUAL(*s11.get(std::string("a")), std::string("g"));
    BOOST_CHECK_EQUAL(*s11.get(std::string("c")), std::string("h"));

    BOOST_CHECK_EQUAL(*s21.get(std::string("a")), std::string("k"));
    BOOST_CHECK_EQUAL(*s21.get(std::string("c")), std::string("l"));
    BOOST_CHECK_EQUAL(*s22.get(std::string("a")), std::string("m"));
    BOOST_CHECK_EQUAL(*s22.get(std::string("c")), std::string("n"));
    BOOST_CHECK_EQUAL(*s23.get(std::string("a")), std::string("q"));
    BOOST_CHECK_EQUAL(*s23.get(std::string("c")), std::string("r"));

}

BOOST_AUTO_TEST_CASE(deleting_entire_line_does_not_screw_up_scopes)
{

    scopedmap<std::string, std::string> s21;
    scopedmap<std::string, std::string> s22;

    std::unordered_map<std::string, std::string> values;
    {

        scopedmap<std::string, std::string> s;

        values["a"] = "b";
        values["c"] = "d";
        scopedmap<std::string, std::string> s0 = s.add(values.begin(), values.end());

        values.clear();
        values["a"] = "e";
        values["c"] = "f";
        values["z"] = "a";
        scopedmap<std::string, std::string> s1 = s0.add(values.begin(), values.end());

        values.clear();
        values["a"] = "g";
        values["c"] = "h";
        values["z"] = "b";
        scopedmap<std::string, std::string> s11 = s1.add(values.begin(), values.end());

        values.clear();
        values["a"] = "o";
        values["c"] = "p";
        values["z"] = "c";
        scopedmap<std::string, std::string> s12 = s11.add(values.begin(), values.end());

        values.clear();
        values["a"] = "i";
        values["c"] = "j";
        scopedmap<std::string, std::string> s2 = s0.add(values.begin(), values.end());

        values.clear();
        values["a"] = "k";
        values["c"] = "l";
        s21 = s2.add(values.begin(), values.end());

        values.clear();
        values["a"] = "m";
        values["c"] = "n";
        s22 = s21.add(values.begin(), values.end());

    }

    values.clear();
    values["a"] = "o";
    values["c"] = "p";
    scopedmap<std::string, std::string> s23 = s22.add(values.begin(), values.end());

    values.clear();
    values["a"] = "q";
    values["c"] = "r";
    scopedmap<std::string, std::string> s24 = s23.add(values.begin(), values.end());

    values.clear();
    values["a"] = "s";
    values["c"] = "t";
    scopedmap<std::string, std::string> s3 = s21.add(values.begin(), values.end());

    values.clear();
    values["z"] = "d";
    scopedmap<std::string, std::string> s31 = s3.add(values.begin(), values.end());

    BOOST_CHECK_EQUAL(*s21.get(std::string("a")), std::string("k"));
    BOOST_CHECK_EQUAL(*s21.get(std::string("c")), std::string("l"));
    BOOST_CHECK(!s21.get(std::string("z")));

    BOOST_CHECK_EQUAL(*s22.get(std::string("a")), std::string("m"));
    BOOST_CHECK_EQUAL(*s22.get(std::string("c")), std::string("n"));
    BOOST_CHECK(!s21.get(std::string("z")));

    BOOST_CHECK_EQUAL(*s23.get(std::string("a")), std::string("o"));
    BOOST_CHECK_EQUAL(*s23.get(std::string("c")), std::string("p"));
    BOOST_CHECK(!s21.get(std::string("z")));

    BOOST_CHECK_EQUAL(*s24.get(std::string("a")), std::string("q"));
    BOOST_CHECK_EQUAL(*s24.get(std::string("c")), std::string("r"));
    BOOST_CHECK(!s21.get(std::string("z")));

    BOOST_CHECK_EQUAL(*s3.get(std::string("a")), std::string("s"));
    BOOST_CHECK_EQUAL(*s3.get(std::string("c")), std::string("t"));
    BOOST_CHECK(!s21.get(std::string("z")));

    BOOST_CHECK_EQUAL(*s31.get(std::string("z")), std::string("d"));

}

BOOST_AUTO_TEST_CASE(sibling_maps_do_not_share_elements_added_to_them_individually)
{
    scopedmap<std::string, std::string> s;

    std::unordered_map<std::string, std::string> values1;
    values1["a"] = "b";
    values1["c"] = "d";
    scopedmap<std::string, std::string> s1 = s.add(values1.begin(), values1.end());

    std::unordered_map<std::string, std::string> values2;
    values2["e"] = "f";
    values2["g"] = "h";
    scopedmap<std::string, std::string> s2 = s1.add(values2.begin(), values2.end());


    std::unordered_map<std::string, std::string> values3;
    values3["i"] = "j";
    values3["k"] = "l";
    scopedmap<std::string, std::string> s3 = s1.add(values3.begin(), values3.end());

    BOOST_CHECK(!s2.get(std::string("i")));
    BOOST_CHECK(!s2.get(std::string("k")));

    BOOST_CHECK(!s3.get(std::string("e")));
    BOOST_CHECK(!s3.get(std::string("g")));

}

BOOST_AUTO_TEST_CASE(rvalue_assign_then_delete_does_not_affect_chain)
{
    scopedmap<std::string, std::string> s;

    std::unordered_map<std::string, std::string> values1;
    values1["a"] = "b";
    values1["c"] = "d";
    scopedmap<std::string, std::string> s1 = s.add(values1.begin(), values1.end());



    scopedmap<std::string, std::string> s3;
    {
        std::unordered_map<std::string, std::string> values2;
        values2["e"] = "f";
        values2["g"] = "h";
        scopedmap<std::string, std::string> s2 = s1.add(values2.begin(), values2.end());

        std::unordered_map<std::string, std::string> values3;
        values3["i"] = "j";
        values3["k"] = "l";
        s3 = s2.add(values3.begin(), values3.end());
    }

    BOOST_CHECK_EQUAL(*s1.get(std::string("a")), std::string("b"));
    BOOST_CHECK_EQUAL(*s1.get(std::string("c")), std::string("d"));

    BOOST_CHECK_EQUAL(*s3.get(std::string("e")), std::string("f"));
    BOOST_CHECK_EQUAL(*s3.get(std::string("g")), std::string("h"));

    BOOST_CHECK_EQUAL(*s3.get(std::string("i")), std::string("j"));
    BOOST_CHECK_EQUAL(*s3.get(std::string("k")), std::string("l"));

}

BOOST_AUTO_TEST_CASE(const_ref_assign_then_delete_does_not_affect_chain)
{
    scopedmap<std::string, std::string> s;

    std::unordered_map<std::string, std::string> values1;
    values1["a"] = "b";
    values1["c"] = "d";
    scopedmap<std::string, std::string> s1 = s.add(values1.begin(), values1.end());

    scopedmap<std::string, std::string> s3;
    {
        std::unordered_map<std::string, std::string> values2;
        values2["e"] = "f";
        values2["g"] = "h";
        scopedmap<std::string, std::string> s2 = s1.add(values2.begin(), values2.end());

        std::unordered_map<std::string, std::string> values3;
        values3["i"] = "j";
        values3["k"] = "l";

        scopedmap<std::string, std::string> sTemp = s2.add(values3.begin(), values3.end());

        s3 = sTemp;
    }

    BOOST_CHECK_EQUAL(*s1.get(std::string("a")), std::string("b"));
    BOOST_CHECK_EQUAL(*s1.get(std::string("c")), std::string("d"));

    BOOST_CHECK_EQUAL(*s3.get(std::string("e")), std::string("f"));
    BOOST_CHECK_EQUAL(*s3.get(std::string("g")), std::string("h"));

    BOOST_CHECK_EQUAL(*s3.get(std::string("i")), std::string("j"));
    BOOST_CHECK_EQUAL(*s3.get(std::string("k")), std::string("l"));

}
