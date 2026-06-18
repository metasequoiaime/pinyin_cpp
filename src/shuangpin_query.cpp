#include "shuangpin_query.h"

#include <fmt/core.h>
#include <sqlite3.h>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <memory>
#include <regex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace shuangpin
{
namespace
{

constexpr const char *kDefaultAppName = "MetasequoiaImeTsf";
constexpr const char *kDefaultDbName = "cutted_flyciku_with_jp.db";

using Segments = std::vector<std::string>;

const std::vector<std::string> &IntactQuanpinList()
{
    static const std::vector<std::string> kList = {
        "a",     "ai",    "an",   "ang",   "ao",    "ba",   "bai",   "ban",   "bang",  "bao",  "bei",   "ben",   "beng",   "bi",    "bian",  "biao", "bie",   "bin",  "bing",  "bo",    "bu",     "ca",   "cai",  "can",  "cang", "cao",  "ce",   "cen",  "ceng", "cha",  "chai",  "chan",
        "chang", "chao",  "che",  "chen",  "cheng", "chi",  "chong", "chou",  "chu",   "chua", "chuai", "chuan", "chuang", "chui",  "chun",  "chuo", "ci",    "cong", "cou",   "cu",    "cuan",   "cui",  "cun",  "cuo",  "da",   "dai",  "dan",  "dang", "dao",  "de",   "dei",   "den",
        "deng",  "di",    "dia",  "dian",  "diao",  "die",  "ding",  "diu",   "dong",  "dou",  "du",    "duan",  "dui",    "dun",   "duo",   "e",    "ei",    "en",   "er",    "fa",    "fan",    "fang", "fei",  "fen",  "feng", "fiao", "fo",   "fou",  "fu",   "ga",   "gai",   "gan",
        "gang",  "gao",   "ge",   "gei",   "gen",   "geng", "gong",  "gou",   "gu",    "gua",  "guai",  "guan",  "guang",  "gui",   "gun",   "guo",  "ha",    "hai",  "han",   "hang",  "hao",    "he",   "hei",  "hen",  "heng", "hong", "hou",  "hu",   "hua",  "huai", "huan",  "huang",
        "hui",   "hun",   "huo",  "ji",    "jia",   "jian", "jiang", "jiao",  "jie",   "jin",  "jing",  "jiong", "jiu",    "ju",    "juan",  "jue",  "jun",   "jve",  "ka",    "kai",   "kan",    "kang", "kao",  "ke",   "kei",  "ken",  "keng", "kong", "kou",  "ku",   "kua",   "kuai",
        "kuan",  "kuang", "kui",  "kun",   "kuo",   "la",   "lai",   "lan",   "lang",  "lao",  "le",    "lei",   "leng",   "li",    "lia",   "lian", "liang", "liao", "lie",   "lin",   "ling",   "liu",  "lo",   "long", "lou",  "lu",   "luan", "lue",  "lun",  "luo",  "lv",    "lve",
        "ma",    "mai",   "man",  "mang",  "mao",   "me",   "mei",   "men",   "meng",  "mi",   "mian",  "miao",  "mie",    "min",   "ming",  "miu",  "mo",    "mou",  "mu",    "na",    "nai",    "nan",  "nang", "nao",  "ne",   "nei",  "nen",  "neng", "ni",   "nian", "niang", "niao",
        "nie",   "nin",   "ning", "niu",   "nong",  "nou",  "nu",    "nuan",  "nue",   "nun",  "nuo",   "nv",    "nve",    "o",     "ou",    "pa",   "pai",   "pan",  "pang",  "pao",   "pei",    "pen",  "peng", "pi",   "pian", "piao", "pie",  "pin",  "ping", "po",   "pou",   "pu",
        "qi",    "qia",   "qian", "qiang", "qiao",  "qie",  "qin",   "qing",  "qiong", "qiu",  "qu",    "quan",  "que",    "qun",   "qve",   "ran",  "rang",  "rao",  "re",    "ren",   "reng",   "ri",   "rong", "rou",  "ru",   "ruan", "rui",  "run",  "ruo",  "sa",   "sai",   "san",
        "sang",  "sao",   "se",   "sen",   "seng",  "sha",  "shai",  "shan",  "shang", "shao", "she",   "shei",  "shen",   "sheng", "shi",   "shou", "shu",   "shua", "shuai", "shuan", "shuang", "shui", "shun", "shuo", "si",   "song", "sou",  "su",   "suan", "sui",  "sun",   "suo",
        "ta",    "tai",   "tan",  "tang",  "tao",   "te",   "teng",  "ti",    "tian",  "tiao", "tie",   "ting",  "tong",   "tou",   "tu",    "tuan", "tui",   "tun",  "tuo",   "wa",    "wai",    "wan",  "wang", "wei",  "wen",  "weng", "wo",   "wu",   "xi",   "xia",  "xian",  "xiang",
        "xiao",  "xie",   "xin",  "xing",  "xiong", "xiu",  "xu",    "xuan",  "xue",   "xun",  "xve",   "ya",    "yan",    "yang",  "yao",   "ye",   "yi",    "yin",  "ying",  "yo",    "yong",   "you",  "yu",   "yuan", "yue",  "yun",  "yve",  "za",   "zai",  "zan",  "zang",  "zao",
        "ze",    "zei",   "zen",  "zeng",  "zha",   "zhai", "zhan",  "zhang", "zhao",  "zhe",  "zhei",  "zhen",  "zheng",  "zhi",   "zhong", "zhou", "zhu",   "zhua", "zhuai", "zhuan", "zhuang", "zhui", "zhun", "zhuo", "zi",   "zong", "zou",  "zu",   "zuan", "zui",  "zun",   "zuo"};
    return kList;
}

const std::unordered_set<std::string> &IntactQuanpinSet()
{
    static const std::unordered_set<std::string> kSet(IntactQuanpinList().begin(), IntactQuanpinList().end());
    return kSet;
}

const std::unordered_map<std::string, std::string> &ShengmuMap()
{
    static const std::unordered_map<std::string, std::string> kMap = {
        {"u", "sh"},
        {"i", "ch"},
        {"v", "zh"},
    };
    return kMap;
}

const std::unordered_map<std::string, std::string> &ZeroShengmuMap()
{
    static const std::unordered_map<std::string, std::string> kMap = {
        {"aa", "a"},
        {"ai", "ai"},
        {"an", "an"},
        {"ao", "ao"},
        {"ah", "ang"},
        {"ee", "e"},
        {"ei", "ei"},
        {"en", "en"},
        {"eg", "eng"},
        {"er", "er"},
        {"oo", "o"},
        {"ou", "ou"},
    };
    return kMap;
}

const std::unordered_map<std::string, std::string> &YunmuMap()
{
    static const std::unordered_map<std::string, std::string> kMap = {
        {"q", "iu"}, {"w", "ei"}, {"e", "e"},   {"r", "uan"}, {"t", "ve"}, {"y", "un"}, {"u", "u"}, {"i", "i"},
        {"o", "o"},  {"p", "ie"}, {"a", "a"},   {"s", "iong"},{"d", "ai"}, {"f", "en"}, {"g", "eng"},{"h", "ang"},
        {"j", "an"}, {"k", "ing"},{"l", "iang"},{"z", "ou"},  {"x", "ia"}, {"c", "ao"}, {"v", "v"},  {"b", "in"},
        {"n", "iao"},{"m", "ian"},
    };
    return kMap;
}

std::string GetLocalAppDataPath()
{
    char *raw_path = nullptr;
    size_t len = 0;
    const errno_t err = _dupenv_s(&raw_path, &len, "LOCALAPPDATA");
    if (err != 0 || raw_path == nullptr)
    {
        return {};
    }
    std::unique_ptr<char, decltype(&free)> holder(raw_path, free);
    return std::string(raw_path);
}

std::vector<std::string> Split(const std::string &text, char delimiter)
{
    std::vector<std::string> parts;
    size_t start = 0;
    while (true)
    {
        const size_t pos = text.find(delimiter, start);
        if (pos == std::string::npos)
        {
            parts.push_back(text.substr(start));
            return parts;
        }
        parts.push_back(text.substr(start, pos - start));
        start = pos + 1;
    }
}

std::string ToLower(std::string text)
{
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return text;
}

std::string Join(const Segments &segments, const std::string &delimiter)
{
    std::string joined;
    for (size_t i = 0; i < segments.size(); ++i)
    {
        if (i > 0)
        {
            joined += delimiter;
        }
        joined += segments[i];
    }
    return joined;
}

std::string RemoveApostrophes(std::string text)
{
    text.erase(std::remove(text.begin(), text.end(), '\''), text.end());
    return text;
}

std::string ConvertSingleShuangpinToQuanpin(const std::string &syllable)
{
    const auto &zero_map = ZeroShengmuMap();
    const auto zero_found = zero_map.find(syllable);
    if (zero_found != zero_map.end())
    {
        return zero_found->second;
    }

    if (syllable.size() != 2)
    {
        return {};
    }

    std::string shengmu = syllable.substr(0, 1);
    const auto &shengmu_map = ShengmuMap();
    const auto shengmu_found = shengmu_map.find(shengmu);
    if (shengmu_found != shengmu_map.end())
    {
        shengmu = shengmu_found->second;
    }

    const auto &yunmu_map = YunmuMap();
    const auto yunmu_found = yunmu_map.find(syllable.substr(1, 1));
    if (yunmu_found == yunmu_map.end())
    {
        return {};
    }

    const std::string candidate = shengmu + yunmu_found->second;
    return IntactQuanpinSet().count(candidate) > 0 ? candidate : std::string{};
}

std::string ChooseTable(const std::string &shuangpin_code, size_t word_len)
{
    if (shuangpin_code.empty())
    {
        return {};
    }
    if (word_len >= 8)
    {
        return fmt::format("tbl_others_{}", shuangpin_code.front());
    }
    return fmt::format("tbl_{}_{}", word_len, shuangpin_code.front());
}

std::vector<QueryRow> RunDynamicQuery(sqlite3 *db, const std::string &sql)
{
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw std::runtime_error(sqlite3_errmsg(db));
    }

    std::vector<QueryRow> rows;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        rows.emplace_back(
            reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)),
            reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2)),
            sqlite3_column_int(stmt, 3));
    }
    sqlite3_finalize(stmt);
    return rows;
}

std::pair<std::string, bool> BuildSql(const std::string &shuangpin_code, const Segments &segments, int limit)
{
    bool all_complete = true;
    bool all_jianpin = true;
    size_t jianpin_count = 0;
    for (const auto &segment : segments)
    {
        if (segment.size() == 1)
        {
            all_complete = false;
            ++jianpin_count;
        }
        else
        {
            all_jianpin = false;
        }
    }

    const std::string table = ChooseTable(shuangpin_code, segments.size());
    if (table.empty())
    {
        return {"", false};
    }

    if (all_complete)
    {
        return {fmt::format("select * from {} where key = '{}' order by weight desc limit {};",
                            table,
                            shuangpin_code,
                            limit),
                false};
    }

    if (all_jianpin)
    {
        return {fmt::format("select * from {} where jp = '{}' order by weight desc limit {};",
                            table,
                            shuangpin_code,
                            limit),
                false};
    }

    if (jianpin_count == 1)
    {
        std::string pattern;
        for (const auto &segment : segments)
        {
            pattern += segment.size() == 1 ? segment + "_" : segment;
        }
        return {fmt::format("select * from {} where key like '{}' order by weight desc limit {};",
                            table,
                            pattern,
                            limit),
                false};
    }

    std::string jianpin;
    for (const auto &segment : segments)
    {
        jianpin.push_back(segment.front());
    }
    return {fmt::format("select * from {} where jp = '{}';", table, jianpin), true};
}

std::vector<QueryRow> FilterRows(const Segments &segments, const std::vector<QueryRow> &rows, int limit)
{
    std::string pattern_text;
    for (const auto &segment : segments)
    {
        pattern_text += segment.size() == 2 ? segment : segment + "[a-z]";
    }
    const std::regex pattern(pattern_text);

    std::vector<QueryRow> filtered;
    for (const auto &row : rows)
    {
        if (std::regex_match(std::get<0>(row), pattern))
        {
            filtered.push_back(row);
            if (static_cast<int>(filtered.size()) >= limit)
            {
                break;
            }
        }
    }
    return filtered;
}

class SqliteDb
{
  public:
    explicit SqliteDb(const std::string &db_path)
    {
        if (sqlite3_open(db_path.c_str(), &db_) != SQLITE_OK)
        {
            std::string message = db_ != nullptr ? sqlite3_errmsg(db_) : "sqlite open failed";
            if (db_ != nullptr)
            {
                sqlite3_close(db_);
                db_ = nullptr;
            }
            throw std::runtime_error(message);
        }
    }

    ~SqliteDb()
    {
        if (db_ != nullptr)
        {
            sqlite3_close(db_);
        }
    }

    SqliteDb(const SqliteDb &) = delete;
    SqliteDb &operator=(const SqliteDb &) = delete;

    sqlite3 *get() const
    {
        return db_;
    }

  private:
    sqlite3 *db_ = nullptr;
};

ProfiledQueryResult QueryWordsProfiledWithDb(
    SqliteDb &db,
    const std::string &shuangpin_code,
    int limit,
    std::chrono::steady_clock::duration db_open_elapsed)
{
    QueryProfile profile;
    const auto total_start = std::chrono::steady_clock::now();

    const auto segment_start = std::chrono::steady_clock::now();
    const std::string segmentation = SegmentInput(shuangpin_code);
    const std::string quanpin_segmentation = ToQuanpinSegmentation(segmentation);
    profile.segment_elapsed = std::chrono::steady_clock::now() - segment_start;

    const auto normalize_start = std::chrono::steady_clock::now();
    const std::string normalized_quanpin = RemoveApostrophes(quanpin_segmentation);
    profile.normalize_elapsed = std::chrono::steady_clock::now() - normalize_start;

    const Segments segments = segmentation.empty() ? Segments{} : Split(segmentation, '\'');
    const auto [sql, needs_filtering] = BuildSql(ToLower(shuangpin_code), segments, limit);
    const std::string table = ChooseTable(ToLower(shuangpin_code), segments.size());

    QueryResult result{
        shuangpin_code,
        segmentation,
        quanpin_segmentation,
        normalized_quanpin,
        table,
        sql,
        needs_filtering,
        {}};

    profile.db_open_elapsed = db_open_elapsed;
    if (sql.empty())
    {
        profile.total = std::chrono::steady_clock::now() - total_start;
        return {std::move(result), std::move(profile)};
    }

    const auto db_query_start = std::chrono::steady_clock::now();
    auto rows = RunDynamicQuery(db.get(), sql);
    profile.db_query_elapsed = std::chrono::steady_clock::now() - db_query_start;

    if (needs_filtering)
    {
        const auto filter_start = std::chrono::steady_clock::now();
        rows = FilterRows(segments, rows, limit);
        profile.filter_elapsed = std::chrono::steady_clock::now() - filter_start;
    }

    profile.row_count = static_cast<int>(rows.size());
    result.rows = std::move(rows);
    profile.total = std::chrono::steady_clock::now() - total_start;
    return {std::move(result), std::move(profile)};
}

} // namespace

std::string SegmentInput(const std::string &raw_input)
{
    const std::string input = ToLower(raw_input);
    if (input.size() <= 1)
    {
        return input;
    }

    std::string result;
    size_t start = 0;
    while (start < input.size())
    {
        if (start + 2 <= input.size())
        {
            const std::string candidate = input.substr(start, 2);
            if (!ConvertSingleShuangpinToQuanpin(candidate).empty())
            {
                if (!result.empty())
                {
                    result += "'";
                }
                result += candidate;
                start += 2;
                continue;
            }
        }

        if (!result.empty())
        {
            result += "'";
        }
        result += input.substr(start, 1);
        ++start;
    }
    return result;
}

std::string ToQuanpinSegmentation(const std::string &segmented_input)
{
    if (segmented_input.empty())
    {
        return {};
    }

    std::string result;
    for (const auto &segment : Split(segmented_input, '\''))
    {
        std::string converted = segment.size() == 1 ? segment : ConvertSingleShuangpinToQuanpin(segment);
        if (segment.size() == 1)
        {
            const auto found = ShengmuMap().find(segment);
            if (found != ShengmuMap().end())
            {
                converted = found->second;
            }
        }

        if (converted.empty())
        {
            continue;
        }

        if (!result.empty())
        {
            result += "'";
        }
        result += converted;
    }
    return result;
}

std::string NormalizeInput(const std::string &raw_input)
{
    return RemoveApostrophes(ToQuanpinSegmentation(SegmentInput(raw_input)));
}

ProfiledQueryResult QueryWordsProfiled(const std::string &shuangpin_code, const std::string &db_path, int limit)
{
    const auto db_open_start = std::chrono::steady_clock::now();
    SqliteDb db(db_path);
    const auto db_open_elapsed = std::chrono::steady_clock::now() - db_open_start;
    return QueryWordsProfiledWithDb(db, shuangpin_code, limit, db_open_elapsed);
}

std::vector<ProfiledQueryResult> QueryWordsProfiledMany(
    const std::vector<std::string> &shuangpin_codes,
    const std::string &db_path,
    int limit)
{
    if (shuangpin_codes.empty())
    {
        return {};
    }

    const auto db_open_start = std::chrono::steady_clock::now();
    SqliteDb db(db_path);
    const auto db_open_elapsed = std::chrono::steady_clock::now() - db_open_start;

    std::vector<ProfiledQueryResult> results;
    results.reserve(shuangpin_codes.size());
    for (size_t i = 0; i < shuangpin_codes.size(); ++i)
    {
        const auto open_elapsed = i == 0 ? db_open_elapsed : std::chrono::steady_clock::duration::zero();
        results.push_back(QueryWordsProfiledWithDb(db, shuangpin_codes[i], limit, open_elapsed));
    }
    return results;
}

const std::string &DefaultDbPath()
{
    static const std::string kPath = [] {
        const std::string appdata = GetLocalAppDataPath();
        return appdata.empty() ? std::string{} : appdata + "\\" + kDefaultAppName + "\\" + kDefaultDbName;
    }();
    return kPath;
}

const std::string &DefaultAppName()
{
    static const std::string kName = kDefaultAppName;
    return kName;
}

} // namespace shuangpin
