#ifndef CATNIP_OBJECTS_VECTOR_H
#define CATNIP_OBJECTS_VECTOR_H

#include <stddef.h>

typedef struct _catnip_vector catnip_vector_t;

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

catnip_vector_t*
catnip_vector_create(size_t padding);

void
catnip_vector_destroy(catnip_vector_t* vector);

// -----------------------------------------------------------------------------
// Get
// -----------------------------------------------------------------------------

size_t
catnip_vector_get_length(catnip_vector_t* vector);

// -----------------------------------------------------------------------------
// Methods
// -----------------------------------------------------------------------------

void*
catnip_vector_at(struct _catnip_vector* vector, size_t index);

void
catnip_vector_add(catnip_vector_t* vector, void* element);

void
catnip_vector_remove(catnip_vector_t* vector, void* element);

#endif
