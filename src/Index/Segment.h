#ifndef _MENCI_SEARCH_ENGINE_SEGMENT_H
#define _MENCI_SEARCH_ENGINE_SEGMENT_H

#include <filesystem>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <memory>

#include "InvertedIndex.h"
#include "Document/Document.h"
#include "Document/Meta.h"

class Segment {
    std::map<std::string, InvertedIndex> invertedIndexes;
    std::map<std::string, std::map<uint32_t, std::vector<std::string>>> indexedDocumentFieldTerms;
    std::map<uint32_t, std::map<std::string, std::string>> storedFieldValues;
    std::vector<DocumentMeta> documentMetas;
    std::set<uint32_t> deletedDocIDs;

    uint32_t segmentID;

    std::filesystem::path path;
    bool dataModified, deletedDocIDModified;

    Segment();
    Segment(const Segment &) = delete;
    Segment(Segment &&) = delete;

    static std::shared_ptr<Segment> create(const std::filesystem::path &path, uint32_t segmentID);
    static std::shared_ptr<Segment> load(const std::filesystem::path &path, uint32_t segmentID);

    void addDocument(const Document &document);
    void deleteDocument(uint32_t docIDInSegment);

    friend class Index;

public:
    ~Segment();

    Document getDocument(uint32_t docIDInSegment) const;
    uint32_t getDocumentCount() const;

    const InvertedIndex &getInvertedIndex(const std::string &fieldName) const;
    const std::vector<std::string> &getIndexedDocumentFieldTerms(const std::string &fieldName, uint32_t docIDInSegment) const;
    uint32_t getTermOccuredDocumentCount(const std::string &fieldName, const std::string &term) const;
    uint32_t getTermOccuredCountInDocument(const std::string &fieldName, const std::string &term, uint32_t docIDInSegment) const;
    
    void save();
};

#endif // _MENCI_SEARCH_ENGINE_SEGMENT_H
