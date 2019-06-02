#ifndef _MENCI_SEARCH_ENGINE_SEARCHRESULTITEM_H
#define _MENCI_SEARCH_ENGINE_SEARCHRESULTITEM_H

struct SearchResultItem {
    size_t docIDGlobal;
    double score;

    bool operator>(const SearchResultItem &rhs) const {
        return score > rhs.score;
    }
};

#endif // _MENCI_SEARCH_ENGINE_SEARCHRESULTITEM_H
