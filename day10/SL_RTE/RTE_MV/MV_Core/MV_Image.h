#ifndef __MV_IMAGE_H
#define __MV_IMAGE_H
#include <stdint.h>
#include <stddef.h>
#include "RTE_MATH.h"
typedef enum image_bpp
{
	IMAGE_BPP_BINARY,       // BPP = 0
	IMAGE_BPP_GRAYSCALE,    // BPP = 1
	IMAGE_BPP_RGB565,       // BPP = 2
	IMAGE_BPP_BAYER,        // BPP = 3
	IMAGE_BPP_JPEG          // BPP > 3
}image_bpp_t;
typedef struct image{
	int w;
	int h;
	int bpp;
	union {
		uint8_t *pixels;
		uint8_t *data;
	};
}image_t;
void image_init(image_t *ptr, int w, int h, int bpp, void *data);
void image_copy(image_t *dst, image_t *src);
size_t image_size(image_t *ptr);
bool image_get_mask_pixel(image_t *ptr, int x, int y);
#define IMAGE_IS_MUTABLE(image) \
({ \
    __typeof__ (image) _image = (image); \
    (_image->bpp == IMAGE_BPP_BINARY) || \
    (_image->bpp == IMAGE_BPP_GRAYSCALE) || \
    (_image->bpp == IMAGE_BPP_RGB565); \
})

#define IMAGE_IS_MUTABLE_BAYER(image) \
({ \
    __typeof__ (image) _image = (image); \
    (_image->bpp == IMAGE_BPP_BINARY) || \
    (_image->bpp == IMAGE_BPP_GRAYSCALE) || \
    (_image->bpp == IMAGE_BPP_RGB565) || \
    (_image->bpp == IMAGE_BPP_BAYER); \
})

#define IMAGE_BINARY_LINE_LEN(image) (((image)->w + UINT32_T_MASK) >> UINT32_T_SHIFT)
#define IMAGE_BINARY_LINE_LEN_BYTES(image) (IMAGE_BINARY_LINE_LEN(image) * sizeof(uint32_t))

#define IMAGE_GRAYSCALE_LINE_LEN(image) ((image)->w)
#define IMAGE_GRAYSCALE_LINE_LEN_BYTES(image) (IMAGE_GRAYSCALE_LINE_LEN(image) * sizeof(uint8_t))

#define IMAGE_RGB565_LINE_LEN(image) ((image)->w)
#define IMAGE_RGB565_LINE_LEN_BYTES(image) (IMAGE_RGB565_LINE_LEN(image) * sizeof(uint16_t))

#define IMAGE_GET_BINARY_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    (((uint32_t *) _image->data)[(((_image->w + UINT32_T_MASK) >> UINT32_T_SHIFT) * _y) + (_x >> UINT32_T_SHIFT)] >> (_x & UINT32_T_MASK)) & 1; \
})

#define IMAGE_PUT_BINARY_PIXEL(image, x, y, v) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    __typeof__ (v) _v = (v); \
    size_t _i = (((_image->w + UINT32_T_MASK) >> UINT32_T_SHIFT) * _y) + (_x >> UINT32_T_SHIFT); \
    size_t _j = _x & UINT32_T_MASK; \
    ((uint32_t *) _image->data)[_i] = (((uint32_t *) _image->data)[_i] & (~(1 << _j))) | ((_v & 1) << _j); \
})

#define IMAGE_CLEAR_BINARY_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    ((uint32_t *) _image->data)[(((_image->w + UINT32_T_MASK) >> UINT32_T_SHIFT) * _y) + (_x >> UINT32_T_SHIFT)] &= ~(1 << (_x & UINT32_T_MASK)); \
})

#define IMAGE_SET_BINARY_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    ((uint32_t *) _image->data)[(((_image->w + UINT32_T_MASK) >> UINT32_T_SHIFT) * _y) + (_x >> UINT32_T_SHIFT)] |= 1 << (_x & UINT32_T_MASK); \
})

#define IMAGE_GET_GRAYSCALE_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    ((uint8_t *) _image->data)[(_image->w * _y) + _x]; \
})

#define IMAGE_PUT_GRAYSCALE_PIXEL(image, x, y, v) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    __typeof__ (v) _v = (v); \
    ((uint8_t *) _image->data)[(_image->w * _y) + _x] = _v; \
})

#define IMAGE_GET_RGB565_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    ((uint16_t *) _image->data)[(_image->w * _y) + _x]; \
})

#define IMAGE_PUT_RGB565_PIXEL(image, x, y, v) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    __typeof__ (v) _v = (v); \
    ((uint16_t *) _image->data)[(_image->w * _y) + _x] = _v; \
})

#ifdef __arm__
    #define IMAGE_REV_RGB565_PIXEL(pixel) \
    ({ \
        __typeof__ (pixel) _pixel = (pixel); \
        __REV16(_pixel); \
    })
#else
    #define IMAGE_REV_RGB565_PIXEL(pixel) \
    ({ \
        __typeof__ (pixel) _pixel = (pixel); \
        ((_pixel >> 8) | (_pixel << 8)) & 0xFFFF; \
    })
#endif

#define IMAGE_COMPUTE_TARGET_SIZE_SCALE_FACTOR(target_size, source_rect) \
__typeof__ (target_size) _target_size = (target_size); \
__typeof__ (source_rect) _source_rect = (source_rect); \
int IMAGE_X_SOURCE_OFFSET = _source_rect->p.x; \
int IMAGE_Y_SOURCE_OFFSET = _source_rect->p.y; \
int IMAGE_X_TARGET_OFFSET = 0; \
int IMAGE_Y_TARGET_OFFSET = 0; \
float IMAGE_X_RATIO = ((float) _source_rect->s.w) / ((float) _target_size->w); \
float IMAGE_Y_RATIO = ((float) _source_rect->s.h) / ((float) _target_size->h); \
({ 0; })

#define IMAGE_COMPUTE_TARGET_RECT_SCALE_FACTOR(target_rect, source_rect) \
__typeof__ (target_rect) _target_rect = (target_rect); \
__typeof__ (source_rect) _source_rect = (source_rect); \
int IMAGE_X_SOURCE_OFFSET = _source_rect->p.x; \
int IMAGE_Y_SOURCE_OFFSET = _source_rect->p.y; \
int IMAGE_X_TARGET_OFFSET = _target_rect->p.x; \
int IMAGE_Y_TARGET_OFFSET = _target_rect->p.y; \
float IMAGE_X_RATIO = ((float) _source_rect->s.w) / ((float) _target_rect->s.w); \
float IMAGE_Y_RATIO = ((float) _source_rect->s.h) / ((float) _target_rect->s.h); \
({ 0; })

#define IMAGE_GET_SCALED_BINARY_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    IMAGE_GET_BINARY_PIXEL(_image, ((size_t) ((IMAGE_X_RATIO * (_x - IMAGE_X_TARGET_OFFSET)) + 0.5)) + IMAGE_X_SOURCE_OFFSET, ((size_t) ((IMAGE_Y_RATIO * (_y - IMAGE_Y_TARGET_OFFSET)) + 0.5)) + IMAGE_Y_SOURCE_OFFSET); \
})

#define IMAGE_GET_SCALED_GRAYSCALE_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    IMAGE_GET_GRAYSCALE_PIXEL(_image, ((size_t) ((IMAGE_X_RATIO * (_x - IMAGE_X_TARGET_OFFSET)) + 0.5)) + IMAGE_X_SOURCE_OFFSET, ((size_t) ((IMAGE_Y_RATIO * (_y - IMAGE_Y_TARGET_OFFSET)) + 0.5)) + IMAGE_Y_SOURCE_OFFSET); \
})

#define IMAGE_GET_SCALED_RGB565_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    IMAGE_GET_RGB565_PIXEL(_image, ((size_t) ((IMAGE_X_RATIO * (_x - IMAGE_X_TARGET_OFFSET)) + 0.5)) + IMAGE_X_SOURCE_OFFSET, ((size_t) ((IMAGE_Y_RATIO * (_y - IMAGE_Y_TARGET_OFFSET)) + 0.5)) + IMAGE_Y_SOURCE_OFFSET); \
})

// Fast Stuff //

#define IMAGE_COMPUTE_BINARY_PIXEL_ROW_PTR(image, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (y) _y = (y); \
    ((uint32_t *) _image->data) + (((_image->w + UINT32_T_MASK) >> UINT32_T_SHIFT) * _y); \
})

#define IMAGE_INC_BINARY_PIXEL_ROW_PTR(row_ptr, image) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (image) _image = (image); \
    _row_ptr + ((_image->w + UINT32_T_MASK) >> UINT32_T_SHIFT); \
})

#define IMAGE_GET_BINARY_PIXEL_FAST(row_ptr, x) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    (_row_ptr[_x >> UINT32_T_SHIFT] >> (_x & UINT32_T_MASK)) & 1; \
})

#define IMAGE_PUT_BINARY_PIXEL_FAST(row_ptr, x, v) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    __typeof__ (v) _v = (v); \
    size_t _i = _x >> UINT32_T_SHIFT; \
    size_t _j = _x & UINT32_T_MASK; \
    _row_ptr[_i] = (_row_ptr[_i] & (~(1 << _j))) | ((_v & 1) << _j); \
})

#define IMAGE_CLEAR_BINARY_PIXEL_FAST(row_ptr, x) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    _row_ptr[_x >> UINT32_T_SHIFT] &= ~(1 << (_x & UINT32_T_MASK)); \
})

#define IMAGE_SET_BINARY_PIXEL_FAST(row_ptr, x) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    _row_ptr[_x >> UINT32_T_SHIFT] |= 1 << (_x & UINT32_T_MASK); \
})

#define IMAGE_COMPUTE_GRAYSCALE_PIXEL_ROW_PTR(image, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (y) _y = (y); \
    ((uint8_t *) _image->data) + (_image->w * _y); \
})

#define IMAGE_INC_GRAYSCALE_PIXEL_ROW_PTR(row_ptr, image) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (image) _image = (image); \
    row_ptr + _image->w; \
})

#define IMAGE_GET_GRAYSCALE_PIXEL_FAST(row_ptr, x) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    _row_ptr[_x]; \
})

#define IMAGE_PUT_GRAYSCALE_PIXEL_FAST(row_ptr, x, v) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    __typeof__ (v) _v = (v); \
    _row_ptr[_x] = _v; \
})

#define IMAGE_COMPUTE_RGB565_PIXEL_ROW_PTR(image, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (y) _y = (y); \
    ((uint16_t *) _image->data) + (_image->w * _y); \
})

#define IMAGE_INC_RGB565_PIXEL_ROW_PTR(row_ptr, image) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (image) _image = (image); \
    row_ptr + _image->w; \
})

#define IMAGE_GET_RGB565_PIXEL_FAST(row_ptr, x) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    _row_ptr[_x]; \
})

#define IMAGE_PUT_RGB565_PIXEL_FAST(row_ptr, x, v) \
({ \
    __typeof__ (row_ptr) _row_ptr = (row_ptr); \
    __typeof__ (x) _x = (x); \
    __typeof__ (v) _v = (v); \
    _row_ptr[_x] = _v; \
})

#define IMAGE_COMPUTE_SCALED_BINARY_PIXEL_ROW_PTR(image, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (y) _y = (y); \
    ((uint32_t *) _image->data) + (((_image->w + UINT32_T_MASK) >> UINT32_T_SHIFT) * (((size_t) ((IMAGE_Y_RATIO * (_y - IMAGE_Y_TARGET_OFFSET)) + 0.5)) + IMAGE_Y_SOURCE_OFFSET)); \
})

#define IMAGE_GET_SCALED_BINARY_PIXEL_FAST(row_ptr, x) IMAGE_GET_BINARY_PIXEL_FAST((row_ptr), ((size_t) ((IMAGE_X_RATIO * ((x) - IMAGE_X_TARGET_OFFSET)) + 0.5)) + IMAGE_X_SOURCE_OFFSET)

#define IMAGE_COMPUTE_SCALED_GRAYSCALE_PIXEL_ROW_PTR(image, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (y) _y = (y); \
    ((uint8_t *) _image->data) + (_image->w * (((size_t) ((IMAGE_Y_RATIO * (_y - IMAGE_Y_TARGET_OFFSET)) + 0.5)) + IMAGE_Y_SOURCE_OFFSET)); \
})

#define IMAGE_GET_SCALED_GRAYSCALE_PIXEL_FAST(row_ptr, x) IMAGE_GET_GRAYSCALE_PIXEL_FAST((row_ptr), ((size_t) ((IMAGE_X_RATIO * ((x) - IMAGE_X_TARGET_OFFSET)) + 0.5)) + IMAGE_X_SOURCE_OFFSET)

#define IMAGE_COMPUTE_SCALED_RGB565_PIXEL_ROW_PTR(image, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (y) _y = (y); \
    ((uint16_t *) _image->data) + (_image->w * (((size_t) ((IMAGE_Y_RATIO * (_y - IMAGE_Y_TARGET_OFFSET)) + 0.5)) + IMAGE_Y_SOURCE_OFFSET)); \
})

#define IMAGE_GET_SCALED_RGB565_PIXEL_FAST(row_ptr, x) IMAGE_GET_RGB565_PIXEL_FAST((row_ptr), ((size_t) ((IMAGE_X_RATIO * ((x) - IMAGE_X_TARGET_OFFSET)) + 0.5)) + IMAGE_X_SOURCE_OFFSET)

// Old Image Macros - Will be refactor and removed. But, only after making sure through testing new macros work.

#define IM_SWAP16(x) __REV16(x) // Swap bottom two chars in short.
#define IM_SWAP32(x) __REV32(x) // Swap bottom two shorts in long.

// RGB565 to RGB888 conversion.
extern const uint8_t rb528_table[32];
extern const uint8_t g628_table[64];

#define IM_R528(p) ({ __typeof__ (p) _p = (p); rb528_table[_p]; })

#define IM_G628(p) ({ __typeof__ (p) _p = (p); g628_table[_p]; })

#define IM_B528(p) ({ __typeof__ (p) _p = (p); rb528_table[_p]; })

// RGB888 to RGB565 conversion.
extern const uint8_t rb825_table[256];
extern const uint8_t g826_table[256];
#define IM_R825(p) ({ __typeof__ (p) _p = (p); rb825_table[_p]; })
#define IM_G826(p) ({ __typeof__ (p) _p = (p); g826_table[_p]; })
#define IM_B825(p) ({ __typeof__ (p) _p = (p); rb825_table[_p]; })

// Split RGB565 values (note the RGB565 value is byte reversed).
#define IM_R565(p) ({ __typeof__ (p) _p = (p); ((_p)>>3)&0x1F; })
#define IM_G565(p) ({ __typeof__ (p) _p = (p); (((_p)&0x07)<<3)|((_p)>>13); })
#define IM_B565(p) ({ __typeof__ (p) _p = (p); ((_p)>>8)&0x1F; })

// Merge RGB565 values (note the RGB565 value is byte reversed).
#define IM_RGB565(r, g, b) \
    ({ __typeof__ (r) _r = (r); \
       __typeof__ (g) _g = (g); \
       __typeof__ (b) _b = (b); \
       ((_r)<<3)|((_g)>>3)|((_g)<<13)|((_b)<<8); })

// Image kernels
extern const int8_t kernel_gauss_3[9];
extern const int8_t kernel_gauss_5[25];
extern const int kernel_laplacian_3[9];
extern const int kernel_high_pass_3[9];
#define IM_RGB5652L(p) ({ __typeof__ (p) _p = (p); lab_table[_p * 3]; })
#define IM_RGB5652A(p) ({ __typeof__ (p) _p = (p); lab_table[(_p * 3) + 1]; })
#define IM_RGB5652B(p) ({ __typeof__ (p) _p = (p); lab_table[(_p * 3) + 2]; })

// Grayscale maxes
#define IM_MAX_GS (255)

// RGB565 maxes
#define IM_MAX_R5 (31)
#define IM_MAX_G6 (63)
#define IM_MAX_B5 (31)

// Grayscale histogram
#define IM_G_HIST_SIZE (256)
#define IM_G_HIST_OFFSET (0)

// LAB histogram
#define IM_L_HIST_SIZE (256)
#define IM_L_HIST_OFFSET (0)
#define IM_A_HIST_SIZE (256)
#define IM_A_HIST_OFFSET (256)
#define IM_B_HIST_SIZE (256)
#define IM_B_HIST_OFFSET (512)

#define IM_IS_BINARY(img) ({ __typeof__ (img) _img = (img); _img->bpp == 0; })
#define IM_IS_GS(img) ({ __typeof__ (img) _img = (img); _img->bpp == 1; })
#define IM_IS_RGB565(img) ({ __typeof__ (img) _img = (img); _img->bpp == 2; })
#define IM_IS_BAYER(img) ({ __typeof__ (img) _img = (img); _img->bpp == 3; })
#define IM_IS_JPEG(img) ({ __typeof__ (img) _img = (img); _img->bpp >= 4; })
#define IM_IS_MUTABLE(img) ({ __typeof__ (img) _img = (img); (_img->bpp == 1 || _img->bpp == 2); })
#define IM_X_INSIDE(img, x) ({ __typeof__ (img) _img = (img); __typeof__ (x) _x = (x); (0<=_x)&&(_x<_img->w); })
#define IM_Y_INSIDE(img, y) ({ __typeof__ (img) _img = (img); __typeof__ (y) _y = (y); (0<=_y)&&(_y<_img->h); })

#define IM_GET_GS_PIXEL(img, x, y) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       ((uint8_t*)_img->pixels)[(_y*_img->w)+_x]; })

#define IM_GET_RAW_PIXEL(img, x, y) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       ((uint8_t*)_img->pixels)[(_y*_img->w)+_x]; })

#define IM_GET_RAW_PIXEL_CHECK_BOUNDS_X(img, x, y) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       _x = (_x < 0) ? 0 : (_x >= img->w) ? (img->w -1): _x; \
       ((uint8_t*)_img->pixels)[(_y*_img->w)+_x]; })

#define IM_GET_RAW_PIXEL_CHECK_BOUNDS_Y(img, x, y) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       _y = (_y < 0) ? 0 : (_y >= img->h) ? (img->h -1): _y; \
       ((uint8_t*)_img->pixels)[(_y*_img->w)+_x]; })

#define IM_GET_RAW_PIXEL_CHECK_BOUNDS_XY(img, x, y) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       _x = (_x < 0) ? 0 : (_x >= img->w) ? (img->w -1): _x; \
       _y = (_y < 0) ? 0 : (_y >= img->h) ? (img->h -1): _y; \
       ((uint8_t*)_img->pixels)[(_y*_img->w)+_x]; })

#define IM_GET_RGB565_PIXEL(img, x, y) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       ((uint16_t*)_img->pixels)[(_y*_img->w)+_x]; })

#define IM_SET_GS_PIXEL(img, x, y, p) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       __typeof__ (p) _p = (p); \
       ((uint8_t*)_img->pixels)[(_y*_img->w)+_x]=_p; })

#define IM_SET_RGB565_PIXEL(img, x, y, p) \
    ({ __typeof__ (img) _img = (img); \
       __typeof__ (x) _x = (x); \
       __typeof__ (y) _y = (y); \
       __typeof__ (p) _p = (p); \
       ((uint16_t*)_img->pixels)[(_y*_img->w)+_x]=_p; })

#define IM_EQUAL(img0, img1) \
    ({ __typeof__ (img0) _img0 = (img0); \
       __typeof__ (img1) _img1 = (img1); \
       (_img0->w==_img1->w)&&(_img0->h==_img1->h)&&(_img0->bpp==_img1->bpp); })

#define IM_TO_GS_PIXEL(img, x, y)    \
    (img->bpp == 1 ? img->pixels[((y)*img->w)+(x)] : (COLOR_RGB565_TO_Y(((uint16_t*)img->pixels)[((y)*img->w)+(x)]) + 128))

// Drawing Functions
int imlib_get_pixel(image_t *img, int x, int y);
void imlib_set_pixel(image_t *img, int x, int y, int p);
void imlib_draw_line(image_t *img, int x0, int y0, int x1, int y1, int c, int thickness);
void imlib_draw_rectangle(image_t *img, int rx, int ry, int rw, int rh, int c, int thickness, bool fill);
void imlib_draw_circle(image_t *img, int cx, int cy, int r, int c, int thickness, bool fill);
void imlib_draw_ellipse(image_t *img, int cx, int cy, int rx, int ry, int rotation, int c, int thickness, bool fill);
void imlib_draw_string(image_t *img, int x_off, int y_off, const char *str, int c, float scale, int x_spacing, int y_spacing, bool mono_space);
void imlib_draw_image(image_t *img, image_t *other, int x_off, int y_off, float x_scale, float y_scale, float alpha, image_t *mask);

#endif
