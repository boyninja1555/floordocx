#ifndef FLOORDOCX_H
#define FLOORDOCX_H

#include <stdint.h>

typedef uint32_t u32;

typedef enum {
    // Optional data
    FCTYPE_META,
    FCTYPE_SKIP,

    // Important data
    FCTYPE_PLAINTEXT,
} FComponentType;

typedef struct {
    FComponentType type;
    u32 len;
    char *data;
} FComponent;

typedef struct {
    u32 len;
    u32 alloc;
    FComponent *components;
} FDoc;

FDoc *fdoc_new();

FComponent fdoc_component(FComponentType type, u32 len, const char *data);

void fdoc_append(FDoc *doc, FComponent component);

FDoc *fdoc_from_buffer(const char *buffer, u32 len);

u32 fdoc_to_buffer(const FDoc *doc, char *buffer, u32 len);

void fdoc_free(FDoc *doc);

#endif // FLOORDOCX_H
