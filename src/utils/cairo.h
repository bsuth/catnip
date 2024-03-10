#ifndef CATNIP_UTILS_CAIRO_H
#define CATNIP_UTILS_CAIRO_H

#include <cairo.h>

void
cairo_set_source_hex(cairo_t* cr, int hex);

void
cairo_set_source_hexa(cairo_t* cr, int hex, double alpha);

#endif
