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

FComponent fdoc_component(const FComponentType type, const size_t len, const char *data) {
    return (FComponent){type, len, (char *) data};
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

// Section: FDoc from buffer

size_t read_size_t(const char *buffer, size_t *bi) {
    size_t value = 0;
    for (size_t i = 0; i < sizeof(size_t); i++)
        value |= (size_t) (unsigned char) buffer[*bi + i] << (8 * i);

    *bi += sizeof(size_t);
    return value;
}

FComponent read_fcomponent(const char *buffer, size_t *bi) {
    read_size_t(buffer, bi); // FIXME: Parse component type
    size_t len = read_size_t(buffer, bi);
    char *data = malloc(len);
    memcpy(data, buffer + *bi, len); // FIXME: Copy from buffer at offset of `bi` and length of `len` to `data`
    *bi += len;
    return fdoc_component(FCTYPE_SKIP, len, data); // FIXME: Parse real type instead of always SKIP component
}

FDoc *fdoc_from_buffer(const char *buffer, const size_t len) {
    FDoc *doc = fdoc_new();
    size_t bi = 0;
    if (len < sizeof(size_t)) return doc;
    const size_t components_len = read_size_t(buffer, &bi);
    for (int ci = 0; ci < components_len; ci++) {
        if (bi + sizeof(size_t) > len) break;
        fdoc_append(doc, read_fcomponent(buffer, &bi));
    }

    return doc;
}

// Continue

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
