#include "Index.h"

#include <fstream>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

constexpr const char *META_FILE_NAME = "Meta.xml";
constexpr size_t SEGMENT_CACHE_SIZE = 3;
constexpr uint32_t DEFAULT_DOCUMENTS_PER_SEGMENT = 100;

Index::Index(const std::string &path) : path(path), segmentCache(SEGMENT_CACHE_SIZE), modified(false) {
    std::filesystem::path metaFilePath = this->path / META_FILE_NAME;

    std::ifstream metaFileStream(metaFilePath);
    if (!metaFileStream) {
        segmentsCount = documentsCount = 0;
        documentsPerSegment = DEFAULT_DOCUMENTS_PER_SEGMENT;
    } else {
        boost::archive::xml_iarchive metaFile(metaFileStream);

        metaFile & BOOST_SERIALIZATION_NVP(segmentsCount);
        metaFile & BOOST_SERIALIZATION_NVP(documentsCount);
        metaFile & BOOST_SERIALIZATION_NVP(documentsPerSegment);
    }
}

Index::~Index() {
    save();
}

uint32_t Index::getSegmentsCount() const {
    return segmentsCount;
}

uint32_t Index::getDocumentsCount() const {
    return documentsCount;
}

uint32_t Index::getDocumentsPerSegment() const {
    return documentsPerSegment;
}

uint32_t Index::toGlobalDocID(uint32_t docIDInSegment, uint32_t segmentID) const {
    return docIDInSegment + documentsPerSegment * segmentID;
}

void Index::toInSegmentDocID(uint32_t docIDGlobal, uint32_t &segmentID, uint32_t &docIDInSegment) const {
    segmentID = docIDGlobal / documentsPerSegment;
    docIDInSegment = docIDGlobal - (segmentID * documentsPerSegment);
}

std::shared_ptr<Segment> Index::getSegment(uint32_t segmentID) {
    if (segmentID >= segmentsCount) {
        throw std::invalid_argument("No such segment");
    }

    if (boost::optional<std::shared_ptr<Segment>> cached = segmentCache.get(segmentID)) {
        return cached.get();
    } else {
        std::shared_ptr<Segment> loadedSegment = Segment::load(path, segmentID);
        loadedSegment->segmentID = segmentID;
        segmentCache.insert(segmentID, loadedSegment);
        return loadedSegment;
    }
}

Document Index::getDocument(uint32_t docIDGlobal) {
    uint32_t segmentID, docIDInSegment;
    toInSegmentDocID(docIDGlobal, segmentID, docIDInSegment);

    if (segmentID >= segmentsCount) {
        throw std::invalid_argument("Document global ID too large, no such segment");
    }

    std::shared_ptr<Segment> segment = getSegment(segmentID);

    return segment->getDocument(docIDInSegment);
}

void Index::deleteDocument(uint32_t docIDGlobal) {
    uint32_t segmentID, docIDInSegment;
    toInSegmentDocID(docIDGlobal, segmentID, docIDInSegment);

    if (segmentID >= segmentsCount) {
        throw std::invalid_argument("Document global ID too large, no such segment");
    }

    std::shared_ptr<Segment> segment = getSegment(segmentID);

    segment->deleteDocument(docIDInSegment);

    modified = true;
}

uint32_t Index::addDocument(const Document &document) {
    std::shared_ptr<Segment> segment;
    if (documentsCount % documentsPerSegment == 0) {
        // Last segment is full, create a new one
        uint32_t segmentID = segmentsCount++;
        segment = Segment::create(path, segmentID);
        segment->segmentID = segmentID;
        segmentCache.insert(segmentID, segment);
    } else {
        // Last segment is not full, append to last segment
        segment = getSegment(segmentsCount - 1);
    }

    segment->addDocument(document);

    modified = true;

    return documentsCount++;
}

void Index::save() {
    if (modified) {
        std::filesystem::create_directories(this->path);

        std::filesystem::path metaFilePath = this->path / META_FILE_NAME;

        std::ofstream metaFileStream(metaFilePath);
        boost::archive::xml_oarchive metaFile(metaFileStream);

        metaFile & BOOST_SERIALIZATION_NVP(segmentsCount);
        metaFile & BOOST_SERIALIZATION_NVP(documentsCount);
        metaFile & BOOST_SERIALIZATION_NVP(documentsPerSegment);

        modified = false;
    }
}
