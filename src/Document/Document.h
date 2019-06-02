#ifndef _MENCI_SEARCH_ENGINE_DOCUMENT_H
#define _MENCI_SEARCH_ENGINE_DOCUMENT_H

#include <cstdint>
#include <vector>

#include "Field.h"
#include "Meta.h"

class Document {
public:
    std::vector<std::shared_ptr<Field>> fields;

    Document();

    void add(Field &&field);
    void add(const Field &field);
    void add(std::shared_ptr<Field> field);
    std::shared_ptr<Field> getField(const std::string &fieldName);
    DocumentMeta getMeta() const;
};

#endif // _MENCI_SEARCH_ENGINE_DOCUMENT_H
