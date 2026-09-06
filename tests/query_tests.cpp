#include "quanpin_query.h"
#include "shuangpin_query.h"
#include <sqlite3.h>
#include <cstdio>
#include <iostream>
#include <stdexcept>

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

int main() {
    const char* db_path = "ci-query-fixture.db";
    std::remove(db_path);
    sqlite3* db = nullptr;
    try {
        require(sqlite3_open(db_path, &db) == SQLITE_OK, "open fixture");
        require(sqlite3_exec(db,
            "CREATE TABLE tbl_2_n (key TEXT, value TEXT, weight INTEGER, jp TEXT);"
            "INSERT INTO tbl_2_n VALUES ('ni''hao','你好',100,'nh');"
            "INSERT INTO tbl_2_n VALUES ('ni''hao','拟好',10,'nh');",
            nullptr, nullptr, nullptr) == SQLITE_OK, "create fixture");
        sqlite3_close(db);
        db = nullptr;
        const auto cuts = quanpin::CutQuanpinGreedy("xianren", true);
        require(cuts == std::vector<quanpin::Segments>{{"xian", "ren"}}, "greedy segmentation");
        require(quanpin::CutQuanpinByMode("jainmian", "correction") ==
            std::vector<quanpin::Segments>{{"jian", "mian"}}, "correction segmentation");
        const auto words = quanpin::QueryWordsFlat("ni'hao", db_path, "greedy", 1);
        require(words.size() == 1 && words[0].first == "你好", "ranked dictionary query and limit");
        const auto code = shuangpin::ToShuangpinInput("nihao");
        require(!code.empty(), "double pinyin encoding");
        require(shuangpin::NormalizeInput(code) == "nihao", "double pinyin round trip");
    } catch (const std::exception& error) {
        if (db) sqlite3_close(db);
        std::remove(db_path);
        std::cerr << error.what() << '\n';
        return 1;
    }
    std::remove(db_path);
    return 0;
}
