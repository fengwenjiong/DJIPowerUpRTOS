#include "MV_PPM.h"
static void read_int_reset(ppm_read_settings_t *rs)
{
    rs->read_int_c_valid = false;
}

static void read_int(FIL *fp, uint32_t *i, ppm_read_settings_t *rs)
{
    enum { EAT_WHITESPACE, EAT_COMMENT, EAT_NUMBER } mode = EAT_WHITESPACE;
    for(*i = 0;;) {
        if (!rs->read_int_c_valid) {
            if (File_Module_TellWBuf(fp) == File_Module_SizeWBuf(fp)) return;
            File_Module_ReadByte(fp, &rs->read_int_c);
            rs->read_int_c_valid = true;
        }
        if (mode == EAT_WHITESPACE) {
            if (rs->read_int_c == '#') {
                mode = EAT_COMMENT;
            } else if (('0' <= rs->read_int_c) && (rs->read_int_c <= '9')) {
                *i = rs->read_int_c - '0';
                mode = EAT_NUMBER;
            }
        } else if (mode == EAT_COMMENT) {
            if ((rs->read_int_c == '\n') || (rs->read_int_c == '\r')) {
                mode = EAT_WHITESPACE;
            }
        } else if (mode == EAT_NUMBER) {
            if (('0' <= rs->read_int_c) && (rs->read_int_c <= '9')) {
                *i = (*i * 10) + rs->read_int_c - '0';
            } else {
                return; // read_int_c_valid==true on exit
            }
        }
        rs->read_int_c_valid = false;
    }
}

// This function inits the geometry values of an image.
void ppm_read_geometry(FIL *fp, image_t *img, const char *path, ppm_read_settings_t *rs)
{
    read_int_reset(rs);
    File_Module_CompareByte(fp, 'P');
    File_Module_ReadByte(fp, &rs->ppm_fmt);
    if ((rs->ppm_fmt!='2') && (rs->ppm_fmt!='3') && (rs->ppm_fmt!='5') && (rs->ppm_fmt!='6'))
       File_Module_Error(fp,"%s format error!",__FUNCTION__);
    img->bpp = ((rs->ppm_fmt == '2') || (rs->ppm_fmt == '5')) ? 1 : 2;

    read_int(fp, (uint32_t *) &img->w, rs);
    read_int(fp, (uint32_t *) &img->h, rs);
    if ((img->w == 0) || (img->h == 0))
        File_Module_Error(fp,"%s bmp w&h error!",__FUNCTION__);

    uint32_t max;
    read_int(fp, &max, rs);
    if (max != 255)
        File_Module_Error(fp,"%s format error!",__FUNCTION__);
}

// This function reads the pixel values of an image.
void ppm_read_pixels(FIL *fp, image_t *img, int line_start, int line_end, ppm_read_settings_t *rs)
{
    if (rs->ppm_fmt == '2') {
        for (int i = line_start; i < line_end; i++) {
            for (int j = 0; j < img->w; j++) {
                uint32_t pixel;
                read_int(fp, &pixel, rs);
                IM_SET_GS_PIXEL(img, j, i, pixel);
            }
        }
    } else if (rs->ppm_fmt == '3') {
        for (int i = line_start; i < line_end; i++) {
            for (int j = 0; j < img->w; j++) {
                uint32_t r, g, b;
                read_int(fp, &r, rs);
                read_int(fp, &g, rs);
                read_int(fp, &b, rs);
                IM_SET_RGB565_PIXEL(img, j, i, IM_RGB565(IM_R825(r),
                                                         IM_G826(g),
                                                         IM_B825(b)));
            }
        }
    } else if (rs->ppm_fmt == '5') {
        File_Module_ReadData(fp, // Super Fast - Zoom, Zoom!
                  img->pixels + (line_start * img->w),
                  (line_end - line_start) * img->w);
    } else if (rs->ppm_fmt == '6') {
        for (int i = line_start; i < line_end; i++) {
            for (int j = 0; j < img->w; j++) {
                uint8_t r, g, b;
                File_Module_ReadByte(fp, &r);
                File_Module_ReadByte(fp, &g);
                File_Module_ReadByte(fp, &b);
                IM_SET_RGB565_PIXEL(img, j, i, IM_RGB565(IM_R825(r),
                                                         IM_G826(g),
                                                         IM_B825(b)));
            }
        }
    }
}

void ppm_read(image_t *img, const char *path)
{
    FIL fp;
    ppm_read_settings_t rs;
    File_Module_OpenRead(&fp, path);
    File_Module_RWBufOn(&fp);
    ppm_read_geometry(&fp, img, path, &rs);
    if (!img->pixels)
        img->pixels = RTE_MEM_Alloc0(MEM_AXIM,img->w * img->h * img->bpp);
    ppm_read_pixels(&fp, img, 0, img->h, &rs);
    File_Module_RWBufOff(&fp);
    File_Module_Close(&fp);
}
