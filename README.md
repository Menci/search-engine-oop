# Search Engine
The task of Advanced Programming class on 2019/04/23. Text search engine using inversed index and TF-IDF in C++.

# Requirements
* GCC (>= 8), Clang (>= 6)
* CMake (>= 3.0)
* Boost
* GNU Readline
* Python3 (>= 3.4)
* jieba

# Build
Replace `clang++` with your compiler.

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++ ..
make
# Compiled program will be in `bin/search-engine`
```

# Usage
Build and run this project, you'll get a REPL.

Crawl a website with [web-crawler](https://github.com/Menci/web-crawler) (or another crawler), put only HTML documents (path preserved) in a directory. Build index database with:

```bash
>>> ADD-INDEX documents-directory
```

Then start search with:

```bash
>>> QUERY keywords
```

# Notices
This project is only aimed to studying OOP and the structure of Lucene. No optimized algorithm is used so it's very slow. Serialized data is in XML format so it costs a lot time to load data and large space to store, so don't use this project with non-study purpose.
