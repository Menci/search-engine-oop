#ifndef _MENCI_SEARCH_ENGINE_PHRASEQUERY_H
#define _MENCI_SEARCH_ENGINE_PHRASEQUERY_H

#include <string>
#include <vector>
#include <memory>

#include "Query.h"

class PhraseQuery : public Query {
    std::vector<std::string> terms;
    std::shared_ptr<Query> analyzedPhraseQuery;

public:
    PhraseQuery(const std::string &fieldName, const std::string &phrase);

protected:
    virtual std::map<uint32_t, double> execute(const Segment &segment) const;
};

#endif // _MENCI_SEARCH_ENGINE_PHRASEQUERY_H
