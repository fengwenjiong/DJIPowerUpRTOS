#ifndef __MV_FILE_H
#define __MV_FILE_H
#include "MV_Core/MV_Image.h"
#include "File_Module.h"
typedef struct bmp_read_settings {
    int32_t bmp_w;
    int32_t bmp_h;
    uint16_t bmp_bpp;
    uint32_t bmp_fmt;
    uint32_t bmp_row_bytes;
} bmp_read_settings_t;
typedef struct ppm_read_settings {
    uint8_t read_int_c;
    bool read_int_c_valid;
    uint8_t ppm_fmt;
} ppm_read_settings_t;
typedef enum save_image_format {
    FORMAT_DONT_CARE,
    FORMAT_BMP,
    FORMAT_PNM,
    FORMAT_JPG,
    FORMAT_RAW,
} save_image_format_t;
typedef struct img_read_settings {
    union
    {
        bmp_read_settings_t bmp_rs;
        ppm_read_settings_t ppm_rs;
    };
    save_image_format_t format;
} img_read_settings_t;


#endif // __MV_FILE_H
