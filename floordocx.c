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

FComponent fdoc_component(const FComponentType type, const u32 len, const char *data) {
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

u32 read_u32(const char *buffer, u32 *bi) {
    u32 value = 0;
    for (u32 i = 0; i < sizeof(u32); i++)
        value |= (u32) (unsigned char) buffer[*bi + i] << (8 * i);

    *bi += sizeof(u32);
    return value;
}

FComponent read_fcomponent(const char *buffer, u32 *bi, const u32 buffer_len) {
    const FComponentType type = read_u32(buffer, bi);
    u32 len = read_u32(buffer, bi);

    if (*bi + len > buffer_len) return fdoc_component(FCTYPE_SKIP, 0, NULL);
    char *data = malloc(len);
    memcpy(data, buffer + *bi, len);
    *bi += len;
    return fdoc_component(type, len, data);
}

FDoc *fdoc_from_buffer(const char *buffer, const u32 len) {
    FDoc *doc = fdoc_new();
    u32 bi = 0;
    if (len < sizeof(u32)) return doc;
    const u32 components_len = read_u32(buffer, &bi);
    for (int ci = 0; ci < components_len; ci++) {
        if (bi + sizeof(u32) > len) break;
        fdoc_append(doc, read_fcomponent(buffer, &bi, len));
    }

    return doc;
}

// Continue

u32 fdoc_to_buffer(const FDoc *doc, char *buffer, const u32 len) {
    u32 bi = 0;
    if (len < sizeof(u32)) return 0;

    memcpy(buffer + bi, &doc->len, sizeof(u32));
    bi += sizeof(u32);
    for (u32 i = 0; i < doc->len; i++) {
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
