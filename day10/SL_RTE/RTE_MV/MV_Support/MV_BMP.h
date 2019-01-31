#ifndef __MV_BMP_H
#define __MV_BMP_H
#include "MV_Support/MV_File.h"
bool bmp_read_geometry(FIL *fp, image_t *img, const char *path, bmp_read_settings_t *rs);
void bmp_read_pixels(FIL *fp, image_t *img, int line_start, int line_end, bmp_read_settings_t *rs);
void bmp_read(image_t *img, const char *path);
#endif // __MV_BMP_H
