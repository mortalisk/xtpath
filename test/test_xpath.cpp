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
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/distance.hpp>

#include "../pugi_adaptor.hpp"


#include <pugixml.hpp>

#include <vector>


#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Hello
#include <boost/test/unit_test.hpp>

using namespace mediasequencer::plugin::util::xpath;

struct xml_fixture {

    std::string xml;
    pugi::xml_document document;
    xml_fixture(std::string xml) : xml(xml) {
        std::istringstream iss(xml);
        auto status = document.load(iss);
        BOOST_REQUIRE_MESSAGE(status, "Parsing error: " << status.description());
    }

    pugi::xml_node root() {
        return document.root().first_child();
    }
};

template<typename ExpectedRange, typename NodeRange>
void CHECK_EXPECTED_NAMES_RANGE_EQUAL_NODE_RANGE_NAMES(ExpectedRange expected_names, NodeRange nodes) {
    auto iterator = nodes.begin();
    for (auto& expected: expected_names) {
        BOOST_REQUIRE(iterator != nodes.end());
        auto& node = (*iterator).get_node();
        BOOST_REQUIRE(node);
        BOOST_CHECK_EQUAL(node.name(), expected);
        ++iterator;
    }

    BOOST_CHECK(iterator == nodes.end());
}


BOOST_AUTO_TEST_CASE(singleton_range)
{
    xml_fixture xml_fixture(
            "<a xmlns=\"a:a\" xmlns:foo=\"b:b\">"
                "<b></b>"
                "<b></b>"
            "</a>");
    pugi::xml_node root = xml_fixture.root();

    auto node_range = singleton(context(root));
    auto it = node_range.begin();

    BOOST_CHECK_EQUAL((*it).get_node(), root);

    it++;

    BOOST_CHECK(it == node_range.end());

    // the range should be reuseable
    it = node_range.begin();

    BOOST_CHECK_EQUAL((*it).get_node(), root);

    it++;

    BOOST_CHECK(it == node_range.end());

}

BOOST_AUTO_TEST_CASE(children_range)
{
    xml_fixture xml_fixture(
            "<a xmlns=\"a:a\" xmlns:foo=\"b:b\">"
                "<b><d/></b>"
                "<c><e/></c>"
            "</a>");
    pugi::xml_node root = xml_fixture.root();

    auto node_range = singleton(context(root));

    auto result_range = make_children(node_range);

    std::vector<std::string> expected_names = {"b", "c"};
    CHECK_EXPECTED_NAMES_RANGE_EQUAL_NODE_RANGE_NAMES(expected_names, result_range);

}


BOOST_AUTO_TEST_CASE(reuse_range_of_children)
{
    xml_fixture xml_fixture(
            "<a xmlns=\"a:a\" xmlns:foo=\"b:b\">"
                "<b><d/></b>"
                "<c><e/></c>"
            "</a>");
    pugi::xml_node root = xml_fixture.root();

    auto node_range = singleton(context(root));

    auto children = make_children(node_range);
    std::vector<std::string> expected_names = {"b", "c"};
    CHECK_EXPECTED_NAMES_RANGE_EQUAL_NODE_RANGE_NAMES(expected_names, children);

    auto children_of_children = make_children(children);
    expected_names = {"d", "e"};
    CHECK_EXPECTED_NAMES_RANGE_EQUAL_NODE_RANGE_NAMES(expected_names, children_of_children);
}

BOOST_AUTO_TEST_CASE(copmposit_calls_to_children)
{
    xml_fixture xml_fixture(
            "<a xmlns=\"a:a\" xmlns:foo=\"b:b\">"
                "<b><d/></b>"
                "<c><e/></c>"
            "</a>");
    pugi::xml_node root = xml_fixture.root();

    auto node_range = singleton(context(root));
    auto result_range = make_children(
                make_children(node_range));

    std::vector<std::string> expected_names = {"d", "e"};

    CHECK_EXPECTED_NAMES_RANGE_EQUAL_NODE_RANGE_NAMES(expected_names, result_range);
}

BOOST_AUTO_TEST_CASE(four_levels_of_nested_children)
{
    xml_fixture xml_fixture(
            "<a>"
                "<z/>"
                "<b>"
                    "<d>"
                        "<h>"
                            "<i></i>"
                            "<j></j>"
                        "</h>"
                    "</d>"
                "</b>"
                "<y/>"
                "<c>"
                    "<e>"
                        "<f>"
                            "<g></g>"
                            "<k></k>"
                        "</f>"
                    "</e>"
                "</c>"
            "</a>");
    pugi::xml_node root = xml_fixture.root();

    auto node_range = singleton(context(root));
    auto result_range =
            make_children(
                make_children(
                    make_children(
                        make_children(node_range))));


    std::vector<std::string> expected_names = {"i", "j", "g", "k"};

    CHECK_EXPECTED_NAMES_RANGE_EQUAL_NODE_RANGE_NAMES(expected_names, result_range);
}

BOOST_AUTO_TEST_CASE(pipe_operator_and_filter)
{
    xml_fixture xml_fixture(
            "<a>"
                "<z/>"
                "<b>"
                    "<d>"
                        "<h>"
                            "<i></i>"
                            "<j></j>"
                        "</h>"
                    "</d>"
                "</b>"
                "<y/>"
                "<c>"
                    "<e>"
                        "<f>"
                            "<g></g>"
                            "<k></k>"
                        "</f>"
                    "</e>"
                    "<e>"
                        "<f>"
                            "<g></g>"
                            "<k></k>"
                        "</f>"
                    "</e>"
                    "<c>"
                        "<g>"
                            "<g></g>"
                            "<k></k>"
                        "</g>"
                    "</c>"
                "</c>"
            "</a>");
    pugi::xml_node root = xml_fixture.root();

    auto node_range = singleton(context(root));
    auto result_range =
            node_range | child("c") | child("e") | child("f") | child("g");

    std::vector<std::string> expected_names = {"g", "g"};

    CHECK_EXPECTED_NAMES_RANGE_EQUAL_NODE_RANGE_NAMES(expected_names, result_range);
}

BOOST_AUTO_TEST_CASE(descendants_of_node)
{
    xml_fixture xml_fixture(
            "<a>"
                "<z/>"
                "<b>"
                    "<d>"
                        "<h>"
                            "<i></i>"
                            "<j></j>"
                        "</h>"
                    "</d>"
                "</b>"
                "<y/>"
                "<c>"
                    "<e>"
                        "<f>"
                            "<g></g>"
                            "<k></k>"
                        "</f>"
                    "</e>"
                    "<e>"
                        "<f>"
                            "<g></g>"
                            "<k></k>"
                        "</f>"
                    "</e>"
                    "<c>"
                        "<g>"
                            "<g></g>"
                            "<k></k>"
                        "</g>"
                    "</c>"
                "</c>"
            "</a>");
    pugi::xml_node root = xml_fixture.root();

    auto node_range = singleton(context(root));
    auto result_range =
           node_range | child | descendant;

    std::vector<std::string> expected_names = {"d", "h", "i", "j",
                                              "e", "f", "g", "k",
                                              "e", "f", "g", "k",
                                              "c", "g", "g", "k"};

    CHECK_EXPECTED_NAMES_RANGE_EQUAL_NODE_RANGE_NAMES(expected_names, result_range);
}


BOOST_AUTO_TEST_CASE(ancectors_of_node)
{
    xml_fixture xml_fixture(
            "<a>"
                "<z/>"
                "<b>"
                    "<d>"
                        "<h>"
                            "<i></i>"
                            "<j></j>"
                        "</h>"
                    "</d>"
                "</b>"
                "<y/>"
                "<c>"
                    "<e>"
                        "<f>"
                            "<g></g>"
                            "<k></k>"
                        "</f>"
                    "</e>"
                    "<e>"
                        "<f>"
                            "<g></g>"
                            "<k></k>"
                        "</f>"
                    "</e>"
                    "<c>"
                        "<g>"
                            "<g></g>"
                            "<k></k>"
                        "</g>"
                    "</c>"
                "</c>"
            "</a>");
    pugi::xml_node root = xml_fixture.root();
    pugi::xml_node leaf = root.child("b").child("d").child("h").child("i");

    auto node_range = singleton(context(leaf));
    auto result_range =
            node_range | ancestor;

    std::vector<std::string> expected_names = {"h", "d", "b", "a"};

    CHECK_EXPECTED_NAMES_RANGE_EQUAL_NODE_RANGE_NAMES(expected_names, result_range);
}


BOOST_AUTO_TEST_CASE(ancectors_of_range)
{
    xml_fixture xml_fixture(
            "<a>"
                "<z/>"
                "<b>"
                    "<d/>"
                "</b>"
                "<y/>"
                "<f>"
                    "<d/>"
                "</f>"
            "</a>");
    pugi::xml_node root = xml_fixture.root();

    auto node_range = singleton(context(root));
    auto result_range =
            node_range | child | child ("d") | ancestor;

    std::vector<std::string> expected_names = {"b", "a", "f", "a"};

    CHECK_EXPECTED_NAMES_RANGE_EQUAL_NODE_RANGE_NAMES(expected_names, result_range);
}

BOOST_AUTO_TEST_CASE(parents_of_node)
{
    xml_fixture xml_fixture(
            "<a>"
                "<z/>"
                "<b>"
                    "<d>"
                        "<h>"
                            "<i></i>"
                            "<j></j>"
                        "</h>"
                    "</d>"
                "</b>"
                "<y/>"
                "<c>"
                    "<e>"
                        "<f>"
                            "<g></g>"
                            "<k></k>"
                        "</f>"
                    "</e>"
                    "<e>"
                        "<f>"
                            "<g></g>"
                            "<k></k>"
                        "</f>"
                    "</e>"
                    "<c>"
                        "<g>"
                            "<g></g>"
                            "<k></k>"
                        "</g>"
                    "</c>"
                "</c>"
            "</a>");
    pugi::xml_node root = xml_fixture.root();
    pugi::xml_node leaf = root.child("b").child("d").child("h").child("i");

    auto node_range = singleton(context(leaf));
    auto result_range =
            node_range | parent;

    std::vector<std::string> expected_names = {"h"};

    CHECK_EXPECTED_NAMES_RANGE_EQUAL_NODE_RANGE_NAMES(expected_names, result_range);
}


BOOST_AUTO_TEST_CASE(parents_of_range)
{
    xml_fixture xml_fixture(
            "<a>"
                "<z/>"
                "<b>"
                    "<d/>"
                "</b>"
                "<y/>"
                "<f>"
                    "<d/>"
                "</f>"
            "</a>");
    pugi::xml_node root = xml_fixture.root();

    auto node_range = singleton(context(root));
    auto result_range =
            node_range | child | child ("d") | parent;

    std::vector<std::string> expected_names = {"b", "f"};

    CHECK_EXPECTED_NAMES_RANGE_EQUAL_NODE_RANGE_NAMES(expected_names, result_range);
}

BOOST_AUTO_TEST_CASE(where_clause_child)
{
    xml_fixture xml_fixture(
            "<a>"
                "<z/>"
                "<b>"
                    "<x>"
                        "<y/>"
                    "</x>"
                "</b>"
                "<y/>"
                "<f>"
                    "<d/>"
                "</f>"
                "<z/>"
                "<d>"
                    "<x>"
                        "<y/>"
                    "</x>"
                "</d>"
                "<y/>"
                "<f>"
                    "<d/>"
                "</f>"
            "</a>");
    pugi::xml_node root = xml_fixture.root();

    auto node_range = singleton(context(root));
    auto result_range =
            node_range | child | where(child("x") | child("y"));

    std::vector<std::string> expected_names = {"b", "d"};

    CHECK_EXPECTED_NAMES_RANGE_EQUAL_NODE_RANGE_NAMES(expected_names, result_range);
}

BOOST_AUTO_TEST_CASE(where_clause_descendant_parent)
{
    xml_fixture xml_fixture(
            "<a>"
                "<z/>"
                "<b>"
                    "<x>"
                        "<y/>"
                    "</x>"
                "</b>"
                "<y/>"
                "<f>"
                    "<d/>"
                "</f>"
                "<z/>"
                "<d>"
                    "<x>"
                        "<y/>"
                    "</x>"
                "</d>"
                "<y/>"
                "<f>"
                    "<d/>"
                "</f>"
            "</a>");
    pugi::xml_node root = xml_fixture.root();

    auto node_range = singleton(context(root));
    auto result_range =
            node_range | child | where(descendant("y") | parent("x"));

    std::vector<std::string> expected_names = {"b", "d"};

    CHECK_EXPECTED_NAMES_RANGE_EQUAL_NODE_RANGE_NAMES(expected_names, result_range);
}


BOOST_AUTO_TEST_CASE(where_clause_with_or)
{
    xml_fixture xml_fixture(
            "<a>"
                "<z/>"
                "<b>"
                    "<x>"
                        "<s/>"
                    "</x>"
                "</b>"
                "<y/>"
                "<f>"
                    "<d/>"
                "</f>"
                "<z/>"
                "<d>"
                    "<y>"
                        "<t/>"
                    "</y>"
                "</d>"
                "<y/>"
                "<f>"
                    "<d/>"
                "</f>"
            "</a>");
    pugi::xml_node root = xml_fixture.root();

    auto node_range = singleton(context(root));
    auto result_range =
            node_range | descendant | where( (parent("x")| child("s")) || parent("y") | child("t"));

    std::vector<std::string> expected_names = {"s", "t"};


    CHECK_EXPECTED_NAMES_RANGE_EQUAL_NODE_RANGE_NAMES(expected_names, result_range);

}


BOOST_AUTO_TEST_CASE(namespace_selector_test)
{
    xml_fixture xml_fixture(
            "<a xmlns:foo=\"a:a\">"
                "<z/>"
                "<b xmlns=\"a:b\">"
                    "<x xmlns:foo=\"a:c\">"
                        "<s xmlns=\"a:d\"/>"
                    "</x>"
                "</b>"
                "<y/>"
                "<f>"
                    "<d/>"
                "</f>"
                "<z/>"
                "<d>"
                    "<y>"
                        "<t/>"
                    "</y>"
                "</d>"
                "<y/>"
                "<f>"
                    "<d/>"
                "</f>"
            "</a>");
    pugi::xml_node root = xml_fixture.root();

    auto node_range = singleton(context(root));
    auto result_range =
            node_range | descendant | where( ns("a:d") || ns("a:b"));

    std::vector<std::string> expected_names = {"b", "x", "s"};

    CHECK_EXPECTED_NAMES_RANGE_EQUAL_NODE_RANGE_NAMES(expected_names, result_range);

}

BOOST_AUTO_TEST_CASE(attribute_selector)
{
    xml_fixture xml_fixture(
            "<a>"
                "<z a=\"b\" b=\"c\"/>"
                "<b foo=\"bar\" bar=\"baz\"/>"
            "</a>");
    pugi::xml_node root = xml_fixture.root();

    auto node_range = singleton(context(root));
    auto result_range =
            node_range | child | attribute;

    std::vector<std::pair<std::string, std::string> > expected_attributes =
        {
            std::make_pair("a","b"),
            std::make_pair("b","c"),
            std::make_pair("foo","bar"),
            std::make_pair("bar","baz")
        };

    auto result_it = result_range.begin();
    for (auto pair: expected_attributes) {
        BOOST_REQUIRE_MESSAGE(result_it != result_range.end(), "too few results");
        BOOST_CHECK_EQUAL(pair.first, (*result_it).first);
        BOOST_CHECK_EQUAL(pair.second, (*result_it).second);
        ++result_it;
    }
    BOOST_CHECK(result_it == result_range.end());

}

BOOST_AUTO_TEST_CASE(attribute_selector_name)
{
    xml_fixture xml_fixture(
            "<a>"
                "<z a=\"b\" b=\"c\"/>"
                "<b b=\"bar\" bar=\"baz\"/>"
            "</a>");
    pugi::xml_node root = xml_fixture.root();

    auto node_range = singleton(context(root));
    auto result_range =
            node_range | child | attribute("b");

    std::vector<std::string> expected_attributes =
        {
            "c",
            "bar"
        };

    auto result_it = result_range.begin();
    for (auto expected: expected_attributes) {
        BOOST_REQUIRE_MESSAGE(result_it != result_range.end(), "too few results");
        BOOST_CHECK_EQUAL(expected, *result_it);
        ++result_it;
    }
    BOOST_CHECK(result_it == result_range.end());
}

BOOST_AUTO_TEST_CASE(attribute_selector_name_and_value)
{
    xml_fixture xml_fixture(
            "<a>"
                "<x a=\"b\" b=\"c\"/>"
                "<y b=\"bar\" bar=\"baz\"/>"
                "<z a=\"foo\" b=\"ar\"/>"
                "<b b=\"baz\" bar=\"bar\"/>"
                "<y b=\"bar\" bar=\"baz\"/>"
            "</a>");
    pugi::xml_node root = xml_fixture.root();

    auto node_range = singleton(context(root));
    auto result_range =
            node_range | child | (attribute("b", "ar") || attribute("b", "bar"));

    std::vector<std::string> expected_node_names =
        {
        "z", "y", "y"
        };

    CHECK_EXPECTED_NAMES_RANGE_EQUAL_NODE_RANGE_NAMES(expected_node_names, result_range);

}

BOOST_AUTO_TEST_CASE(text_selector)
{
    xml_fixture xml_fixture(
            "<a>"
                "<z>foo</z>"
                "<b>bar</b>"
                "<z>baz</z>"
            "</a>");
    pugi::xml_node root = xml_fixture.root();

    auto node_range = singleton(context(root));
    auto result_range =
            node_range | child("z") | text;

    std::vector<std::string> expected_text =
        {
            "foo",
            "baz"
        };

    auto result_it = result_range.begin();
    for (auto expected: expected_text) {
        BOOST_REQUIRE_MESSAGE(result_it != result_range.end(), "too few results");
        BOOST_CHECK_EQUAL(expected, *result_it);
        ++result_it;
    }
    BOOST_CHECK(result_it == result_range.end());
}

BOOST_AUTO_TEST_CASE(text_selector_contains)
{
    xml_fixture xml_fixture(
                "<gateway from_actor_type=\"VIZGATEWAY-client\">"
                  "<mos>"
                    "<mosID>aircache.newscenter.com</mosID>"
                    "<ncsID>ncs.newscenter.com</ncsID>"
                    "<roCreate>"
                      "<roID>3</roID>"
                      "<roSlug>Embeded mos object test</roSlug>"
                      "<roEdStart>1999-04-17T17:02:00</roEdStart>"
                      "<roEdDur>00:58:25</roEdDur>"
                      "<story>"
                        "<storyID>1</storyID>"
                        "<storySlug>Story1</storySlug>"
                        "<storyNum>A5</storyNum>"
                        "<item>"
                          "<itemID>0</itemID>"
                          "<itemSlug>An embeded mos object</itemSlug>"
                          "<objID>EMBEDDED</objID>"
                          "<mosID>aircache.newscenter.com</mosID>"
                          "<itemEdStart>150</itemEdStart>"
                          "<itemEdDur>300</itemEdDur>"
                          "<itemUserTimingDur>310</itemUserTimingDur>"
                          "<itemTrigger>CHAINED</itemTrigger>"
                          "<mosExternalMetadata>"
                            "<mosScope>PLAYLIST</mosScope>"
                            "<mosSchema>http://www.vizrt.com/mosObj/embedded</mosSchema>"
                            "<mosPayload>"
                              "<embedded_item>"
                                "<external_id>1</external_id>"
                                "<fields>"
                                  "<field>James Bond</field>"
                                  "<field>Secret Agent</field>"
                                "</fields>"
                              "</embedded_item>"
                            "</mosPayload>"
                          "</mosExternalMetadata>"
                        "</item>"
                      "</story>"
                    "</roCreate>"
                  "</mos>"
                "</gateway>");
    pugi::xml_node root = xml_fixture.root();

    auto node_range = singleton(context(root));
    auto mosExternalMetadataContext
            = node_range |descendant("item") | child("mosExternalMetadata") |
                    where(child("mosSchema") | text_contains("mosObj/embedded"))
                    | first;
    BOOST_REQUIRE(mosExternalMetadataContext.get_node());
    BOOST_CHECK_EQUAL(std::string("mosExternalMetadata"),
                      std::string(mosExternalMetadataContext.get_node().name()));
}

int count_ds(range<_context<PugiXmlAdaptor> > r) {
  return boost::distance( r | child("d"));
}

BOOST_AUTO_TEST_CASE(type_erasure)
{
    xml_fixture xml_fixture(
            "<a>"
                "<b><d/><d/></b>"
                "<c><d/><d/><d/></c>"
		"<e><d/></e>"
		"<d/>"
		"<f><d/></f>"
            "</a>");
    pugi::xml_node root = xml_fixture.root();

    auto node_range = singleton(context(root));
    auto result_range =
            node_range | child("c");

    int count = count_ds(result_range);

    BOOST_CHECK_EQUAL(count, 3);
}


