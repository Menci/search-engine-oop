#ifndef _MENCI_SEARCH_ENGINE_BOOLEANQUERY_H
#define _MENCI_SEARCH_ENGINE_BOOLEANQUERY_H

#include <algorithm>

#include "Query.h"

class BinaryBooleanQuery : public Query {
protected:
    std::shared_ptr<Query> q1;
    std::shared_ptr<Query> q2;

    BinaryBooleanQuery(std::shared_ptr<Query> q1, std::shared_ptr<Query> q2);
};

class BooleanAndQuery : public BinaryBooleanQuery {    
public:
    BooleanAndQuery(std::shared_ptr<Query> q1, std::shared_ptr<Query> q2) : BinaryBooleanQuery(q1, q2) {}

protected:
    virtual std::map<uint32_t, double> execute(const Segment &segment) const;
};

class BooleanOrQuery : public BinaryBooleanQuery {
public:
    BooleanOrQuery(std::shared_ptr<Query> q1, std::shared_ptr<Query> q2) : BinaryBooleanQuery(q1, q2) {}

protected:
    virtual std::map<uint32_t, double> execute(const Segment &segment) const;
};

#endif // _MENCI_SEARCH_ENGINE_BOOLEANQUERY_H
