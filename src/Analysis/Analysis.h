#ifndef _MENCI_SEARCH_ENGINE_ANALYSIS_H
#define _MENCI_SEARCH_ENGINE_ANALYSIS_H

#include <set>
#include <vector>
#include <string>

#include "Jieba.h"

class Analysis {
    static Jieba jieba;
    static std::set<std::string> stopWords;

public:
    static void setStopWords(const std::set<std::string> &stopWords);

    static std::vector<std::string> analysis(const std::string &text);

    static std::string removeExtraSpaces(const std::string &text);

    static std::vector<std::string> filterStopwords(const std::vector<std::string> &tokens);
};

#endif // _MENCI_SEARCH_ENGINE_ANALYSIS_H
