#ifndef _MENCI_SEARCH_ENGINE_QUERY_H
#define _MENCI_SEARCH_ENGINE_QUERY_H

#include <map>
#include <cstdint>

#include "Index/Segment.h"

class Query {
    friend class IndexSearcher;

    friend class PhraseQuery;
    friend class TermQuery;
    friend class BooleanAndQuery;
    friend class BooleanOrQuery;

public:
    virtual ~Query() = default;

protected:
    virtual std::map<uint32_t, double> execute(const Segment &segment) const = 0;
};

#endif // _MENCI_SEARCH_ENGINE_QUERY_H
