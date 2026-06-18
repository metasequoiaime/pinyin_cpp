#include <fmt/core.h>
#include <fmt/ranges.h>

#include <chrono>
#include <exception>
#include <string>
#include <vector>

#include "quanpin_query.h"

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

std::string QueryLabel(size_t index)
{
    if (index == 0)
    {
        return "first query";
    }
    return fmt::format("query #{} (same db connection, different quanpin, no open cost)", index + 1);
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

} // namespace

int main()
{
    std::vector<std::string> quanpins;
    std::string mode = "correction";
    std::string db_path = quanpin::DefaultDbPath();

    quanpins = DefaultQuanpins();

    try
    {
        const auto profiled_results = quanpin::QueryWordsProfiledMany(
            quanpins,
            db_path,
            mode,
            80);

        for (size_t i = 0; i < profiled_results.size(); ++i)
        {
            if (i > 0)
            {
                fmt::print("\n");
            }
            const auto label = QueryLabel(i);
            PrintProfile(label.c_str(), profiled_results[i]);
        }
        return 0;
    }
    catch (const std::exception &ex)
    {
        fmt::print(stderr, "error: {}\n", ex.what());
        return 1;
    }
}
