#ifndef _MENCI_SEARCH_ENGINE_META_H
#define _MENCI_SEARCH_ENGINE_META_H

#include <string>
#include <vector>

#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

#include "FieldType.h"

struct FieldMeta {
    std::string name;
    uint8_t attributes;

    template <typename Archive>
    void serialize(Archive &archive, unsigned int version) {
        archive & BOOST_SERIALIZATION_NVP(name);
        archive & BOOST_SERIALIZATION_NVP(attributes);
    }
};

struct DocumentMeta {
    std::vector<FieldMeta> fields;

    template <typename Archive>
    void serialize(Archive &archive, unsigned int version) {
        archive & BOOST_SERIALIZATION_NVP(fields);
    }
};

#endif // _MENCI_SEARCH_ENGINE_META_H
