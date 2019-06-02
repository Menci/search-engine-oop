#include "Segment.h"

#include <stdexcept>
#include <fstream>
#include <iostream>

#include <boost/format.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/vector.hpp>

#include "Analysis/Analysis.h"

constexpr const char *SEGMENT_DATA_FILE_NAME = "SegmentData%d.xml";
constexpr const char *SEGMENT_DELETED_DOCUMENTS_FILE_NAME = "SegmentDeletedDocuments%d.xml";

Segment::Segment() : dataModified(false), deletedDocIDModified(false) {}

Segment::~Segment() {
    save();
}

std::shared_ptr<Segment> Segment::create(const std::filesystem::path &path, uint32_t segmentID) {
    std::shared_ptr<Segment> segment(new Segment());
    segment->path = path;
    segment->dataModified = true;
    segment->deletedDocIDModified = true;
    return segment;
}

std::shared_ptr<Segment> Segment::load(const std::filesystem::path &path, uint32_t segmentID) {
    std::cerr << boost::format("Loading Segment %u") % segmentID << std::endl;

    std::shared_ptr<Segment> segment(new Segment());
    segment->path = path;

    std::string dataFileBaseName = boost::str(boost::format(SEGMENT_DATA_FILE_NAME) % segmentID);
    std::ifstream dataFileStream(path / dataFileBaseName);
    boost::archive::xml_iarchive dataFile(dataFileStream);

    dataFile & BOOST_SERIALIZATION_NVP(segment->invertedIndexes);
    dataFile & BOOST_SERIALIZATION_NVP(segment->indexedDocumentFieldTerms);
    dataFile & BOOST_SERIALIZATION_NVP(segment->storedFieldValues);
    dataFile & BOOST_SERIALIZATION_NVP(segment->documentMetas);

    std::string deletedDocumentsFileBaseName = boost::str(boost::format(SEGMENT_DELETED_DOCUMENTS_FILE_NAME) % segmentID);
    std::ifstream deletedDocumentsFileStream(path / deletedDocumentsFileBaseName);
    boost::archive::xml_iarchive deletedDocumentsFile(deletedDocumentsFileStream);

    deletedDocumentsFile & BOOST_SERIALIZATION_NVP(segment->deletedDocIDs);

    return segment;
}

Document Segment::getDocument(uint32_t docIDInSegment) const {
    if (docIDInSegment >= documentMetas.size()) {
        throw std::invalid_argument("No such document");
    }

    if (deletedDocIDs.count(docIDInSegment) != 0) {
        throw std::invalid_argument("Specfied document is deleted");
    }

    Document document;
    const DocumentMeta &meta = documentMetas[docIDInSegment];
    for (const FieldMeta &fieldMeta : meta.fields) {
        if (fieldMeta.attributes & FieldType::Stored) {
            document.add(std::make_shared<Field>(fieldMeta.name, fieldMeta.attributes, storedFieldValues.find(docIDInSegment)->second.find(fieldMeta.name)->second));
        } else {
            document.add(std::make_shared<Field>(fieldMeta.name, fieldMeta.attributes));
        }
    }

    return document;
}

uint32_t Segment::getDocumentCount() const {
    return documentMetas.size();
}

void Segment::deleteDocument(uint32_t docIDInSegment) {
    if (docIDInSegment >= documentMetas.size()) {
        throw std::invalid_argument("Document in-segment ID too large, no such segment");
    }

    deletedDocIDs.insert(docIDInSegment);
    deletedDocIDModified = true;
}

void Segment::addDocument(const Document &document) {
    size_t docIDInSegment = documentMetas.size();
    
    documentMetas.push_back(document.getMeta());

    for (const std::shared_ptr<Field> &field : document.fields) {
        if (field->getAttributes() & FieldType::Stored) {
            storedFieldValues[docIDInSegment][field->getName()] = field->getValue();
        }

        if (field->getAttributes() & FieldType::Indexed) {
            std::vector<std::string> terms = Analysis::analysis(field->getValue());
            invertedIndexes[field->getName()].addDocument(docIDInSegment, terms);
            indexedDocumentFieldTerms[field->getName()][docIDInSegment] = terms;
        }
    }
}

const InvertedIndex &Segment::getInvertedIndex(const std::string &fieldName) const {
    std::map<std::string, InvertedIndex>::const_iterator it = invertedIndexes.find(fieldName);
    if (it == invertedIndexes.end()) {
        static InvertedIndex emptyInvertedIndex;
        return emptyInvertedIndex;
    }

    return it->second;
}

const std::vector<std::string> &Segment::getIndexedDocumentFieldTerms(const std::string &fieldName, uint32_t docIDInSegment) const {
    static std::vector<std::string> emptyVector;

    std::map<std::string, std::map<uint32_t, std::vector<std::string>>>::const_iterator it = indexedDocumentFieldTerms.find(fieldName);
    if (it == indexedDocumentFieldTerms.end()) return emptyVector;

    const std::map<uint32_t, std::vector<std::string>> &indexedDocumentTerms = it->second;
    std::map<uint32_t, std::vector<std::string>>::const_iterator it2 = indexedDocumentTerms.find(docIDInSegment);
    if (it2 == indexedDocumentTerms.end()) return emptyVector;

    return it2->second;
}

uint32_t Segment::getTermOccuredDocumentCount(const std::string &fieldName, const std::string &term) const {
    return getInvertedIndex(fieldName).getTermOccuredDocumentCount(term);
}

uint32_t Segment::getTermOccuredCountInDocument(const std::string &fieldName, const std::string &term, uint32_t docIDInSegment) const {
    return getInvertedIndex(fieldName).getTermOccuredCountInDocument(term, docIDInSegment);
}

void Segment::save() {
    if (dataModified || deletedDocIDModified) {
        std::filesystem::create_directories(path);
    }

    if (dataModified) {
        std::string dataFileBaseName = boost::str(boost::format(SEGMENT_DATA_FILE_NAME) % segmentID);
        std::ofstream dataFileStream(path / dataFileBaseName);
        boost::archive::xml_oarchive dataFile(dataFileStream);

        dataFile & BOOST_SERIALIZATION_NVP(invertedIndexes);
        dataFile & BOOST_SERIALIZATION_NVP(indexedDocumentFieldTerms);
        dataFile & BOOST_SERIALIZATION_NVP(storedFieldValues);
        dataFile & BOOST_SERIALIZATION_NVP(documentMetas);

        dataModified = false;
    }

    if (deletedDocIDModified) {
        std::string deletedDocumentsBaseName = boost::str(boost::format(SEGMENT_DELETED_DOCUMENTS_FILE_NAME) % segmentID);
        std::ofstream deletedDocumentsFileStream(path / deletedDocumentsBaseName);
        boost::archive::xml_oarchive deletedDocumentsFile(deletedDocumentsFileStream);

        deletedDocumentsFile & BOOST_SERIALIZATION_NVP(deletedDocIDs);

        deletedDocIDModified = false;
    }
}
