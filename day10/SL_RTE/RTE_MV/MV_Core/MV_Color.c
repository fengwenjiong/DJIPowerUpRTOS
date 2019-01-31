#include "MV_Color.h"
#ifndef MV_FASTMATH_H
#define fast_cbrtf cbrtf
#define fast_roundf roundf
#define fast_floorf floorf
#endif // MV_FASTMATH_H
// Gamma uncompress
extern const float xyz_table[256];

const int8_t kernel_gauss_3[3*3] = {
     1, 2, 1,
     2, 4, 2,
     1, 2, 1,
};

const int8_t kernel_gauss_5[5*5] = {
    1,  4,  6,  4, 1,
    4, 16, 24, 16, 4,
    6, 24, 36, 24, 6,
    4, 16, 24, 16, 4,
    1,  4,  6,  4, 1
};

const int kernel_laplacian_3[3*3] = {
     -1, -1, -1,
     -1,  8, -1,
     -1, -1, -1
};

const int kernel_high_pass_3[3*3] = {
    -1, -1, -1,
    -1, +8, -1,
    -1, -1, -1
};

int8_t imlib_rgb565_to_l(uint16_t pixel)
{
    float r_lin = xyz_table[COLOR_RGB565_TO_R8(pixel)];
    float g_lin = xyz_table[COLOR_RGB565_TO_G8(pixel)];
    float b_lin = xyz_table[COLOR_RGB565_TO_B8(pixel)];

    float y = ((r_lin * 0.2126f) + (g_lin * 0.7152f) + (b_lin * 0.0722f)) * (1.0f / 100.000f);
    y = (y>0.008856f) ? fast_cbrtf(y) : ((y * 7.787037f) + 0.137931f);
    return fast_roundf(116 * y) - 16;
}

int8_t imlib_rgb565_to_a(uint16_t pixel)
{
    float r_lin = xyz_table[COLOR_RGB565_TO_R8(pixel)];
    float g_lin = xyz_table[COLOR_RGB565_TO_G8(pixel)];
    float b_lin = xyz_table[COLOR_RGB565_TO_B8(pixel)];

    float x = ((r_lin * 0.4124f) + (g_lin * 0.3576f) + (b_lin * 0.1805f)) * (1.0f / 095.047f);
    float y = ((r_lin * 0.2126f) + (g_lin * 0.7152f) + (b_lin * 0.0722f)) * (1.0f / 100.000f);

    x = (x>0.008856f) ? fast_cbrtf(x) : ((x * 7.787037f) + 0.137931f);
    y = (y>0.008856f) ? fast_cbrtf(y) : ((y * 7.787037f) + 0.137931f);

    return fast_roundf(500 * (x-y));
}

int8_t imlib_rgb565_to_b(uint16_t pixel)
{
    float r_lin = xyz_table[COLOR_RGB565_TO_R8(pixel)];
    float g_lin = xyz_table[COLOR_RGB565_TO_G8(pixel)];
    float b_lin = xyz_table[COLOR_RGB565_TO_B8(pixel)];

    float y = ((r_lin * 0.2126f) + (g_lin * 0.7152f) + (b_lin * 0.0722f)) * (1.0f / 100.000f); \
    float z = ((r_lin * 0.0193f) + (g_lin * 0.1192f) + (b_lin * 0.9505f)) * (1.0f / 108.883f); \

    y = (y>0.008856f) ? fast_cbrtf(y) : ((y * 7.787037f) + 0.137931f);
    z = (z>0.008856f) ? fast_cbrtf(z) : ((z * 7.787037f) + 0.137931f);

    return fast_roundf(200 * (y-z));
}

int8_t imlib_rgb565_to_y(uint16_t pixel)
{
    int r = COLOR_RGB565_TO_R8(pixel);
    int g = COLOR_RGB565_TO_G8(pixel);
    int b = COLOR_RGB565_TO_B8(pixel);
    return fast_floorf((r * +0.299000f) + (g * +0.587000f) + (b * +0.114000f)) - 128;
}

int8_t imlib_rgb565_to_u(uint16_t pixel)
{
    int r = COLOR_RGB565_TO_R8(pixel);
    int g = COLOR_RGB565_TO_G8(pixel);
    int b = COLOR_RGB565_TO_B8(pixel);
    return fast_floorf((r * -0.168736f) + (g * -0.331264f) + (b * +0.500000f));
}

int8_t imlib_rgb565_to_v(uint16_t pixel)
{
    int r = COLOR_RGB565_TO_R8(pixel);
    int g = COLOR_RGB565_TO_G8(pixel);
    int b = COLOR_RGB565_TO_B8(pixel);
    return fast_floorf((r * +0.500000f) + (g * -0.418688f) + (b * -0.081312f));
}

void imlib_rgb_to_lab(simple_color_t *rgb, simple_color_t *lab)
{
    // https://en.wikipedia.org/wiki/SRGB -> Specification of the transformation
    // https://en.wikipedia.org/wiki/Lab_color_space -> CIELAB-CIEXYZ conversions

    float r_lin = xyz_table[rgb->red];
    float g_lin = xyz_table[rgb->green];
    float b_lin = xyz_table[rgb->blue];

    float x = ((r_lin * 0.4124f) + (g_lin * 0.3576f) + (b_lin * 0.1805f)) / 095.047f;
    float y = ((r_lin * 0.2126f) + (g_lin * 0.7152f) + (b_lin * 0.0722f)) / 100.000f;
    float z = ((r_lin * 0.0193f) + (g_lin * 0.1192f) + (b_lin * 0.9505f)) / 108.883f;

    x = (x>0.008856f) ? fast_cbrtf(x) : ((x * 7.787037f) + 0.137931f);
    y = (y>0.008856f) ? fast_cbrtf(y) : ((y * 7.787037f) + 0.137931f);
    z = (z>0.008856f) ? fast_cbrtf(z) : ((z * 7.787037f) + 0.137931f);

    lab->L = ((int8_t) fast_roundf(116 * y)) - 16;
    lab->A = ((int8_t) fast_roundf(500 * (x-y)));
    lab->B = ((int8_t) fast_roundf(200 * (y-z)));
}

void imlib_lab_to_rgb(simple_color_t *lab, simple_color_t *rgb)
{
    // https://en.wikipedia.org/wiki/Lab_color_space -> CIELAB-CIEXYZ conversions
    // https://en.wikipedia.org/wiki/SRGB -> Specification of the transformation

    float x = ((lab->L + 16) * 0.008621f) + (lab->A * 0.002f);
    float y = ((lab->L + 16) * 0.008621f);
    float z = ((lab->L + 16) * 0.008621f) - (lab->B * 0.005f);

    x = ((x>0.206897f) ? (x*x*x) : ((0.128419f * x) - 0.017713f)) * 095.047f;
    y = ((y>0.206897f) ? (y*y*y) : ((0.128419f * y) - 0.017713f)) * 100.000f;
    z = ((z>0.206897f) ? (z*z*z) : ((0.128419f * z) - 0.017713f)) * 108.883f;

    float r_lin = ((x * +3.2406f) + (y * -1.5372f) + (z * -0.4986f)) / 100.0f;
    float g_lin = ((x * -0.9689f) + (y * +1.8758f) + (z * +0.0415f)) / 100.0f;
    float b_lin = ((x * +0.0557f) + (y * -0.2040f) + (z * +1.0570f)) / 100.0f;

    r_lin = (r_lin>0.0031308f) ? ((1.055f*powf(r_lin, 0.416666f))-0.055f) : (r_lin*12.92f);
    g_lin = (g_lin>0.0031308f) ? ((1.055f*powf(g_lin, 0.416666f))-0.055f) : (g_lin*12.92f);
    b_lin = (b_lin>0.0031308f) ? ((1.055f*powf(b_lin, 0.416666f))-0.055f) : (b_lin*12.92f);

    rgb->red   = MATH_MAX(MATH_MIN(fast_roundf(r_lin * 255), 255), 0);
    rgb->green = MATH_MAX(MATH_MIN(fast_roundf(g_lin * 255), 255), 0);
    rgb->blue  = MATH_MAX(MATH_MIN(fast_roundf(b_lin * 255), 255), 0);
}

void imlib_rgb_to_grayscale(simple_color_t *rgb, simple_color_t *grayscale)
{
    float r_lin = xyz_table[rgb->red];
    float g_lin = xyz_table[rgb->green];
    float b_lin = xyz_table[rgb->blue];
    float y = ((r_lin * 0.2126f) + (g_lin * 0.7152f) + (b_lin * 0.0722f)) / 100.0f;
    y = (y>0.0031308f) ? ((1.055f*powf(y, 0.416666f))-0.055f) : (y*12.92f);
    grayscale->G = MATH_MAX(MATH_MIN(fast_roundf(y * 255), 255), 0);
}

// Just copy settings back.
void imlib_grayscale_to_rgb(simple_color_t *grayscale, simple_color_t *rgb)
{
    rgb->red   = grayscale->G;
    rgb->green = grayscale->G;
    rgb->blue  = grayscale->G;
}

inline uint16_t imlib_yuv_to_rgb(uint8_t y, int8_t u, int8_t v)
{
    uint32_t r = MATH_MAX(MATH_MIN(y + ((91881*v)>>16), 255), 0);
    uint32_t g = MATH_MAX(MATH_MIN(y - (((22554*u)+(46802*v))>>16), 255), 0);
    uint32_t b = MATH_MAX(MATH_MIN(y + ((116130*u)>>16), 255), 0);
    return IM_RGB565(IM_R825(r), IM_G826(g), IM_B825(b));
}

void imlib_bayer_to_rgb565(image_t *img, int w, int h, int xoffs, int yoffs, uint16_t *rgbbuf)
{
    int r, g, b;
    for (int y=yoffs; y<yoffs+h; y++) {
        for (int x=xoffs; x<xoffs+w; x++) {
            if ((y % 2) == 0) { // Even row
                if ((x % 2) == 0) { // Even col
                    r = (IM_GET_RAW_PIXEL_CHECK_BOUNDS_XY(img, x-1, y-1)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_XY(img, x+1, y-1)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_XY(img, x-1, y+1)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_XY(img, x+1, y+1)) >> 2;

                    g = (IM_GET_RAW_PIXEL_CHECK_BOUNDS_Y(img, x, y-1)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_Y(img, x, y+1)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_X(img, x-1, y)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_X(img, x+1, y)) >> 2;

                    b = IM_GET_RAW_PIXEL(img,  x, y);
                } else { // Odd col
                    r = (IM_GET_RAW_PIXEL_CHECK_BOUNDS_Y(img, x, y-1)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_Y(img, x, y+1)) >> 1;

                    b = (IM_GET_RAW_PIXEL_CHECK_BOUNDS_X(img, x-1, y)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_X(img, x+1, y)) >> 1;

                    g =  IM_GET_RAW_PIXEL(img, x, y);
                }
            } else { // Odd row
                if ((x % 2) == 0) { // Even Col
                    r = (IM_GET_RAW_PIXEL_CHECK_BOUNDS_X(img, x-1, y)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_X(img, x+1, y)) >> 1;

                    g =  IM_GET_RAW_PIXEL(img, x, y);

                    b = (IM_GET_RAW_PIXEL_CHECK_BOUNDS_Y(img, x, y-1)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_Y(img, x, y+1)) >> 1;
                } else { // Odd col
                    r = IM_GET_RAW_PIXEL(img,  x, y);

                    g = (IM_GET_RAW_PIXEL_CHECK_BOUNDS_Y(img, x, y-1)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_Y(img, x, y+1)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_X(img, x-1, y)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_X(img, x+1, y)) >> 2;

                    b = (IM_GET_RAW_PIXEL_CHECK_BOUNDS_XY(img, x-1, y-1)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_XY(img, x+1, y-1)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_XY(img, x-1, y+1)  +
                         IM_GET_RAW_PIXEL_CHECK_BOUNDS_XY(img, x+1, y+1)) >> 2;
                }

            }
            r = IM_R825(r);
            g = IM_G826(g);
            b = IM_B825(b);
            *rgbbuf++ = IM_RGB565(r, g, b);
        }
    }
}
