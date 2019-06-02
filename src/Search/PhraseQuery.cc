#include "PhraseQuery.h"

#include "AnalyzedPhraseQuery.h"
#include "Analysis/Analysis.h"

PhraseQuery::PhraseQuery(const std::string &fieldName, const std::string &phrase)
 : terms(Analysis::analysis(phrase)),
   analyzedPhraseQuery(new AnalyzedPhraseQuery(fieldName, terms.cbegin(), terms.cend()))
   {}

std::map<uint32_t, double> PhraseQuery::execute(const Segment &segment) const {
    return analyzedPhraseQuery->execute(segment);
}
