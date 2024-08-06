#include "vector.h"
#include <stdlib.h>

struct _catnip_vector {
  size_t length;
  size_t padding;
  size_t capacity;
  void** elements;
};

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

static void
catnip_vector_resize(struct _catnip_vector* vector, size_t new_capacity)
{
  void** new_elements = malloc(new_capacity * sizeof(void*));

  for (int i = 0; i < vector->length; ++i) {
    new_elements[i] = vector->elements[i];
  }

  free(vector->elements);

  vector->capacity = new_capacity;
  vector->elements = new_elements;
}

static int
catnip_vector_find(struct _catnip_vector* vector, void* element)
{
  for (int i = 0; i < vector->length; ++i) {
    if (vector->elements[i] == element) {
      return i;
    }
  }

  return -1;
}

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

struct _catnip_vector*
catnip_vector_create(size_t padding)
{
  struct _catnip_vector* vector = malloc(sizeof(struct _catnip_vector));

  vector->length = 0;
  vector->padding = padding;
  vector->capacity = padding;
  vector->elements = malloc(vector->capacity * sizeof(void*));

  return vector;
}

void
catnip_vector_destroy(struct _catnip_vector* vector)
{
  free(vector->elements);
  free(vector);
}

// -----------------------------------------------------------------------------
// Get
// -----------------------------------------------------------------------------

size_t
catnip_vector_get_length(struct _catnip_vector* vector)
{
  return vector->length;
}

// -----------------------------------------------------------------------------
// Methods
// -----------------------------------------------------------------------------

void*
catnip_vector_at(struct _catnip_vector* vector, size_t index)
{
  if (index < vector->length) {
    return vector->elements[index];
  } else {
    return NULL;
  }
}

void
catnip_vector_add(struct _catnip_vector* vector, void* element)
{
  int index = catnip_vector_find(vector, element);

  if (index != -1) {
    return; // already added
  }

  if (vector->capacity < vector->length + 1) {
    catnip_vector_resize(vector, vector->capacity + vector->padding);
  }

  vector->elements[vector->length] = element;
  ++vector->length;
}

void
catnip_vector_remove(struct _catnip_vector* vector, void* element)
{
  int index = catnip_vector_find(vector, element);

  if (index == -1) {
    return; // already removed
  }

  vector->elements[index] = vector->elements[vector->length - 1];
  --vector->length;

  if (vector->length < vector->capacity - 2 * vector->padding) {
    catnip_vector_resize(vector, vector->capacity - vector->padding);
  }
}
