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
//! \author     Michal Siedlaczek
//! \copyright  MIT License

#pragma once

#include <numeric>

#include <boost/iterator/iterator_facade.hpp>

namespace irk {

using std::begin;
using std::end;
using std::min;
using std::move;
using std::next;
using std::ptrdiff_t;

struct identity {
    template<class T>
    constexpr T&& operator()(T&& t) const noexcept
    {
        return std::forward<T>(t);
    }
};

template<class T>
struct series {
public:
    series() = delete;
    series(std::function<T(ptrdiff_t)> f) : f_(std::move(f)) {}
    class iterator : public boost::iterator_facade<iterator,
                         T,
                         boost::forward_traversal_tag,
                         const T&> {
    public:
        iterator() = delete;
        iterator(std::function<T(ptrdiff_t)> f, ptrdiff_t n = 0)
            : f_(std::move(f)), n_(n) {};

    private:
        std::function<T(ptrdiff_t)> f_;
        ptrdiff_t n_ = 0;
        mutable T cur_{};

        friend class boost::iterator_core_access;
        void increment() { ++n_; }
        bool equal(const iterator& other) const { return n_ == other.n_; }
        const T& dereference() const { return cur_ = f_(n_); }
    };

    iterator begin() const { return iterator(f_); };
    iterator end() const
    {
        return iterator(f_, std::numeric_limits<ptrdiff_t>::max());
    };
    ptrdiff_t size() const { return std::numeric_limits<ptrdiff_t>::max(); }

private:
    std::function<T(ptrdiff_t)> f_;
};

template<class WeightRange, class RelevanceTransform = identity>
struct weighted_precision {
public:
    weighted_precision(WeightRange weights,
        ptrdiff_t cutoff = std::numeric_limits<int>::max(),
        RelevanceTransform relevance_transform = identity{})
        : weights_(move(weights)),
          cutoff_(cutoff),
          relevance_transform_(std::move(relevance_transform))
    {}

    template<class RelevanceRange>
    double operator()(const RelevanceRange& relevance) const
    {
        return this->operator()(begin(relevance), end(relevance));
    }

    template<class RelevanceIterator>
    double operator()(RelevanceIterator first, RelevanceIterator last) const
    {
        ptrdiff_t cutoff = min(cutoff_, distance(first, last));
        cutoff = min(cutoff, static_cast<ptrdiff_t>(weights_.size()));
        return std::inner_product(begin(weights_),
            next(begin(weights_), cutoff),
            first,
            0.0,
            std::plus<>(),
            [this](const auto& lhs, const auto& rhs) {
                return lhs * relevance_transform_(rhs);
            });
    }

private:
    const WeightRange weights_{};
    ptrdiff_t cutoff_{};
    RelevanceTransform relevance_transform_{};
};

inline auto precision_at(int k)
{
    return weighted_precision(
        std::vector<double>(k, 1.0 / k),
        k,
        [](const auto& r) { return r > 0 ? 1 : 0; });
};

inline auto rank_biased_precision(double persistance)
{
    return weighted_precision(
        series<double>{[persistance](ptrdiff_t n) {
            return (1.0 - persistance) * std::pow(persistance, n);
        }},
        std::numeric_limits<int>::max(),
        [](const auto& r) { return r > 0 ? 1 : 0; });
};

} // namespace irk
