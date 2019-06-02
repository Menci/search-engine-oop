#ifndef _MENCI_SEARCH_ENGINE_INDEX_H
#define _MENCI_SEARCH_ENGINE_INDEX_H

#include <cstdint>
#include <string>
#include <filesystem>
#include <memory>

#include <boost/compute/detail/lru_cache.hpp>

#include "Document/Document.h"
#include "Segment.h"

class Index {
    uint32_t segmentsCount, documentsCount, documentsPerSegment;
    std::map<std::string, uint32_t> termOccurCount;
    std::filesystem::path path;
    boost::compute::detail::lru_cache<uint32_t, std::shared_ptr<Segment>> segmentCache;

    bool modified;

public:
    Index(const std::string &path);
    Index(const Index &) = delete;
    Index(Index &&) = default;
    ~Index();

    uint32_t getSegmentsCount() const;
    uint32_t getDocumentsCount() const;
    uint32_t getDocumentsPerSegment() const;

    uint32_t toGlobalDocID(uint32_t docIDInSegment, uint32_t segmentID) const;
    void toInSegmentDocID(uint32_t docIDGlobal, uint32_t &segmentID, uint32_t &docIDInSegment) const;

    std::shared_ptr<Segment> getSegment(uint32_t segmentID);
    Document getDocument(uint32_t docIDGlobal);
    void deleteDocument(uint32_t docIDGlobal);
    uint32_t addDocument(const Document &document);

    void save();
};

#endif // _MENCI_SEARCH_ENGINE_INDEX_H
