#pragma once

#include <chrono>
#include <functional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace quanpin {

using Segments = std::vector<std::string>;
using QueryItem = std::pair<std::string, int>;

struct QueryResultEntry {
    Segments segments;
    std::string key;
    std::string table;
    std::vector<QueryItem> items;
};

struct QueryResult {
    std::string quanpin;
    std::string mode;
    std::vector<QueryResultEntry> results;
};

struct QuerySqlProfile {
    std::string label;
    std::string lookup_value;
    std::chrono::steady_clock::duration elapsed{};
    int row_count = 0;
};

struct QueryCutProfile {
    Segments segments;
    std::string table;
    std::string key;
    std::chrono::steady_clock::duration elapsed{};
    std::vector<QuerySqlProfile> sql_profiles;
};

struct QueryProfile {
    std::chrono::steady_clock::duration total{};
    std::chrono::steady_clock::duration cut_elapsed{};
    std::chrono::steady_clock::duration dedupe_elapsed{};
    std::chrono::steady_clock::duration db_open_elapsed{};
    std::chrono::steady_clock::duration db_query_elapsed{};
    size_t raw_cut_count = 0;
    size_t unique_cut_count = 0;
    std::vector<QueryCutProfile> cut_profiles;
};

struct ProfiledQueryResult {
    QueryResult result;
    QueryProfile profile;
};

struct FuzzyRule {
    std::string source;
    std::string target;
    bool enabled;
};

template <typename T>
struct TimedQueryResult {
    T value;
    std::chrono::steady_clock::duration elapsed;
};

template <typename Func, typename... Args>
auto MeasureQueryTime(Func&& func, Args&&... args)
{
    using ResultType = std::invoke_result_t<Func, Args...>;

    const auto start = std::chrono::steady_clock::now();
    if constexpr (std::is_void_v<ResultType>) {
        std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
        const auto end = std::chrono::steady_clock::now();
        return end - start;
    }

    auto value = std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
    const auto end = std::chrono::steady_clock::now();

    return TimedQueryResult<ResultType>{std::move(value), end - start};
}

std::vector<Segments> CutQuanpinGreedy(const std::string& quanpin,
                                      bool is_intact = false,
                                      bool is_break = true);

std::vector<Segments> CutQuanpinByMode(
    const std::string& quanpin,
    const std::string& mode = "greedy",
    const std::vector<FuzzyRule>& fuzzy_rules = {},
    bool is_break = true);

QueryResult QueryWords(
    const std::string& quanpin,
    const std::string& db_path,
    const std::string& mode = "greedy",
    int limit = 8);

ProfiledQueryResult QueryWordsProfiled(
    const std::string& quanpin,
    const std::string& db_path,
    const std::string& mode = "greedy",
    int limit = 8);

std::vector<ProfiledQueryResult> QueryWordsProfiledMany(
    const std::vector<std::string>& quanpins,
    const std::string& db_path,
    const std::string& mode = "greedy",
    int limit = 8);

std::vector<QueryItem> QueryWordsFlat(
    const std::string& quanpin,
    const std::string& db_path,
    const std::string& mode = "greedy",
    int limit = 8);

const std::vector<FuzzyRule>& DefaultFuzzyRules();
const std::string& DefaultDbPath();

}  // namespace quanpin
