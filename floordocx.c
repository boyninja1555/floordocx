#include "floordocx.h"
#include <stdlib.h>
#include <string.h>

FDoc *fdoc_new() {
    FDoc *doc = malloc(sizeof(FDoc));
    doc->len = 0;
    doc->alloc = 4;
    doc->components = malloc(doc->alloc * sizeof(FComponent));
    return doc;
}

FComponent fdoc_component(const FComponentType type, const size_t len, char *data) {
    return (FComponent){type, len, data};
}

void fdoc_append(FDoc *doc, const FComponent component) {
    if (doc->len >= doc->alloc) {
        doc->alloc *= 2;
        FComponent *new_components = realloc(doc->components, doc->alloc * sizeof(FComponent));
        if (new_components == NULL) {
            doc->alloc /= 2;
            return;
        }

        doc->components = new_components;
    }

    doc->components[doc->len++] = component;
}

FDoc *fdoc_from_buffer(char *buffer, size_t len) {
}

size_t fdoc_to_buffer(const FDoc *doc, char *buffer, const size_t len) {
    size_t bi = 0;
    if (len < sizeof(size_t)) return 0;

    memcpy(buffer + bi, &doc->len, sizeof(size_t));
    bi += sizeof(size_t);
    for (size_t i = 0; i < doc->len; i++) {
        FComponent component = doc->components[i];
        if (bi + sizeof(component.type) + sizeof(component.len) + component.len > len)
            return 0;

        // Component type
        memcpy(buffer + bi, &component.type, sizeof(component.type));
        bi += sizeof(component.type);

        // Component length
        memcpy(buffer + bi, &component.len, sizeof(component.len));
        bi += sizeof(component.len);

        // Component data
        memcpy(buffer + bi, component.data, component.len);
        bi += component.len;
    }

    return bi;
}

void fdoc_free(FDoc *doc) {
    if (doc->components != NULL) free(doc->components);
    free(doc);
}
