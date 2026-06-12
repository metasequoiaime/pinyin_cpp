#include "pinyin_query.h"

#include <sqlite3.h>

#include <algorithm>
#include <functional>
#include <stdexcept>
#include <string>
#include <unordered_set>

namespace pinyin
{
namespace
{

constexpr const char *kDefaultDbPath = R"(C:\Users\sonnycalcr\AppData\Local\MetasequoiaImeTsf\quanpin_multi_tbl_has_jp.db)";

const std::vector<std::string> &IntactPinyinList()
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

const std::unordered_set<std::string> &IntactPinyinSet()
{
    static const std::unordered_set<std::string> kSet(IntactPinyinList().begin(), IntactPinyinList().end());
    return kSet;
}

const std::unordered_set<std::string> &AllPinyinSet()
{
    static const std::unordered_set<std::string> kSet = [] {
        std::unordered_set<std::string> result;
        for (const auto &item : IntactPinyinList())
        {
            for (size_t i = 1; i <= item.size(); ++i)
            {
                result.insert(item.substr(0, i));
            }
        }
        return result;
    }();
    return kSet;
}

std::vector<FuzzyRule> MakeDefaultFuzzyRules()
{
    return {
        {"z", "zh", false},       //
        {"c", "ch", false},       //
        {"s", "sh", false},       //
        {"g", "k", false},        //
        {"h", "f", false},        //
        {"n", "l", false},        //
        {"r", "l", false},        //
        {"an", "ang", false},     //
        {"en", "eng", false},     //
        {"in", "ing", false},     //
        {"an", "ai", false},      //
        {"eng", "ong", false},    //
        {"ian", "iang", false},   //
        {"on", "ong", false},     //
        {"uan", "uang", false},   //
        {"un", "ong", false},     //
        {"un", "iong", false},    //
        {"hui", "fei", false},    //
        {"huang", "wang", false}, //
    };
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

std::string BuildTableName(const Segments &segments)
{
    return "tbl_" + std::to_string(segments.size()) + "_" + std::string(1, segments.front().front());
}

std::string SegmentsToKey(const Segments &segments)
{
    return Join(segments, "'");
}

std::string SegmentsToJp(const Segments &segments)
{
    std::string jp;
    for (const auto &segment : segments)
    {
        if (!segment.empty())
        {
            jp.push_back(segment.front());
        }
    }
    return jp;
}

std::string BuildKeyLikePattern(const Segments &segments)
{
    Segments parts;
    for (size_t i = 0; i < segments.size(); ++i)
    {
        const bool is_last = i + 1 == segments.size();
        const auto &segment = segments[i];
        if (is_last || segment.size() == 1)
        {
            parts.push_back(segment + "%");
        }
        else
        {
            parts.push_back(segment);
        }
    }
    return Join(parts, "'");
}

bool IsPureJpInput(const Segments &segments)
{
    return std::all_of(segments.begin(), segments.end(), [](const std::string &s) { return s.size() == 1; });
}

std::string FindLongestPrefix(const std::string &pinyin, const std::unordered_set<std::string> &pinyin_set)
{
    for (size_t end = pinyin.size(); end > 0; --end)
    {
        const auto candidate = pinyin.substr(0, end);
        if (pinyin_set.find(candidate) != pinyin_set.end())
        {
            return candidate;
        }
    }
    return {};
}

std::string SwapAdjacent(const std::string &s, size_t index)
{
    std::string out = s;
    std::swap(out[index], out[index + 1]);
    return out;
}

std::pair<std::string, size_t> CorrectFirstSyllableBySwap(const std::string &pinyin)
{
    const auto partial = FindLongestPrefix(pinyin, AllPinyinSet());
    if (partial.empty() || partial.size() >= pinyin.size())
    {
        return {};
    }
    for (size_t end = pinyin.size(); end > partial.size(); --end)
    {
        const auto candidate = pinyin.substr(0, end);
        for (size_t index = 0; index + 1 < candidate.size(); ++index)
        {
            const auto corrected = SwapAdjacent(candidate, index);
            if (IntactPinyinSet().find(corrected) != IntactPinyinSet().end())
            {
                return {corrected, end};
            }
        }
    }
    return {};
}

std::string ApplyFuzzyRulesGlobally(const std::string &pinyin, const std::vector<FuzzyRule> &fuzzy_rules)
{
    std::string changed = pinyin;
    for (const auto &rule : fuzzy_rules)
    {
        if (!rule.enabled)
        {
            continue;
        }
        const auto source_pos = changed.find(rule.source);
        if (source_pos != std::string::npos)
        {
            changed.replace(source_pos, rule.source.size(), rule.target);
            continue;
        }
        const auto target_pos = changed.find(rule.target);
        if (target_pos != std::string::npos)
        {
            changed.replace(target_pos, rule.target.size(), rule.source);
        }
    }
    return changed;
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

std::vector<QueryItem> RunQuery(sqlite3 *db, const std::string &sql, const std::string &value, int limit)
{
    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        return {};
    }

    sqlite3_bind_text(stmt, 1, value.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, limit);

    std::vector<QueryItem> rows;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const unsigned char *text = sqlite3_column_text(stmt, 0);
        const int weight = sqlite3_column_int(stmt, 1);
        rows.emplace_back(text == nullptr ? "" : reinterpret_cast<const char *>(text), weight);
    }
    sqlite3_finalize(stmt);
    return rows;
}

std::vector<QueryItem> QuerySingleCut(sqlite3 *db, const Segments &segments, int limit)
{
    const auto table = BuildTableName(segments);
    const auto key = SegmentsToKey(segments);
    const auto jp = SegmentsToJp(segments);
    const auto key_like_pattern = BuildKeyLikePattern(segments);

    const auto exact_sql = "SELECT \"value\", \"weight\" FROM \"" + table + "\" WHERE \"key\" = ? ORDER BY \"weight\" DESC LIMIT ?";
    auto rows = RunQuery(db, exact_sql, key, limit);
    if (!rows.empty())
    {
        return rows;
    }

    const auto prefix_sql = "SELECT \"value\", \"weight\" FROM \"" + table + "\" WHERE \"key\" LIKE ? ORDER BY \"weight\" DESC LIMIT ?";
    rows = RunQuery(db, prefix_sql, key_like_pattern, limit);
    if (!rows.empty())
    {
        return rows;
    }

    if (!IsPureJpInput(segments))
    {
        return {};
    }

    const auto jp_sql = "SELECT \"value\", \"weight\" FROM \"" + table + "\" WHERE \"jp\" = ? ORDER BY \"weight\" DESC LIMIT ?";
    return RunQuery(db, jp_sql, jp, limit);
}

std::vector<Segments> CutPinyinByModeNoBreak(const std::string &pinyin, const std::string &mode, const std::vector<FuzzyRule> &fuzzy_rules)
{
    if (mode == "fuzzy")
    {
        std::vector<Segments> ans;
        auto base = CutPinyinGreedy(pinyin, false, false);
        if (!base.empty())
        {
            ans.insert(ans.end(), base.begin(), base.end());
        }
        const auto fuzzy_pinyin = ApplyFuzzyRulesGlobally(pinyin, fuzzy_rules);
        if (fuzzy_pinyin != pinyin)
        {
            auto fuzzy_cut = CutPinyinGreedy(fuzzy_pinyin, false, false);
            if (!fuzzy_cut.empty() && std::find(ans.begin(), ans.end(), fuzzy_cut.front()) == ans.end())
            {
                ans.insert(ans.end(), fuzzy_cut.begin(), fuzzy_cut.end());
            }
        }
        return ans;
    }

    std::function<std::vector<Segments>(const std::string &)> cut_recursive = [&](const std::string &rest) -> std::vector<Segments> {
        if (rest.empty())
        {
            return {{}};
        }

        const auto first = FindLongestPrefix(rest, AllPinyinSet());
        if (first.empty())
        {
            return {};
        }

        auto prepend = [&](const std::string &head, const std::vector<Segments> &tails) -> std::vector<Segments> {
            std::vector<Segments> merged;
            merged.reserve(tails.size());
            for (const auto &tail : tails)
            {
                Segments current;
                current.reserve(tail.size() + 1);
                current.push_back(head);
                current.insert(current.end(), tail.begin(), tail.end());
                merged.push_back(std::move(current));
            }
            return merged;
        };

        if (mode == "greedy")
        {
            return prepend(first, cut_recursive(rest.substr(first.size())));
        }

        if (IntactPinyinSet().find(first) != IntactPinyinSet().end())
        {
            auto tails = cut_recursive(rest.substr(first.size()));
            if (!tails.empty())
            {
                return prepend(first, tails);
            }
        }

        const auto correction = CorrectFirstSyllableBySwap(rest);
        if (!correction.first.empty())
        {
            auto tails = cut_recursive(rest.substr(correction.second));
            if (!tails.empty())
            {
                return prepend(correction.first, tails);
            }
        }

        auto tails = cut_recursive(rest.substr(first.size()));
        if (!tails.empty())
        {
            return prepend(first, tails);
        }

        return {};
    };

    return cut_recursive(pinyin);
}

} // namespace

const std::vector<FuzzyRule> &DefaultFuzzyRules()
{
    static const std::vector<FuzzyRule> kRules = MakeDefaultFuzzyRules();
    return kRules;
}

const std::string &DefaultDbPath()
{
    static const std::string kPath = kDefaultDbPath;
    return kPath;
}

std::vector<Segments> CutPinyinGreedy(const std::string &pinyin, bool is_intact, bool is_break)
{
    const auto &pinyin_set = is_intact ? IntactPinyinSet() : AllPinyinSet();

    if (is_break && pinyin.find('\'') != std::string::npos)
    {
        Segments merged;
        for (const auto &part : Split(pinyin, '\''))
        {
            auto cut = CutPinyinGreedy(part, is_intact, false);
            if (!cut.empty())
            {
                merged.insert(merged.end(), cut.front().begin(), cut.front().end());
            }
            else
            {
                merged.push_back(part);
            }
        }
        return {merged};
    }

    Segments ans;
    size_t index = 0;
    while (index < pinyin.size())
    {
        std::string matched;
        for (size_t end = pinyin.size(); end > index; --end)
        {
            const auto piece = pinyin.substr(index, end - index);
            if (pinyin_set.find(piece) != pinyin_set.end())
            {
                matched = piece;
                break;
            }
        }
        if (matched.empty())
        {
            return {};
        }
        ans.push_back(matched);
        index += matched.size();
    }
    return {ans};
}

std::vector<Segments> CutPinyinByMode(const std::string &pinyin, const std::string &mode, const std::vector<FuzzyRule> &fuzzy_rules, bool is_break)
{
    if (mode != "greedy" && mode != "correction" && mode != "fuzzy")
    {
        throw std::invalid_argument("mode must be one of: greedy, correction, fuzzy");
    }

    const auto &rules = fuzzy_rules.empty() ? DefaultFuzzyRules() : fuzzy_rules;
    if (is_break && pinyin.find('\'') != std::string::npos)
    {
        Segments merged;
        for (const auto &part : Split(pinyin, '\''))
        {
            auto cut = CutPinyinByMode(part, mode, rules, false);
            if (!cut.empty())
            {
                merged.insert(merged.end(), cut.front().begin(), cut.front().end());
            }
            else if (mode == "greedy" || mode == "correction")
            {
                merged.push_back(part);
            }
            else
            {
                return {};
            }
        }
        return {merged};
    }

    return CutPinyinByModeNoBreak(pinyin, mode, rules);
}

QueryResult QueryWords(const std::string &pinyin, const std::string &db_path, const std::string &mode, int limit)
{
    std::vector<Segments> cuts;
    if (mode == "greedy")
    {
        cuts = CutPinyinGreedy(pinyin, false, true);
    }
    else if (mode == "correction" || mode == "fuzzy")
    {
        cuts = CutPinyinByMode(pinyin, mode, DefaultFuzzyRules(), true);
    }
    else
    {
        throw std::invalid_argument("Unknown mode: " + mode);
    }

    QueryResult result{pinyin, mode, {}};
    if (cuts.empty())
    {
        return result;
    }

    std::vector<Segments> unique_cuts;
    for (const auto &cut : cuts)
    {
        if (std::find(unique_cuts.begin(), unique_cuts.end(), cut) == unique_cuts.end())
        {
            unique_cuts.push_back(cut);
        }
    }

    SqliteDb db(db_path);
    for (const auto &segments : unique_cuts)
    {
        result.results.push_back(QueryResultEntry{segments, SegmentsToKey(segments), BuildTableName(segments), QuerySingleCut(db.get(), segments, limit)});
    }
    return result;
}

std::vector<QueryItem> QueryWordsFlat(const std::string &pinyin, const std::string &db_path, const std::string &mode, int limit)
{
    const auto result = QueryWords(pinyin, db_path, mode, limit);
    std::vector<QueryItem> items;
    std::unordered_set<std::string> seen;
    for (const auto &entry : result.results)
    {
        for (const auto &item : entry.items)
        {
            if (seen.insert(item.first).second)
            {
                items.push_back(item);
            }
        }
    }
    std::sort(items.begin(), items.end(), [](const QueryItem &lhs, const QueryItem &rhs) { return lhs.second > rhs.second; });
    if (static_cast<int>(items.size()) > limit)
    {
        items.resize(static_cast<size_t>(limit));
    }
    return items;
}

} // namespace pinyin
