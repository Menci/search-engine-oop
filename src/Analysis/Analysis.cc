#include "Analysis.h"

#include <iostream>
#include <regex>

#include <boost/algorithm/string.hpp>

Jieba Analysis::jieba;
std::set<std::string> Analysis::stopWords;

void Analysis::setStopWords(const std::set<std::string> &stopWords) {
    Analysis::stopWords = stopWords;
}

std::vector<std::string> Analysis::analysis(const std::string &text) {
    std::string newText = text;
    newText = removeExtraSpaces(newText);

    std::vector<std::string> splitted, tokens;
    boost::iter_split(splitted, newText, boost::first_finder(" "));
    for (const std::string &textPart : splitted) {
        std::vector<std::string> words = jieba.cut(textPart);
        tokens.insert(tokens.end(), words.begin(), words.end());
    }

    tokens = filterStopwords(tokens);

    return tokens;
}

std::string Analysis::removeExtraSpaces(const std::string &text) {
    static std::regex spaces("\\s+");
    return std::regex_replace(text, spaces, " ");
}

std::vector<std::string> Analysis::filterStopwords(const std::vector<std::string> &tokens) {
    std::vector<std::string> result;
    std::copy_if(tokens.begin(), tokens.end(), std::back_inserter(result), [](const std::string &word) -> bool {
        return stopWords.count(word) == 0;
    });

    return result;
}
