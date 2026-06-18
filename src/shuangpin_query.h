#pragma once

#include <chrono>
#include <string>
#include <tuple>
#include <vector>

namespace shuangpin {

using QueryRow = std::tuple<std::string, std::string, int>;

struct QueryResult {
    std::string shuangpin;
    std::string segmentation;
    std::string quanpin_segmentation;
    std::string normalized_quanpin;
    std::string table;
    std::string sql;
    bool needs_filtering = false;
    std::vector<QueryRow> rows;
};

struct QueryProfile {
    std::chrono::steady_clock::duration total{};
    std::chrono::steady_clock::duration segment_elapsed{};
    std::chrono::steady_clock::duration normalize_elapsed{};
    std::chrono::steady_clock::duration db_open_elapsed{};
    std::chrono::steady_clock::duration db_query_elapsed{};
    std::chrono::steady_clock::duration filter_elapsed{};
    int row_count = 0;
};

struct ProfiledQueryResult {
    QueryResult result;
    QueryProfile profile;
};

std::string SegmentInput(const std::string& raw_input);
std::string ToQuanpinSegmentation(const std::string& segmented_input);
std::string NormalizeInput(const std::string& raw_input);

ProfiledQueryResult QueryWordsProfiled(
    const std::string& shuangpin,
    const std::string& db_path,
    int limit = 80);

std::vector<ProfiledQueryResult> QueryWordsProfiledMany(
    const std::vector<std::string>& shuangpins,
    const std::string& db_path,
    int limit = 80);

const std::string& DefaultDbPath();
const std::string& DefaultAppName();

}  // namespace shuangpin
