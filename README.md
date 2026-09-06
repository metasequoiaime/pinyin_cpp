# pinyin_cpp

<!-- badges:start -->
[![CI](https://img.shields.io/github/actions/workflow/status/metasequoiaime/pinyin_cpp/ci.yml?branch=main&label=CI)](https://github.com/metasequoiaime/pinyin_cpp/actions/workflows/ci.yml)
[![CodeQL](https://img.shields.io/github/actions/workflow/status/metasequoiaime/pinyin_cpp/codeql.yml?branch=main&label=CodeQL)](https://github.com/metasequoiaime/pinyin_cpp/actions/workflows/codeql.yml)
[![License](https://img.shields.io/github/license/metasequoiaime/pinyin_cpp)](LICENSE)
[![Stars](https://img.shields.io/github/stars/metasequoiaime/pinyin_cpp?style=flat)](https://github.com/metasequoiaime/pinyin_cpp/stargazers)
<!-- badges:end -->

全拼 / 双拼候选查询的 C++ 早期原型，词库用 SQLite。

`src/quanpin_query.*` 和 `src/shuangpin_query.*` 是两种方案的查询实现，`tests/query_tests.cpp` 是配套用例。它把 [pinyin_python](https://github.com/metasequoiaime/pinyin_python) 里验证过的思路用 C++ 重写了一遍，是从原型走向正式引擎的中间一步。

**这个仓库已经被取代，不再开发。** 正式实现在 [MSIME-Engine](https://github.com/metasequoiaime/MSIME-Engine) 的 `quanpin/` 与 `shuangpin/`。

要参与输入法开发请到 [MSIME-Engine](https://github.com/metasequoiaime/MSIME-Engine) 或各平台前端仓库。

<!-- star-history:start -->
## Star History

<a href="https://star-history.com/#metasequoiaime/pinyin_cpp&Date">
  <img src="https://api.star-history.com/svg?repos=metasequoiaime/pinyin_cpp&type=Date" alt="Star History Chart" width="600">
</a>
<!-- star-history:end -->
