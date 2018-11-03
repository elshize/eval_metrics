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

#include <fstream>
#include <functional>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace irm {

using std::begin;
using std::end;
using std::min;
using std::move;
using std::next;
using std::ptrdiff_t;

template<class T>
using group_t = std::map<std::string, T>;

class trec_format_error : public std::exception {
private:
    std::string what_;

    std::string init_what(const char* what) const noexcept
    {
        std::ostringstream out;
        out << "Error reading TREC format: " << what;
        return std::move(out.str());
    }

public:
    explicit trec_format_error(const std::string& what)
        : what_(init_what(what.c_str()))
    {}
    explicit trec_format_error(const char* what) : what_(init_what(what)) {}
    const char* what() const noexcept override { return what_.c_str(); }
};

class trec_result {
private:
    static void parse_field(trec_result& rec, std::string field, int field_num)
    {
        switch (field_num) {
        case 0:
            rec.query_id = std::move(field);
            break;
        case 1:
            rec.iteration = std::move(field);
            break;
        case 2:
            rec.document_id = std::move(field);
            break;
        case 3:
            try {
                rec.rank = std::stoi(field);
            } catch (...) {
                throw trec_format_error("cannot parse rank");
            }
            break;
        case 4:
            try {
                rec.score = std::stod(field);
            } catch (...) {
                throw trec_format_error("cannot parse score");
            }
            break;
        case 5:
            rec.run_id = std::move(field);
            break;
        default:
            throw trec_format_error("too many fields");
        }
    }
public:
    std::string query_id;
    std::string iteration;
    std::string document_id;
    int rank;
    double score;
    std::string run_id;
    int relevance = 0;

    trec_result() = default;
    trec_result(const trec_result&) = default;
    trec_result(trec_result&&) = default;
    trec_result& operator=(const trec_result&) = default;
    trec_result& operator=(trec_result&&) = default;
    ~trec_result() = default;

    static trec_result from(const std::string& result_string)
    {
        trec_result rec;
        std::istringstream in(result_string);
        std::string field;
        int fields_read = 0;
        while (in >> field)
        {
            parse_field(rec, field, fields_read);
            fields_read += 1;
        }
        if (fields_read < 6) { throw trec_format_error("too few fields"); }
        return rec;
    }
};

class trec_rel {
private:
    static void parse_field(trec_rel& rec, std::string field, int field_num)
    {
        switch (field_num) {
        case 0:
            rec.query_id = std::move(field);
            break;
        case 1:
            rec.iteration = std::move(field);
            break;
        case 2:
            rec.document_id = std::move(field);
            break;
        case 3:
            try {
                rec.relevance = std::stoi(field);
            } catch (...) {
                throw trec_format_error("cannot parse relevance");
            }
            break;
        default:
            throw trec_format_error("too many fields");
        }
    }
public:
    std::string query_id;
    std::string iteration;
    std::string document_id;
    int relevance;

    trec_rel() = default;
    trec_rel(const trec_rel&) = default;
    trec_rel(trec_rel&&) = default;
    trec_rel& operator=(const trec_rel&) = default;
    trec_rel& operator=(trec_rel&&) = default;
    ~trec_rel() = default;

    static trec_rel from(const std::string& rel_string)
    {
        trec_rel rec;
        std::istringstream in(rel_string);
        std::string field;
        int fields_read = 0;
        while (in >> field)
        {
            parse_field(rec, field, fields_read);
            fields_read += 1;
        }
        if (fields_read < 4) { throw trec_format_error("too few fields"); }
        return rec;
    }
};

inline std::vector<trec_rel> read_trec_rels(const std::string& filename)
{
    std::vector<trec_rel> rels;
    std::ifstream in(filename);
    std::string line;
    while (std::getline(in, line))
    {
        rels.push_back(std::move(trec_rel::from(line)));
    }
    return rels;
}

inline std::vector<trec_result> read_trec_results(const std::string& filename)
{
    std::vector<trec_result> results;
    std::ifstream in(filename);
    std::string line;
    while (std::getline(in, line))
    {
        results.push_back(std::move(trec_result::from(line)));
    }
    return results;
}

struct group_key_hash : public std::unary_function<
                            std::tuple<std::string, std::string, std::string>,
                            std::size_t> {
    std::size_t
    operator()(const std::tuple<std::string, std::string, std::string>& t) const
    {
        return std::hash<std::string>{}(
            std::get<0>(t) + " " + std::get<1>(t) + " " + std::get<2>(t));
    }
};

template<class R>
inline auto group_by_query(std::vector<R>& records)
{
    std::unordered_map<std::string, std::vector<R>> map;
    for (R& record : records) {
        map[record.query_id].push_back(std::move(record));
    }
    return map;
}

template<class R>
inline auto group(std::vector<R>& records)
{
    group_t<group_t<group_t<std::vector<R>>>> map;
    for (R& record : records) {
        map[record.run_id][record.iteration][record.query_id].push_back(
            std::move(record));
    }
    return map;
}

inline void annotate_single(std::vector<trec_result>& results,
                            const std::vector<trec_rel>& rels)
{
    std::unordered_map<std::string, int> relmap;
    for (const auto& rel : rels) { relmap[rel.document_id] = rel.relevance; }
    for (auto& result : results) {
        result.relevance = relmap[result.document_id];
    }
}

inline auto
annotate(std::vector<trec_result>& results, std::vector<trec_rel>& rels)
{
    auto grouped_results = group(results);
    auto grouped_rels = group_by_query(rels);
    for (auto& [run, results_for_run] : grouped_results)
    {
        for (auto& [iteration, results_for_iteration] : results_for_run)
        {
            for (auto& [query, results_for_query] : results_for_iteration) {
                annotate_single(results_for_query, grouped_rels[query]);
            }
        }
    }
    return grouped_results;
}

}  // namespace irm
