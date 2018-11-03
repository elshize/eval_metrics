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

#include <functional>
#include <vector>

#include <CLI/CLI.hpp>

#include <irm.hpp>

int main(int argc, char** argv)
{
    std::string qrels_file;
    std::string results_file;
    std::vector<std::string> metrics = {
        "P@10",
        "P@20",
        "P@30",
        "P@50",
        "P@100",
        "P@200",
        "P@500",
        "P@1000",
        "RBP:95"};

    CLI::App app{"Evaluate search results with IR metrics."};
    app.add_option("-m,--metric", metrics, "List of metrics", true);
    app.add_option("qrels", qrels_file, "Query relevance data in TREC format")
        ->required()
        ->check(CLI::ExistingFile);
    app.add_option("results", results_file, "Query results in TREC format")
        ->required()
        ->check(CLI::ExistingFile);
    CLI11_PARSE(app, argc, argv);

    auto qrels = irm::read_trec_rels(qrels_file);
    auto results = irm::read_trec_results(results_file);

    std::vector<irm::metric_t> metric_functions;
    std::transform(std::begin(metrics),
        std::end(metrics),
        std::back_inserter(metric_functions),
        irm::parse_metric);

    auto annotated = irm::annotate(results, qrels);
    for (auto& [run_id, run_results] : annotated) {
        for (auto& [iteration, iter_results] : run_results) {
            std::vector<std::vector<double>> metric_values(metrics.size());
            for (auto& [query, results] :iter_results) {
                std::vector<int> relevance_range;
                std::transform(
                    std::begin(results),
                    std::end(results),
                    std::back_inserter(relevance_range),
                    [](const auto& r) -> int { return r.relevance; });
                for (int m = 0; m < metrics.size(); m++)
                {
                    metric_values[m].push_back(metric_functions[m](relevance_range));
                }
            }
            for (int m = 0; m < metrics.size(); m++)
            {
                double avg = std::accumulate(std::begin(metric_values[m]),
                                 std::end(metric_values[m]),
                                 0.0)
                    / metric_values[m].size();
                std::cout << run_id << '\t' << iteration << '\t' << metrics[m]
                          << '\t' << avg << std::endl;
            }
        }
    }

    return 0;
}
