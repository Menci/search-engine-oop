#include "InvertedIndex.h"

void InvertedIndex::addDocument(uint32_t docIDInSegment, const std::vector<std::string> &terms) {
    for (size_t i = 0; i < terms.size(); i++) {
        const std::string &term = terms[i];
        std::map<uint32_t, std::list<uint32_t>> &postingList = postingLists[term];
        postingList[docIDInSegment].push_back(i);
    }
}

uint32_t InvertedIndex::getTermOccuredDocumentCount(const std::string &term) const {
    return getPostingList(term).size();
}

uint32_t InvertedIndex::getTermOccuredCountInDocument(const std::string &term, uint32_t docIDInSegment) const {
    return getOccurPositionsInDocument(term, docIDInSegment).size();
}

const std::map<uint32_t, std::list<uint32_t>> &InvertedIndex::getPostingList(const std::string &term) const {
    std::map<std::string, std::map<uint32_t, std::list<uint32_t>>>::const_iterator it = postingLists.find(term);
    if (it == postingLists.end()) {
        static std::map<uint32_t, std::list<uint32_t>> emptyPostingList;
        return emptyPostingList;
    }

    return it->second;
}

const std::list<uint32_t> &InvertedIndex::getOccurPositionsInDocument(const std::string &term, uint32_t docIDInSegment) const {
    std::map<uint32_t, std::list<uint32_t>> postingList = getPostingList(term);
    std::map<uint32_t, std::list<uint32_t>>::const_iterator it = postingList.find(docIDInSegment);
    if (it == postingList.end()) {
        static std::list<uint32_t> emptyList;
        return emptyList;
    }

    return it->second;
}
