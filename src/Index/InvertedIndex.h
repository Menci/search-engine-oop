#ifndef _MENCI_SEARCH_ENGINE_INVERTED_INDEX_H
#define _MENCI_SEARCH_ENGINE_INVERTED_INDEX_H

#include <list>
#include <map>
#include <list>
#include <string>

#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>

struct InvertedIndex {
    // [term => [docIDInSegment => [position]]]
    std::map<std::string, std::map<uint32_t, std::list<uint32_t>>> postingLists;

    friend class boost::serialization::access;

    template <typename Archive>
    void serialize(Archive &archive, unsigned int version) {
        archive & BOOST_SERIALIZATION_NVP(postingLists);
    }

    void addDocument(uint32_t docIDInSegment, const std::vector<std::string> &terms);

    uint32_t getTermOccuredDocumentCount(const std::string &term) const;
    uint32_t getTermOccuredCountInDocument(const std::string &term, uint32_t docIDInSegment) const;

    const std::map<uint32_t, std::list<uint32_t>> &getPostingList(const std::string &term) const;
    const std::list<uint32_t> &getOccurPositionsInDocument(const std::string &term, uint32_t docIDInSegment) const;
};

#endif // _MENCI_SEARCH_ENGINE_INVERTED_INDEX_H
