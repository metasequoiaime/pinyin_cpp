#include <fmt/core.h>
#include <fmt/ranges.h>

#include <chrono>
#include <exception>
#include <string>
#include <tuple>
#include <vector>

#include "quanpin_query.h"
#include "shuangpin_query.h"

namespace
{

double ToMilliseconds(std::chrono::steady_clock::duration duration)
{
    return std::chrono::duration<double, std::milli>(duration).count();
}

std::vector<std::string> DefaultQuanpins()
{
    return {
        "a",
        "shi",
        "wo",
        "ni",
        "ta",
        "women",
        "nimen",
        "tamen",
        "nihao",
        "zaoshanghao",
        "wanshanghao",
        "zaijian",
        "xiexie",
        "duibuqi",
        "meiguanxi",
        "shenme",
        "weishenme",
        "zenme",
        "zenmeyang",
        "zenmeban",
        "weishenmezheyang",
        "nage",
        "zhege",
        "yige",
        "liangge",
        "zheiyang",
        "nayang",
        "yiming",
        "yigeren",
        "yijia",
        "yibeizi",
        "nanjingren",
        "mingtian",
        "jintian",
        "zuotian",
        "houtian",
        "qiantian",
        "jinnian",
        "qunian",
        "mingnian",
        "zhongguo",
        "beijing",
        "shanghai",
        "guangzhou",
        "shenzhen",
        "hangzhou",
        "chengdu",
        "wuhan",
        "xian",
        "chongqing",
        "gongzuo",
        "xuexi",
        "shenghuo",
        "xuesheng",
        "laoshi",
        "tongxue",
        "pengyou",
        "jiaoren",
        "diannao",
        "shouji",
        "shurufa",
        "pinyin",
        "zhongwen",
        "hanzi",
        "ceshi",
        "shujuku",
        "wangluo",
        "fuwuqi",
        "wenjian",
        "mima",
        "zhanghao",
        "chengxu",
        "biancheng",
        "daima",
        "suanfa",
        "neicun",
        "cunchu",
        "yinyue",
        "dianying",
        "dianshiju",
        "zongyijiemu",
        "tiyu",
        "lanqiu",
        "zuqiu",
        "paobu",
        "lvxing",
        "huochezhan",
        "feijichang",
        "gongjiaoche",
        "ditiezhan",
        "chuzuche",
        "niuroumian",
        "jiaozi",
        "chaofan",
        "mifan",
        "kafei",
        "naicha",
        "bingqilin",
        "pingguo",
        "xiangjiao",
        "xigua",
        "caomei",
        "lanmei",
        "huluobo",
        "xihongshi",
        "qiezi",
        "boluobo",
        "xilanhua",
        "gonggongqiche",
        "jisuanji",
        "diannaokexue",
        "ziranyuyan",
        "jirenshuru",
        "zhinengshurufa",
        "shujujiegou",
        "mashangkaishi",
        "qingdengyixia",
        "wozhidaole",
        "wohaizaixiang",
        "zhegeshihendaidewenti",
        "ganm",
        "yig",
        "kanl"
    };
}

std::vector<std::string> DefaultShuangpins()
{
    std::vector<std::string> shuangpins;
    for (const auto &quanpin_input : DefaultQuanpins())
    {
        const auto shuangpin_input = shuangpin::ToShuangpinInput(quanpin_input);
        shuangpins.push_back(shuangpin_input.empty() ? quanpin_input : shuangpin_input);
    }
    return shuangpins;
}

std::string QueryLabel(size_t index)
{
    if (index == 0)
    {
        return "first query";
    }
    return fmt::format("query #{} (same db connection, different quanpin, no open cost)", index + 1);
}

std::string ShuangpinQueryLabel(size_t index)
{
    if (index == 0)
    {
        return "first shuangpin query";
    }
    return fmt::format("shuangpin query #{} (same db connection, different input, no open cost)", index + 1);
}

void PrintProfile(const char *label, const quanpin::ProfiledQueryResult &profiled)
{
    const auto &result = profiled.result;
    const auto &profile = profiled.profile;

    fmt::print("{}\n", label);
    fmt::print("query \"{}\" ({})\n", result.quanpin, result.mode);
    fmt::print("total: {:.3f} ms\n", ToMilliseconds(profile.total));
    fmt::print(
        "breakdown: cut={:.3f} ms, dedupe={:.3f} ms, db_open={:.3f} ms, db_query={:.3f} ms\n",
        ToMilliseconds(profile.cut_elapsed),
        ToMilliseconds(profile.dedupe_elapsed),
        ToMilliseconds(profile.db_open_elapsed),
        ToMilliseconds(profile.db_query_elapsed));
    fmt::print(
        "cuts: raw={}, unique={}\n",
        profile.raw_cut_count,
        profile.unique_cut_count);
    for (const auto &cut_profile : profile.cut_profiles)
    {
        fmt::print(
            "cut profile: ({}) table={} key={} elapsed={:.3f} ms\n",
            fmt::join(cut_profile.segments, ", "),
            cut_profile.table,
            cut_profile.key,
            ToMilliseconds(cut_profile.elapsed));
        for (const auto &sql_profile : cut_profile.sql_profiles)
        {
            fmt::print(
                "  sql {} value={} rows={} elapsed={:.3f} ms\n",
                sql_profile.label,
                sql_profile.lookup_value,
                sql_profile.row_count,
                ToMilliseconds(sql_profile.elapsed));
        }
    }
    for (const auto &entry : result.results)
    {
        fmt::print("segments: (");
        for (size_t i = 0; i < entry.segments.size(); ++i)
        {
            fmt::print("{}{}", entry.segments[i], i + 1 == entry.segments.size() ? "" : ", ");
        }
        fmt::print(") -> table {}, key={}\n", entry.table, entry.key);
        if (entry.items.empty())
        {
            fmt::print("(no result)\n");
            continue;
        }
        for (const auto &[value, weight] : entry.items)
        {
            fmt::print("{} ({})\n", value, weight);
        }
    }
}

void PrintShuangpinProfile(const char *label, const shuangpin::ProfiledQueryResult &profiled)
{
    const auto &result = profiled.result;
    const auto &profile = profiled.profile;

    fmt::print("{}\n", label);
    fmt::print("query \"{}\"\n", result.shuangpin);
    fmt::print("segmentation: {}\n", result.segmentation.empty() ? "(empty)" : result.segmentation);
    fmt::print("quanpin segmentation: {}\n", result.quanpin_segmentation.empty() ? "(empty)" : result.quanpin_segmentation);
    fmt::print("normalized quanpin: {}\n", result.normalized_quanpin.empty() ? "(empty)" : result.normalized_quanpin);
    fmt::print("table: {}\n", result.table.empty() ? "(none)" : result.table);
    fmt::print("needs filtering: {}\n", result.needs_filtering ? "yes" : "no");
    fmt::print("sql: {}\n", result.sql.empty() ? "(none)" : result.sql);
    fmt::print("total: {:.3f} ms\n", ToMilliseconds(profile.total));
    fmt::print(
        "breakdown: segment={:.3f} ms, normalize={:.3f} ms, db_open={:.3f} ms, db_query={:.3f} ms, filter={:.3f} ms\n",
        ToMilliseconds(profile.segment_elapsed),
        ToMilliseconds(profile.normalize_elapsed),
        ToMilliseconds(profile.db_open_elapsed),
        ToMilliseconds(profile.db_query_elapsed),
        ToMilliseconds(profile.filter_elapsed));
    fmt::print("rows: {}\n", profile.row_count);
    for (const auto &[key, value, weight] : result.rows)
    {
        fmt::print("key={} -> {} ({})\n", key, value, weight);
    }
}

} // namespace

int main()
{
    std::vector<std::string> quanpins;
    std::vector<std::string> shuangpins;
    std::string mode = "correction";
    std::string quanpin_db_path = quanpin::DefaultDbPath();
    std::string shuangpin_db_path = shuangpin::DefaultDbPath();

    quanpins = DefaultQuanpins();
    shuangpins = DefaultShuangpins();

    try
    {
        const auto quanpin_results = quanpin::QueryWordsProfiledMany(
            quanpins,
            quanpin_db_path,
            mode,
            80);
        const auto shuangpin_results = shuangpin::QueryWordsProfiledMany(
            shuangpins,
            shuangpin_db_path,
            80);

        fmt::print("=== quanpin ===\n");
        for (size_t i = 0; i < quanpin_results.size(); ++i)
        {
            if (i > 0)
            {
                fmt::print("\n");
            }
            const auto label = QueryLabel(i);
            PrintProfile(label.c_str(), quanpin_results[i]);
        }

        fmt::print("\n\n=== shuangpin ===\n");
        for (size_t i = 0; i < shuangpin_results.size(); ++i)
        {
            if (i > 0)
            {
                fmt::print("\n");
            }
            const auto label = ShuangpinQueryLabel(i);
            PrintShuangpinProfile(label.c_str(), shuangpin_results[i]);
        }
        return 0;
    }
    catch (const std::exception &ex)
    {
        fmt::print(stderr, "error: {}\n", ex.what());
        return 1;
    }
}
