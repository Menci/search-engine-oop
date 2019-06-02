#include "IndexSearcher.h"

#include <queue>

#include <boost/range/adaptor/reversed.hpp>

#include "Index/Segment.h"

IndexSearcher::IndexSearcher(Index &index) : index(index) {}

std::vector<SearchResultItem> IndexSearcher::search(const Query &query, size_t topCount) {
    std::priority_queue<SearchResultItem, std::vector<SearchResultItem>, std::greater<SearchResultItem>> heap;
    for (uint32_t i = 0; i < index.getSegmentsCount(); i++) {
        std::shared_ptr<Segment> segment = index.getSegment(i);
        for (auto [docIDInSegment, score] : query.execute(*segment)) {
            if (heap.size() == topCount) {
                if (heap.top().score >= score) continue;
                else heap.pop();
            }

            heap.push({index.toGlobalDocID(docIDInSegment, i), score});
        }
    }

    std::vector<SearchResultItem> result(heap.size());
    for (SearchResultItem &item : boost::adaptors::reverse(result)) {
        item = heap.top();
        heap.pop();
    }

    return result;
}
