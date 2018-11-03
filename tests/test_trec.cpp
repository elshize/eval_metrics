// MIT License
//
// Copyright (c) 2018 Michal Siedlaczek
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

//! \file
//! \author Michal Siedlaczek
//! \copyright MIT License

#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <irm/trec.hpp>

namespace {

using tup =
    std::tuple<std::string, std::string, std::string, int, double, std::string>;

auto trec_to_tuple(const irm::trec_result& result)
{
    return std::make_tuple(
        result.query_id,
        result.iteration,
        result.document_id,
        result.rank,
        result.score,
        result.run_id);
}

auto rel_to_tuple(const irm::trec_rel& result)
{
    return std::make_tuple(
        result.query_id,
        result.iteration,
        result.document_id,
        result.relevance);
}

class TrecRel : public ::testing::Test {
    irm::trec_rel rec = irm::trec_rel::from("q0 i0 ZF08-175-870 2");
};

TEST_F(TrecRel, Read)
{
    irm::trec_rel rec = irm::trec_rel::from("q0 i0 ZF08-175-870 2");
    auto expected = std::make_tuple("q0", "i0", "ZF08-175-870", 2);
    ASSERT_THAT(rel_to_tuple(rec), ::testing::Eq(expected));
}

TEST_F(TrecRel, ReadInvalidRelevance)
{
    EXPECT_THROW(
        irm::trec_rel::from("q0 i0 ZF08-175-870 invalid_rel"),
        irm::trec_format_error);
}

TEST_F(TrecRel, ReadTooManyFields)
{
    EXPECT_THROW(
        irm::trec_rel::from("q0 i0 ZF08-175-870 2 superfluous"),
        irm::trec_format_error);
}

TEST_F(TrecRel, ReadTooFewFields)
{
    EXPECT_THROW(irm::trec_rel::from("q0 i0 ZF08-175-870"),
        irm::trec_format_error);
}

class TrecResult : public ::testing::Test {
public:
    using map_type =
        std::unordered_map<std::string, std::vector<irm::trec_result>>;
    std::vector<irm::trec_result> results {
        {"030", "Q0", "ZF08-175-870", 0, 4238, "R0"},
        {"031", "Q0", "ZF08-175-870", 0, 4238, "R0"},
        {"040", "Q0", "ZF08-175-870", 0, 4238, "R0"},
        {"040", "Q0", "ZF08-175-871", 1, 4238, "R0"},
        {"040", "Q0", "ZF08-175-872", 2, 4238, "R0"},
        {"040", "Q0", "ZF08-175-873", 3, 4238, "R0"},
        {"040", "Q0", "ZF08-175-874", 4, 4238, "R0"},
        {"000", "Q0", "ZF08-175-870", 0, 4238, "R0"}
    };
    map_type expected_groups = {
        {"030", {{"030", "Q0", "ZF08-175-870", 0, 4238, "R0"}}},
        {"031", {{"031", "Q0", "ZF08-175-870", 0, 4238, "R0"}}},
        {"040",
            {{"040", "Q0", "ZF08-175-870", 0, 4238, "R0"},
             {"040", "Q0", "ZF08-175-871", 1, 4238, "R0"},
             {"040", "Q0", "ZF08-175-872", 2, 4238, "R0"},
             {"040", "Q0", "ZF08-175-873", 3, 4238, "R0"},
             {"040", "Q0", "ZF08-175-874", 4, 4238, "R0"}}},
        {"000", {{"000", "Q0", "ZF08-175-870", 0, 4238, "R0"}}}};
};

TEST_F(TrecResult, Read)
{
    irm::trec_result rec = irm::trec_result::from(
        "030 Q0 ZF08-175-870 0 4238 R0");
    auto expected = std::make_tuple(
        "030", "Q0", "ZF08-175-870", 0, 4238.0, "R0");
    ASSERT_THAT(trec_to_tuple(rec), ::testing::Eq(expected));
}

TEST_F(TrecResult, ReadInvalidRank)
{
    EXPECT_THROW(
        irm::trec_result::from("030 Q0 ZF08-175-870 invalid_rank 4238 R0"),
        irm::trec_format_error);
}

TEST_F(TrecResult, ReadInvalidScore)
{
    EXPECT_THROW(
        irm::trec_result::from("030 Q0 ZF08-175-870 0 invalid_score R0"),
        irm::trec_format_error);
}

TEST_F(TrecResult, ReadTooManyFields)
{
    EXPECT_THROW(
        irm::trec_result::from("030 Q0 ZF08-175-870 0 4238 R0 superfluous"),
        irm::trec_format_error);
}

TEST_F(TrecResult, ReadTooFewFields)
{
    EXPECT_THROW(irm::trec_result::from("030 Q0 ZF08-175-870 0 4238"),
        irm::trec_format_error);
}

MATCHER(TrecResultEq, "") {
    return trec_to_tuple(::testing::get<0>(arg))
        == trec_to_tuple(::testing::get<1>(arg));
}

TEST_F(TrecResult, GroupByQuery)
{
    auto groups = irm::group_by_query(results);
    ASSERT_EQ(groups.size(), expected_groups.size());
    for (const auto[key, group] : groups) {
        auto expected = expected_groups[key];
        EXPECT_THAT(group, ::testing::Pointwise(TrecResultEq(), expected));
    }
}

}  // namespace

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
