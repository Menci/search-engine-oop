#ifndef _MENCI_SEARCH_ENGINE_ANALYZEDPHRASEQUERY_H
#define _MENCI_SEARCH_ENGINE_ANALYZEDPHRASEQUERY_H

#include <stdexcept>
#include <cmath>
#include <string>

#include <boost/range.hpp>

#include "Query.h"
#include "Index/Segment.h"

template <typename Iter>
class AnalyzedPhraseQuery : public Query {
    std::string fieldName;
    Iter phraseBegin;
    Iter phraseEnd;
    size_t phraseLength;

public:
    AnalyzedPhraseQuery(const std::string &fieldName, Iter phraseBegin, Iter phraseEnd) : fieldName(fieldName), phraseBegin(phraseBegin), phraseEnd(phraseEnd) {
        if (phraseBegin == phraseEnd) {
            throw std::invalid_argument("Phrase must not be empty");
        }

        phraseLength = std::distance(phraseBegin, phraseEnd);
    }

private:
    bool checkFollowingTerms(const Segment &segment, uint32_t docIDInSegment, const std::list<uint32_t> &firstTermPositions) const {
        Iter followingBegin = phraseBegin;
        followingBegin++;

        if (followingBegin == phraseEnd) return true;

        const std::vector<std::string> &documentTerms = segment.getIndexedDocumentFieldTerms(fieldName, docIDInSegment);
        for (uint32_t firstTermPosition : firstTermPositions) {
            if (firstTermPosition + phraseLength > documentTerms.size()) continue;

            bool mismatched = false;
            Iter it = followingBegin;
            for (uint32_t i = 1; i < phraseLength; i++) {
                if (documentTerms[firstTermPosition + i] != *it++) {
                    mismatched = true;
                    break;
                }
            }

            if (!mismatched) return true;
        }

        return false;
    }

protected:
    virtual std::map<uint32_t, double> execute(const Segment &segment) const {
        const InvertedIndex &invertedIndex = segment.getInvertedIndex(fieldName);
        const std::map<uint32_t, std::list<uint32_t>> &postingList = invertedIndex.getPostingList(*phraseBegin);

        std::map<uint32_t, double> result;
        for (auto &[docIDInSegment, firstTermPositions] : postingList) {
            if (checkFollowingTerms(segment, docIDInSegment, firstTermPositions)) {
                uint32_t documentTermsCount = segment.getIndexedDocumentFieldTerms(fieldName, docIDInSegment).size();
                double score = 0;
                for (const std::string &term : boost::make_iterator_range(phraseBegin, phraseEnd)) {
                    double tf = (double)invertedIndex.getOccurPositionsInDocument(term, docIDInSegment).size() / documentTermsCount;
                    double idf = log10(1 + (double)invertedIndex.getPostingList(term).size() / segment.getDocumentCount());
                    score += tf * idf;
                }

                result[docIDInSegment] = score;
            }
        }

        return result;
    }
};

#endif // _MENCI_SEARCH_ENGINE_ANALYZEDPHRASEQUERY_H
