#ifndef __MV_PPM_H
#define __MV_PPM_H
#include "MV_Support/MV_File.h"
void ppm_read_geometry(FIL *fp, image_t *img, const char *path, ppm_read_settings_t *rs);
void ppm_read_pixels(FIL *fp, image_t *img, int line_start, int line_end, ppm_read_settings_t *rs);
void ppm_read(image_t *img, const char *path);
#endif // __MV_PPM_H
