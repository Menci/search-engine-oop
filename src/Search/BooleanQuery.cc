#include "BooleanQuery.h"

BinaryBooleanQuery::BinaryBooleanQuery(std::shared_ptr<Query> q1, std::shared_ptr<Query> q2) : q1(q1), q2(q2) {}

std::map<uint32_t, double> BooleanAndQuery::execute(const Segment &segment) const {
    std::map<uint32_t, double> r1 = q1->execute(segment), r2 = q2->execute(segment), result;
    for (auto [docIDInSegment, score] : r1) {
        if (r2.find(docIDInSegment) != r2.end()) {
            result[docIDInSegment] = std::max(r1[docIDInSegment], r2[docIDInSegment]);
        }
    }

    return result;
}

std::map<uint32_t, double> BooleanOrQuery::execute(const Segment &segment) const {
    std::map<uint32_t, double> r1 = q1->execute(segment), r2 = q2->execute(segment), result;

    for (auto [docIDInSegment, score] : r1) {
        if (r2.find(docIDInSegment) != r2.end()) {
            result[docIDInSegment] = std::max(r1[docIDInSegment], r2[docIDInSegment]);
        }
    }

    for (auto [docIDInSegment, score] : r1) {
        if (r2.find(docIDInSegment) == r2.end()) {
            result[docIDInSegment] = r1[docIDInSegment];
        }
    }

    for (auto [docIDInSegment, score] : r2) {
        if (r1.find(docIDInSegment) == r1.end()) {
            result[docIDInSegment] = r2[docIDInSegment];
        }
    }

    return result;
}
