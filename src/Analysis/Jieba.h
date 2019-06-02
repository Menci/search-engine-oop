#ifndef _MENCI_SEARCH_ENGINE_JIEBA_H
#define _MENCI_SEARCH_ENGINE_JIEBA_H

#include <vector>
#include <string>

class Jieba {
    pid_t pid;
    int fdWriteStdin, fdReadStdout;

    static const char *pythonDaemonCode;

public:
    Jieba();
    ~Jieba();

    std::vector<std::string> cut(const std::string &stringToCut);
};

#endif // _MENCI_SEARCH_ENGINE_JIEBA_H
