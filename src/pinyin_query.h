#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace pinyin {

using Segments = std::vector<std::string>;
using QueryItem = std::pair<std::string, int>;

struct QueryResultEntry {
    Segments segments;
    std::string key;
    std::string table;
    std::vector<QueryItem> items;
};

struct QueryResult {
    std::string pinyin;
    std::string mode;
    std::vector<QueryResultEntry> results;
};

struct FuzzyRule {
    std::string source;
    std::string target;
    bool enabled;
};

std::vector<Segments> CutPinyinGreedy(const std::string& pinyin,
                                      bool is_intact = false,
                                      bool is_break = true);

std::vector<Segments> CutPinyinByMode(
    const std::string& pinyin,
    const std::string& mode = "greedy",
    const std::vector<FuzzyRule>& fuzzy_rules = {},
    bool is_break = true);

QueryResult QueryWords(
    const std::string& pinyin,
    const std::string& db_path,
    const std::string& mode = "greedy",
    int limit = 8);

std::vector<QueryItem> QueryWordsFlat(
    const std::string& pinyin,
    const std::string& db_path,
    const std::string& mode = "greedy",
    int limit = 8);

const std::vector<FuzzyRule>& DefaultFuzzyRules();
const std::string& DefaultDbPath();

}  // namespace pinyin
