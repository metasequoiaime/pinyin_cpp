# pinyin_cpp

全拼 / 双拼候选查询的 C++ 早期原型，基于 SQLite 词库。

**这个仓库已被取代，不再用于水杉输入法的开发。** 它保留下来是作为历史记录：全拼与双拼查询最早是在这里试出来的，之后才成为产品代码。

现在的实现在 [MSIME-Engine](https://github.com/metasequoiaime/MSIME-Engine)——跨平台组词引擎、共享协议、词库生产与辅助码都在那里，Windows、Apple、Linux 三个前端都以 submodule 的形式固定引用它。要改输入行为、查询逻辑或词库，去 MSIME-Engine，不要改这里。

本仓库内容：

- `src/quanpin_query.cpp`、`src/shuangpin_query.cpp` — 针对 SQLite 词库的候选查询原型
- `src/main.cpp` — 手工试验用的命令行入口
- `tests/query_tests.cpp` — 原型自带的测试

这些代码不参与任何产品构建，也不接受功能性改动。
