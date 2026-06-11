#include <fmt/core.h>

#include <exception>
#include <string>

#include "pinyin_query.h"

int main(int argc, char *argv[])
{
    std::string pinyin_text = argc > 1 ? argv[1] : "nihao";
    pinyin_text = "shenme";
    const std::string mode = argc > 2 ? argv[2] : "correction";
    const std::string db_path = argc > 3 ? argv[3] : pinyin::DefaultDbPath();

    try
    {
        const auto result = pinyin::QueryWords(pinyin_text, db_path, mode);
        fmt::print("query \"{}\" ({})\n", result.pinyin, result.mode);
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
        return 0;
    }
    catch (const std::exception &ex)
    {
        fmt::print(stderr, "error: {}\n", ex.what());
        return 1;
    }
}
