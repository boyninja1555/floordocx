#include "floordocx.h"
#include <stdlib.h>
#include <string.h>

FDoc *fdoc_new() {
    FDoc *doc = malloc(sizeof(FDoc));
    doc->len = 0;
    doc->alloc = 4 * sizeof(FComponent);
    doc->components = malloc(doc->alloc);
    return doc;
}

FComponent fdoc_component(const FComponentType type, const size_t len, char *data) {
    return (FComponent){type, len, data};
}

void fdoc_append(FDoc *doc, const FComponent component) {
    if (doc->len >= doc->alloc) {
        doc->alloc *= 2;
        FComponent *new_components = realloc(doc->components, doc->alloc);
        if (new_components == NULL) {
            doc->alloc /= 2;
            return;
        }

        doc->components = new_components;
    }

    doc->components[doc->len] = component;
}

FDoc *fdoc_from_buffer(char *buffer, size_t len) {
}

char *fdoc_to_buffer(const FDoc *doc) {
    char *buffer = malloc(doc->len * sizeof(FComponent) + sizeof(size_t));
    buffer[0] = (char) doc->len;

    int bi = 0;
    for (int i = 0; i < doc->len; i++) {
        FComponent component = doc->components[i];

        // Component type
        bi++;
        memcpy(buffer + bi, &component.type, sizeof(FCTYPE_META));

        // Component length
        bi++;
        memcpy(buffer + bi, &component.len, sizeof(size_t));

        // Component data
        bi++;
        memcpy(buffer + bi, &component.data, component.len);
    }

    return buffer;
}

void fdoc_free(FDoc *doc) {
    if (doc->components != NULL) free(doc->components);
    free(doc);
}
