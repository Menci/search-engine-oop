#include "Field.h"

#include <stdexcept>

Field::Field(const std::string &name, uint8_t attributes, const std::string &value)
 : name(name), attributes(attributes), value(value), hasValue(true) {}

Field::Field(const std::string &name, uint8_t attributes)
 : name(name), attributes(attributes), value(""), hasValue(false) {}

const std::string &Field::getValue() const {
    if (!hasValue) {
        throw std::invalid_argument("Field has no value");
    }
    return value;
}

uint8_t Field::getAttributes() const {
    return attributes;
}

const std::string &Field::getName() const {
    return name;
}
