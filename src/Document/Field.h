#ifndef _MENCI_SEARCH_ENGINE_FIELD_H
#define _MENCI_SEARCH_ENGINE_FIELD_H

#include <string>
#include <cstdint>

#include "FieldType.h"

class Field {
    std::string name;
    uint8_t attributes;
    std::string value;
    bool hasValue;

public:
    Field(const std::string &name, uint8_t attributes, const std::string &value);
    Field(const std::string &name, uint8_t attributes);

    const std::string &getValue() const;
    uint8_t getAttributes() const;
    const std::string &getName() const;
};

#endif // _MENCI_SEARCH_ENGINE_FIELD_H
