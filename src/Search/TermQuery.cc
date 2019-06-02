#include "TermQuery.h"

#include "AnalyzedPhraseQuery.h"

TermQuery::TermQuery(const std::string &fieldName, const std::string &term)
 : term(term),
   analyzedPhraseQuery(new AnalyzedPhraseQuery(fieldName, &term, &term + 1))
   {}

std::map<uint32_t, double> TermQuery::execute(const Segment &segment) const {
    return analyzedPhraseQuery->execute(segment);
}
