#include "Document.h"

Document::Document() {}

void Document::add(Field &&field) {
    add(std::make_shared<Field>(std::move(field)));
}

void Document::add(const Field &field) {
    add(std::make_shared<Field>(field));
}

void Document::add(std::shared_ptr<Field> field) {
    if (getField(field->getName()) != nullptr) {
        throw std::invalid_argument("Duplicated field name");
    }
    fields.push_back(field);
}

std::shared_ptr<Field> Document::getField(const std::string &name) {
    for (std::shared_ptr<Field> &field : fields) {
        if (field->getName() == name) return field;
    }

    return nullptr;
}

DocumentMeta Document::getMeta() const {
    DocumentMeta meta;
    for (const std::shared_ptr<Field> &field : fields) {
        FieldMeta fieldMeta;
        fieldMeta.name = field->getName();
        fieldMeta.attributes = field->getAttributes();
        meta.fields.push_back(fieldMeta);
    }

    return meta;
}
