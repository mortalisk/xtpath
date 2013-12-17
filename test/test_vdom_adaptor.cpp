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
#include "../vdom_adaptor.hpp"

#include <mediasequencer/vdom/treeutil.hpp>
#include <mediasequencer/vdom/xml_convert.hpp>

#include <soul/unittest/test_macros.hpp>


SUITE_BEGIN(vdom_context_test, "Tests for the vdom xpath");

using namespace mediasequencer::plugin::util::xpath;

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

TEST(singleton_range_vdom)
{
    std::string xml(
            "<a xmlns=\"a:a\" xmlns:foo=\"b:b\">"
                "<b></b>"
                "<b></b>"
            "</a>");
    mediasequencer::vdom::scoped_document sdoc;
    mediasequencer::vdom::document doc = sdoc.document();
    mediasequencer::vdom::node root =
            mediasequencer::vdom::treeutil::xml_to_element(doc, xml);

    auto node_range = vdomContext(root);
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


TEST(namespace_selector_vdom)
{
    std::string xml(
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
    mediasequencer::vdom::scoped_document sdoc;
    mediasequencer::vdom::document doc = sdoc.document();
    mediasequencer::vdom::node root =
            mediasequencer::vdom::treeutil::xml_to_element(doc, xml);

    auto node_range = vdomContext(root);
    auto result_range =
            node_range | descendant | where( ns("a:d") || ns("a:b"));

    std::vector<std::string> expected_names = {"b", "x", "s"};

    CHECK_EXPECTED_NAMES_RANGE_EQUAL_NODE_RANGE_NAMES(expected_names, result_range);

}

TEST(attribute_selector_vdom)
{
    std::string xml(
            "<a>"
                "<z a=\"b\" b=\"c\"/>"
                "<b d=\"f\" c=\"d\"/>"
            "</a>");
    mediasequencer::vdom::scoped_document sdoc;
    mediasequencer::vdom::document doc = sdoc.document();
    mediasequencer::vdom::node root =
            mediasequencer::vdom::treeutil::xml_to_element(doc, xml);

    auto node_range = vdomContext(root);
    auto result_range =
            node_range | child | attribute;

    std::vector<std::pair<std::string, std::string> > expected_attributes =
        {
            std::make_pair("b","c"),
            std::make_pair("a","b"),
            std::make_pair("d","f"),
            std::make_pair("c","d")
        };

    auto result_it = result_range.begin();
    for (auto pair: expected_attributes) {
        BOOST_REQUIRE_MESSAGE(result_it != result_range.end(), "too few results");
        BOOST_CHECK_EQUAL(pair.first, (*result_it).first);
        BOOST_CHECK_EQUAL(pair.second, (*result_it).second);
        ++result_it;
    }

}

TEST(text_selector_contains)
{
    std::string xml(
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
    mediasequencer::vdom::scoped_document sdoc;
    mediasequencer::vdom::document doc = sdoc.document();
    mediasequencer::vdom::node root =
            mediasequencer::vdom::treeutil::xml_to_element(doc, xml);

    auto node_range = vdomContext(root);
    auto mosExternalMetadataContext
            = node_range |descendant("item") | child("mosExternalMetadata") |
                    where(child("mosSchema") | text_contains("mosObj/embedded"))
                    | first;
    BOOST_REQUIRE(mosExternalMetadataContext.get_node());
    BOOST_CHECK_EQUAL(std::string("mosExternalMetadata"),
                      std::string(mosExternalMetadataContext.get_node().name()));
}

TEST(mam_namespace_redeclaration_vdom)
{
    std::string xml(
                "<entry xmlns='http://www.w3.org/2005/Atom'>"
                  "<id>http://tnopc3:1973/availability/1100111208021396421</id>"
                  "<title type='text'/>"
                  "<updated>2013-08-18T15:51:18Z</updated>"
                  "<author>"
                    "<email>VVHBridge</email>"
                  "</author>"
                  "<link rel='self' href='http://tnopc3:1973/availability/1100111208021396421'/>"
                  "<link rel='edit' href='http://tnopc3:1973/availability/1100111208021396421'/>"
                  "<link rel='asset' type='application/atom+xml;type=entry' href='http://tnopc3:1973/assets/1100111208021396421'/>"
                  "<content type='application/vnd.vizrt.availability+xml'>"
                    "<availability xmlns='http://www.vizrt.com/2010/mam'>"
                      "<distributionplan href='http://tnopc3:1973/publish/distributionplan/186fee02cec44707acf41bdfea824fa1'/>"
                      "<asset ref='http://tnopc3:1973/assets/1100111208021396421' href='http://tnopc3:1973/assets/1100111208021396421'/>"
                      "<publishedderivative>"
                        "<publishingpoint href='http://tnopc3:1973/publish/publishingpoint/vizroom1'></publishingpoint>"
                        "<state description='Available'>available</state>"
                        "<derivativefilename>DUMMYFILE</derivativefilename>"
                        "<content type='application/mxf' url='http://dummy/' xmlns='http://search.yahoo.com/mrss/'></content>"
                      "</publishedderivative>"
                    "</availability>"
                  "</content>"
                "</entry>");
    mediasequencer::vdom::scoped_document sdoc;
    mediasequencer::vdom::document doc = sdoc.document();
    mediasequencer::vdom::node root =
            mediasequencer::vdom::treeutil::xml_to_element(doc, xml);

    auto node_range = singleton(vdomContext(root));
    auto availability
            = node_range | descendant("availability") |
                where(ns("http://www.vizrt.com/2010/mam"));
    BOOST_CHECK_EQUAL(std::string("availability"),
                      std::string(availability | name | first));
}

SUITE_END();

