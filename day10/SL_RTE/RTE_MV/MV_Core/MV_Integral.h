#ifndef __MV_INTEGRAL_H
#define __MV_INTEGRAL_H
#include "MV_Core/MV_Image.h"
typedef struct integral_image {
    int w;
    int h;
    uint32_t *data;
} i_image_t;
/* Integral image functions */
void imlib_integral_image_alloc(struct integral_image *sum, int w, int h);
void imlib_integral_image_free(struct integral_image *sum);
void imlib_integral_image(struct image *src, struct integral_image *sum);
void imlib_integral_image_sq(struct image *src, struct integral_image *sum);
void imlib_integral_image_scaled(struct image *src, struct integral_image *sum);
uint32_t imlib_integral_lookup(struct integral_image *src, int x, int y, int w, int h);
#endif // __MV_INTEGRAL_H

