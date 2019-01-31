#ifndef __MV_COLOR_H
#define __MV_COLOR_H
#include "MV_Core/MV_Image.h"
/////////////////
// Color Stuff //
/////////////////

typedef struct color_thresholds_list_lnk_data
{
    uint8_t LMin, LMax; // or grayscale
    int8_t AMin, AMax;
    int8_t BMin, BMax;
}
color_thresholds_list_lnk_data_t;

#define COLOR_THRESHOLD_BINARY(pixel, threshold, invert) \
({ \
    __typeof__ (pixel) _pixel = (pixel); \
    __typeof__ (threshold) _threshold = (threshold); \
    __typeof__ (invert) _invert = (invert); \
    ((_threshold->LMin <= _pixel) && (_pixel <= _threshold->LMax)) ^ _invert; \
})

#define COLOR_THRESHOLD_GRAYSCALE(pixel, threshold, invert) \
({ \
    __typeof__ (pixel) _pixel = (pixel); \
    __typeof__ (threshold) _threshold = (threshold); \
    __typeof__ (invert) _invert = (invert); \
    ((_threshold->LMin <= _pixel) && (_pixel <= _threshold->LMax)) ^ _invert; \
})

#define COLOR_THRESHOLD_RGB565(pixel, threshold, invert) \
({ \
    __typeof__ (pixel) _pixel = (pixel); \
    __typeof__ (threshold) _threshold = (threshold); \
    __typeof__ (invert) _invert = (invert); \
    uint8_t _l = COLOR_RGB565_TO_L(_pixel); \
    int8_t _a = COLOR_RGB565_TO_A(_pixel); \
    int8_t _b = COLOR_RGB565_TO_B(_pixel); \
    ((_threshold->LMin <= _l) && (_l <= _threshold->LMax) && \
    (_threshold->AMin <= _a) && (_a <= _threshold->AMax) && \
    (_threshold->BMin <= _b) && (_b <= _threshold->BMax)) ^ _invert; \
})

#define COLOR_BOUND_BINARY(pixel0, pixel1, threshold) \
({ \
    __typeof__ (pixel0) _pixel0 = (pixel0); \
    __typeof__ (pixel1) _pixel1 = (pixel1); \
    __typeof__ (threshold) _threshold = (threshold); \
    (abs(_pixel0 - _pixel1) <= _threshold); \
})

#define COLOR_BOUND_GRAYSCALE(pixel0, pixel1, threshold) \
({ \
    __typeof__ (pixel0) _pixel0 = (pixel0); \
    __typeof__ (pixel1) _pixel1 = (pixel1); \
    __typeof__ (threshold) _threshold = (threshold); \
    (abs(_pixel0 - _pixel1) <= _threshold); \
})

#define COLOR_BOUND_RGB565(pixel0, pixel1, threshold) \
({ \
    __typeof__ (pixel0) _pixel0 = (pixel0); \
    __typeof__ (pixel1) _pixel1 = (pixel1); \
    __typeof__ (threshold) _threshold = (threshold); \
    (abs(COLOR_RGB565_TO_R5(_pixel0) - COLOR_RGB565_TO_R5(_pixel1)) <= COLOR_RGB565_TO_R5(_threshold)) && \
    (abs(COLOR_RGB565_TO_G6(_pixel0) - COLOR_RGB565_TO_G6(_pixel1)) <= COLOR_RGB565_TO_G6(_threshold)) && \
    (abs(COLOR_RGB565_TO_B5(_pixel0) - COLOR_RGB565_TO_B5(_pixel1)) <= COLOR_RGB565_TO_B5(_threshold)); \
})

#define COLOR_BINARY_MIN 0
#define COLOR_BINARY_MAX 1
#define COLOR_GRAYSCALE_BINARY_MIN 0x00
#define COLOR_GRAYSCALE_BINARY_MAX 0xFF
#define COLOR_RGB565_BINARY_MIN 0x0000
#define COLOR_RGB565_BINARY_MAX 0xFFFF

#define COLOR_GRAYSCALE_MIN 0
#define COLOR_GRAYSCALE_MAX 255

#define COLOR_R5_MIN 0
#define COLOR_R5_MAX 31
#define COLOR_G6_MIN 0
#define COLOR_G6_MAX 63
#define COLOR_B5_MIN 0
#define COLOR_B5_MAX 31

#define COLOR_R8_MIN 0
#define COLOR_R8_MAX 255
#define COLOR_G8_MIN 0
#define COLOR_G8_MAX 255
#define COLOR_B8_MIN 0
#define COLOR_B8_MAX 255

#define COLOR_L_MIN 0
#define COLOR_L_MAX 100
#define COLOR_A_MIN -128
#define COLOR_A_MAX 127
#define COLOR_B_MIN -128
#define COLOR_B_MAX 127

#define COLOR_Y_MIN -128
#define COLOR_Y_MAX 127
#define COLOR_U_MIN -128
#define COLOR_U_MAX 127
#define COLOR_V_MIN -128
#define COLOR_V_MAX 127

extern const uint8_t rb528_table[32];
extern const uint8_t g628_table[64];

#define COLOR_R5_TO_R8(color) rb528_table[color]
#define COLOR_G6_TO_G8(color) g628_table[color]
#define COLOR_B5_TO_B8(color) rb528_table[color]

extern const uint8_t rb825_table[256];
extern const uint8_t g826_table[256];

#define COLOR_R8_TO_R5(color) rb825_table[color]
#define COLOR_G8_TO_G6(color) g826_table[color]
#define COLOR_B8_TO_B5(color) rb825_table[color]

// RGB565 Stuff //

#define COLOR_RGB565_TO_R5(pixel) (((pixel) >> 3) & 0x1F)
#define COLOR_RGB565_TO_G6(pixel) \
({ \
    __typeof__ (pixel) _pixel = (pixel); \
    ((_pixel & 0x07) << 3) | (_pixel >> 13); \
})
#define COLOR_RGB565_TO_B5(pixel) (((pixel) >> 8) & 0x1F)
#define COLOR_RGB565_TO_R8(pixel) COLOR_R5_TO_R8(COLOR_RGB565_TO_R5(pixel))
#define COLOR_RGB565_TO_G8(pixel) COLOR_G6_TO_G8(COLOR_RGB565_TO_G6(pixel))
#define COLOR_RGB565_TO_B8(pixel) COLOR_B5_TO_B8(COLOR_RGB565_TO_B5(pixel))

#define COLOR_R5_G6_B5_TO_RGB565(r5, g6, b5) \
({ \
    __typeof__ (r5) _r5 = (r5); \
    __typeof__ (g6) _g6 = (g6); \
    __typeof__ (b5) _b5 = (b5); \
    (_r5 << 3) | (_g6 >> 3) | ((_g6 & 0x7) << 13) | (_b5 << 8); \
})

#define COLOR_R8_G8_B8_TO_RGB565(r8, g8, b8) COLOR_R5_G6_B5_TO_RGB565(COLOR_R8_TO_R5(r8), COLOR_G8_TO_G6(g8), COLOR_B8_TO_B5(b8))

extern const int8_t lab_table[196608];
extern const int8_t yuv_table[196608];

#ifdef IMLIB_ENABLE_LAB_LUT
#define COLOR_RGB565_TO_L(pixel) lab_table[(pixel) * 3]
#define COLOR_RGB565_TO_A(pixel) lab_table[((pixel) * 3) + 1]
#define COLOR_RGB565_TO_B(pixel) lab_table[((pixel) * 3) + 2]
#else
#define COLOR_RGB565_TO_L(pixel) imlib_rgb565_to_l(pixel)
#define COLOR_RGB565_TO_A(pixel) imlib_rgb565_to_a(pixel)
#define COLOR_RGB565_TO_B(pixel) imlib_rgb565_to_b(pixel)
#endif

#ifdef IMLIB_ENABLE_YUV_LUT
#define COLOR_RGB565_TO_Y(pixel) yuv_table[(pixel) * 3]
#define COLOR_RGB565_TO_U(pixel) yuv_table[((pixel) * 3) + 1]
#define COLOR_RGB565_TO_V(pixel) yuv_table[((pixel) * 3) + 2]
#else
#define COLOR_RGB565_TO_Y(pixel) imlib_rgb565_to_y(pixel)
#define COLOR_RGB565_TO_U(pixel) imlib_rgb565_to_u(pixel)
#define COLOR_RGB565_TO_V(pixel) imlib_rgb565_to_v(pixel)
#endif

// https://en.wikipedia.org/wiki/Lab_color_space -> CIELAB-CIEXYZ conversions
// https://en.wikipedia.org/wiki/SRGB -> Specification of the transformation

#define COLOR_LAB_TO_RGB565(l, a, b) \
({ \
    __typeof__ (l) _l = (l); \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    float _x = ((_l + 16) * 0.008621f) + (_a * 0.002f); \
    float _y = ((_l + 16) * 0.008621f); \
    float _z = ((_l + 16) * 0.008621f) - (_b * 0.005f); \
    _x = ((_x > 0.206897f) ? (_x * _x * _x) : ((0.128419f * _x) - 0.017713f)) * 095.047f; \
    _y = ((_y > 0.206897f) ? (_y * _y * _y) : ((0.128419f * _y) - 0.017713f)) * 100.000f; \
    _z = ((_z > 0.206897f) ? (_z * _z * _z) : ((0.128419f * _z) - 0.017713f)) * 108.883f; \
    float _r_lin = ((_x * +3.2406f) + (_y * -1.5372f) + (_z * -0.4986f)) / 100.0f; \
    float _g_lin = ((_x * -0.9689f) + (_y * +1.8758f) + (_z * +0.0415f)) / 100.0f; \
    float _b_lin = ((_x * +0.0557f) + (_y * -0.2040f) + (_z * +1.0570f)) / 100.0f; \
    _r_lin = (_r_lin > 0.0031308f) ? ((1.055f * powf(_r_lin, 0.416666f)) - 0.055f) : (_r_lin * 12.92f); \
    _g_lin = (_g_lin > 0.0031308f) ? ((1.055f * powf(_g_lin, 0.416666f)) - 0.055f) : (_g_lin * 12.92f); \
    _b_lin = (_b_lin > 0.0031308f) ? ((1.055f * powf(_b_lin, 0.416666f)) - 0.055f) : (_b_lin * 12.92f); \
    unsigned int _rgb565_r = MATH_MAX(MATH_MIN(roundf(_r_lin * COLOR_R5_MAX), COLOR_R5_MAX), COLOR_R5_MIN); \
    unsigned int _rgb565_g = MATH_MAX(MATH_MIN(roundf(_g_lin * COLOR_G6_MAX), COLOR_G6_MAX), COLOR_G6_MIN); \
    unsigned int _rgb565_b = MATH_MAX(MATH_MIN(roundf(_b_lin * COLOR_B5_MAX), COLOR_B5_MAX), COLOR_B5_MIN); \
    COLOR_R5_G6_B5_TO_RGB565(_rgb565_r, _rgb565_g, _rgb565_b); \
})

// https://en.wikipedia.org/wiki/YCbCr -> JPEG Conversion

#define COLOR_YUV_TO_RGB565(y, u, v) \
({ \
    __typeof__ (y) _y = (y); \
    __typeof__ (u) _u = (u); \
    __typeof__ (v) _v = (v); \
    unsigned int _r = MATH_MAX(MATH_MIN(128 + _y + ((((uint32_t) ((1.402000 * 65536) + 0.5)) * _v) >> 16), COLOR_R8_MAX), COLOR_R8_MIN); \
    unsigned int _g = MATH_MAX(MATH_MIN(128 + _y - (((((uint32_t) ((0.344136 * 65536) + 0.5)) * _u) + (((uint32_t) ((0.714136 * 65536) + 0.5)) * _v)) >> 16), COLOR_G8_MAX), COLOR_G8_MIN); \
    unsigned int _b = MATH_MAX(MATH_MIN(128 + _y + ((((uint32_t) ((1.772000 * 65536) + 0.5)) * _u) >> 16), COLOR_B8_MAX), COLOR_B8_MIN); \
    COLOR_R8_G8_B8_TO_RGB565(_r, _g, _b); \
})

#define COLOR_BAYER_TO_RGB565(img, x, y, r, g, b)            \
({                                                           \
    __typeof__ (x) __x = (x);                                \
    __typeof__ (y) __y = (y);                                \
    if ((__y % 2) == 0) {                                    \
        if ((__x % 2) == 0) {                                \
            r = (IM_GET_RAW_PIXEL(img, __x-1, __y-1)  +      \
                 IM_GET_RAW_PIXEL(img, __x+1, __y-1)  +      \
                 IM_GET_RAW_PIXEL(img, __x-1, __y+1)  +      \
                 IM_GET_RAW_PIXEL(img, __x+1, __y+1)) >> 2;  \
                                                             \
            g = (IM_GET_RAW_PIXEL(img, __x,   __y-1)  +      \
                 IM_GET_RAW_PIXEL(img, __x,   __y+1)  +      \
                 IM_GET_RAW_PIXEL(img, __x-1, __y)    +      \
                 IM_GET_RAW_PIXEL(img, __x+1, __y))   >> 2;  \
                                                             \
            b = IM_GET_RAW_PIXEL(img,  __x, __y);            \
        } else {                                             \
            r = (IM_GET_RAW_PIXEL(img, __x, __y-1)  +        \
                 IM_GET_RAW_PIXEL(img, __x, __y+1)) >> 1;    \
                                                             \
            b = (IM_GET_RAW_PIXEL(img, __x-1, __y)  +        \
                 IM_GET_RAW_PIXEL(img, __x+1, __y)) >> 1;    \
                                                             \
            g =  IM_GET_RAW_PIXEL(img, __x, __y);            \
        }                                                    \
    } else {                                                 \
        if ((__x % 2) == 0) {                                \
            r = (IM_GET_RAW_PIXEL(img, __x-1, __y)  +        \
                 IM_GET_RAW_PIXEL(img, __x+1, __y)) >> 1;    \
                                                             \
            g =  IM_GET_RAW_PIXEL(img, __x, __y);            \
                                                             \
            b = (IM_GET_RAW_PIXEL(img, __x, __y-1)  +        \
                 IM_GET_RAW_PIXEL(img, __x, __y+1)) >> 1;    \
        } else {                                             \
            r = IM_GET_RAW_PIXEL(img,  __x, __y);            \
                                                             \
            g = (IM_GET_RAW_PIXEL(img, __x, __y-1)    +      \
                 IM_GET_RAW_PIXEL(img, __x, __y+1)    +      \
                 IM_GET_RAW_PIXEL(img, __x-1, __y)    +      \
                 IM_GET_RAW_PIXEL(img, __x+1, __y))   >> 2;  \
                                                             \
            b = (IM_GET_RAW_PIXEL(img, __x-1, __y-1)  +      \
                 IM_GET_RAW_PIXEL(img, __x+1, __y-1)  +      \
                 IM_GET_RAW_PIXEL(img, __x-1, __y+1)  +      \
                 IM_GET_RAW_PIXEL(img, __x+1, __y+1)) >> 2;  \
        }                                                    \
    }                                                        \
    r  = IM_R825(r);                                         \
    g  = IM_G826(g);                                         \
    b  = IM_B825(b);                                         \
})

#define COLOR_BINARY_TO_GRAYSCALE(pixel) ((pixel) * COLOR_GRAYSCALE_MAX)
#define COLOR_BINARY_TO_RGB565(pixel) COLOR_YUV_TO_RGB565((pixel) ? 127 : -128, 0, 0)
#define COLOR_RGB565_TO_BINARY(pixel) (COLOR_RGB565_TO_Y(pixel) > (((COLOR_Y_MAX - COLOR_Y_MIN) / 2) + COLOR_Y_MIN))
#define COLOR_RGB565_TO_GRAYSCALE(pixel) (COLOR_RGB565_TO_Y(pixel) + 128)
#define COLOR_GRAYSCALE_TO_BINARY(pixel) ((pixel) > (((COLOR_GRAYSCALE_MAX - COLOR_GRAYSCALE_MIN) / 2) + COLOR_GRAYSCALE_MIN))
#define COLOR_GRAYSCALE_TO_RGB565(pixel) COLOR_YUV_TO_RGB565((pixel) - 128, 0, 0)
typedef struct simple_color {
    uint8_t G;          // Gray
    union {
        int8_t L;       // LAB L
        uint8_t red;    // RGB888 Red
    };
    union {
        int8_t A;       // LAB A
        uint8_t green;  // RGB888 Green
    };
    union {
        int8_t B;       // LAB B
        uint8_t blue;   // RGB888 Blue
    };
} simple_color_t;
/* Color space functions */
int8_t imlib_rgb565_to_l(uint16_t pixel);
int8_t imlib_rgb565_to_a(uint16_t pixel);
int8_t imlib_rgb565_to_b(uint16_t pixel);
int8_t imlib_rgb565_to_y(uint16_t pixel);
int8_t imlib_rgb565_to_u(uint16_t pixel);
int8_t imlib_rgb565_to_v(uint16_t pixel);
void imlib_rgb_to_lab(simple_color_t *rgb, simple_color_t *lab);
void imlib_lab_to_rgb(simple_color_t *lab, simple_color_t *rgb);
void imlib_rgb_to_grayscale(simple_color_t *rgb, simple_color_t *grayscale);
void imlib_grayscale_to_rgb(simple_color_t *grayscale, simple_color_t *rgb);
uint16_t imlib_yuv_to_rgb(uint8_t y, int8_t u, int8_t v);
void imlib_bayer_to_rgb565(image_t *img, int w, int h, int xoffs, int yoffs, uint16_t *rgbbuf);
#endif // __MV_COLOR_H
