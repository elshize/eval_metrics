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

#include <eval_metrics.hpp>

namespace {

class MetricTest : public ::testing::TestWithParam<
                   std::tuple<int, std::vector<int>, double>> {
};

TEST_P(MetricTest, PrecisionAt)
{
    auto [k, relevance, expected] = GetParam();
    ASSERT_DOUBLE_EQ(irk::precision_at(k)(relevance), expected);
}

INSTANTIATE_TEST_CASE_P(
    binary_relevance,
    MetricTest,
    ::testing::Values(
        std::make_tuple<int, std::vector<int>, double>(1, {1, 1, 1, 0, 0, 1, 0}, 1.0),
        std::make_tuple<int, std::vector<int>, double>(2, {1, 1, 1, 0, 0, 1, 0}, 1.0),
        std::make_tuple<int, std::vector<int>, double>(3, {1, 1, 1, 0, 0, 1, 0}, 1.0),
        std::make_tuple<int, std::vector<int>, double>(4, {1, 1, 1, 0, 0, 1, 0}, 0.75),
        std::make_tuple<int, std::vector<int>, double>(5, {1, 1, 1, 0, 0, 1, 0}, 0.6),
        std::make_tuple<int, std::vector<int>, double>(6, {1, 1, 1, 0, 0, 1, 0}, 4.0 / 6.0),
        std::make_tuple<int, std::vector<int>, double>(7, {1, 1, 1, 0, 0, 1, 0}, 4.0 / 7.0),
        std::make_tuple<int, std::vector<int>, double>(8, {1, 1, 1, 0, 0, 1, 0}, 4.0 / 8.0)
    ));

INSTANTIATE_TEST_CASE_P(
    integer_relevance,
    MetricTest,
    ::testing::Values(
        std::make_tuple<int, std::vector<int>, double>(1, {1, 2, 1, -1, 0, 1, -2}, 1.0),
        std::make_tuple<int, std::vector<int>, double>(2, {1, 2, 1, -1, 0, 1, -2}, 1.0),
        std::make_tuple<int, std::vector<int>, double>(3, {1, 2, 1, -1, 0, 1, -2}, 1.0),
        std::make_tuple<int, std::vector<int>, double>(4, {1, 2, 1, -1, 0, 1, -2}, 0.75),
        std::make_tuple<int, std::vector<int>, double>(5, {1, 2, 1, -1, 0, 1, -2}, 0.6),
        std::make_tuple<int, std::vector<int>, double>(6, {1, 2, 1, -1, 0, 1, -2}, 4.0 / 6.0),
        std::make_tuple<int, std::vector<int>, double>(7, {1, 2, 1, -1, 0, 1, -2}, 4.0 / 7.0),
        std::make_tuple<int, std::vector<int>, double>(8, {1, 2, 1, -1, 0, 1, -2}, 4.0 / 8.0)
    ));

class RBPTest : public ::testing::TestWithParam<
                   std::tuple<double, std::vector<int>, double>> {
};

TEST_P(RBPTest, RBP)
{
    auto [persistance, relevance, expected] = GetParam();
    ASSERT_DOUBLE_EQ(irk::rank_biased_precision(persistance)(relevance), expected);
}

INSTANTIATE_TEST_CASE_P(
    binary_relevance,
    RBPTest,
    ::testing::Values(
        std::make_tuple<double, std::vector<int>, double>(0.9, {1, 1, 1, 0, 0, 1, 0}, 0.330049),
        std::make_tuple<double, std::vector<int>, double>(0.8, {1, 1, 1, 0, 0, 1, 0}, 0.553536),
        std::make_tuple<double, std::vector<int>, double>(0.7, {1, 1, 1, 0, 0, 1, 0}, 0.707421),
        std::make_tuple<double, std::vector<int>, double>(0.6, {1, 1, 1, 0, 0, 1, 0}, 0.815104),
        std::make_tuple<double, std::vector<int>, double>(0.5, {1, 1, 1, 0, 0, 1, 0}, 0.890625),
        std::make_tuple<double, std::vector<int>, double>(0.4, {1, 1, 1, 0, 0, 1, 0}, 0.942144),
        std::make_tuple<double, std::vector<int>, double>(0.3, {1, 1, 1, 0, 0, 1, 0}, 0.974701)
    ));

INSTANTIATE_TEST_CASE_P(
    integer_relevance,
    RBPTest,
    ::testing::Values(
        std::make_tuple<double, std::vector<int>, double>(0.9, {1, 2, 1, -1, 0, 1, -2}, 0.330049),
        std::make_tuple<double, std::vector<int>, double>(0.8, {1, 2, 1, -1, 0, 1, -2}, 0.553536),
        std::make_tuple<double, std::vector<int>, double>(0.7, {1, 2, 1, -1, 0, 1, -2}, 0.707421),
        std::make_tuple<double, std::vector<int>, double>(0.6, {1, 2, 1, -1, 0, 1, -2}, 0.815104),
        std::make_tuple<double, std::vector<int>, double>(0.5, {1, 2, 1, -1, 0, 1, -2}, 0.890625),
        std::make_tuple<double, std::vector<int>, double>(0.4, {1, 2, 1, -1, 0, 1, -2}, 0.942144),
        std::make_tuple<double, std::vector<int>, double>(0.3, {1, 2, 1, -1, 0, 1, -2}, 0.974701)
    ));

};  // namespace

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
