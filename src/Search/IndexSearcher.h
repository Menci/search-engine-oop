#ifndef _MENCI_SEARCH_ENGINE_INDEXSEARCHER_H
#define _MENCI_SEARCH_ENGINE_INDEXSEARCHER_H

#include <vector>

#include "SearchResultItem.h"
#include "Index/Index.h"
#include "Query.h"

class IndexSearcher {
    Index &index;

public:
    IndexSearcher(Index &index);

    std::vector<SearchResultItem> search(const Query &query, size_t topCount);
};

#endif // _MENCI_SEARCH_ENGINE_INDEXSEARCHER_H
