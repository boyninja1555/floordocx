#ifndef FLOORDOCX_H
#define FLOORDOCX_H

#include <stddef.h>

typedef enum {
    // Optional data
    FCTYPE_META,
    FCTYPE_SKIP,

    // Important data
    FCTYPE_PLAINTEXT,
} FComponentType;

typedef struct {
    FComponentType type;
    size_t len;
    char *data;
} FComponent;

typedef struct {
    size_t len;
    size_t alloc;
    FComponent *components;
} FDoc;

FDoc *fdoc_new();

void fdoc_append(FDoc *doc, FComponent component);

FDoc *fdoc_from_buffer(char *buffer, size_t len);

char *fdoc_to_buffer(const FDoc *doc);

void fdoc_free(FDoc *doc);

#endif // FLOORDOCX_H
