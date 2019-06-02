#ifndef _MENCI_SEARCH_ENGINE_TERMQUERY_H
#define _MENCI_SEARCH_ENGINE_TERMQUERY_H

#include <string>
#include <memory>

#include "Query.h"

class TermQuery : public Query {
    std::string term;
    std::shared_ptr<Query> analyzedPhraseQuery;

public:
    TermQuery(const std::string &fieldName, const std::string &term);

protected:
    virtual std::map<uint32_t, double> execute(const Segment &segment) const;
};

#endif // _MENCI_SEARCH_ENGINE_TERMQUERY_H
