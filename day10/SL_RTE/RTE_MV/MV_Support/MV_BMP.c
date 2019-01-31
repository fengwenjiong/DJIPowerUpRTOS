#include "MV_BMP.h"
// This function inits the geometry values of an image (opens file).
bool bmp_read_geometry(FIL *fp, image_t *img, const char *path, bmp_read_settings_t *rs)
{
    File_Module_CompareByte(fp, 'B');
    File_Module_CompareByte(fp, 'M');

    uint32_t file_size;
    File_Module_ReadLong(fp, &file_size);
    File_Module_IgnoreWord(fp);
    File_Module_IgnoreWord(fp);

    uint32_t header_size;
    File_Module_ReadLong(fp, &header_size);
    if (file_size <= header_size)
        File_Module_Error(fp,"%s format error!",__FUNCTION__);

    uint32_t data_size = file_size - header_size;
    if (data_size % 4)
        File_Module_Error(fp,"%s data_size error!",__FUNCTION__);

    File_Module_CompareLong(fp, 40);
    File_Module_ReadLong(fp, (uint32_t*) &rs->bmp_w);
    File_Module_ReadLong(fp, (uint32_t*) &rs->bmp_h);
    if ((rs->bmp_w == 0) || (rs->bmp_h == 0))
        File_Module_Error(fp,"%s bmp w&h error!",__FUNCTION__);
    img->w = MATH_ABS(rs->bmp_w);
    img->h = MATH_ABS(rs->bmp_h);

    File_Module_CompareWord(fp, 1);
    File_Module_ReadWord(fp, &rs->bmp_bpp);
    if ((rs->bmp_bpp != 8) && (rs->bmp_bpp != 16) && (rs->bmp_bpp != 24))
        File_Module_Error(fp,"%s format error!",__FUNCTION__);
    img->bpp = (rs->bmp_bpp == 8) ? 1 : 2;

    File_Module_ReadLong(fp, &rs->bmp_fmt);
    if ((rs->bmp_fmt != 0) && (rs->bmp_fmt != 3))
        File_Module_Error(fp,"%s format error!",__FUNCTION__);

    File_Module_IgnoreLong(fp);
    File_Module_IgnoreLong(fp);
    File_Module_IgnoreLong(fp);
    File_Module_IgnoreLong(fp);
    File_Module_IgnoreLong(fp);

    if (rs->bmp_bpp == 8) {
        if (rs->bmp_fmt != 0)
            File_Module_Error(fp,"%s format error!",__FUNCTION__);
        // Color Table (1024 bytes)
        for (int i = 0; i < 256; i++) {
            File_Module_CompareLong(fp, ((i) << 16) | ((i) << 8) | i);
        }
    } else if (rs->bmp_bpp == 16) {
        if (rs->bmp_fmt != 3)
            File_Module_Error(fp,"%s format error!",__FUNCTION__);
        // Bit Masks (12 bytes)
        File_Module_CompareLong(fp, 0x1F << 11);
        File_Module_CompareLong(fp, 0x3F << 5);
        File_Module_CompareLong(fp, 0x1F);
    } else if (rs->bmp_bpp == 24) {
        if (rs->bmp_fmt == 3) {
            // Bit Masks (12 bytes)
            File_Module_CompareLong(fp, 0xFF << 16);
            File_Module_CompareLong(fp, 0xFF << 8);
            File_Module_CompareLong(fp, 0xFF);
        }
    }

    rs->bmp_row_bytes = (((img->w * rs->bmp_bpp) + 31) / 32) * 4;
    if (data_size != (rs->bmp_row_bytes * img->h))
        File_Module_Error(fp,"%s size error!",__FUNCTION__);
    return (rs->bmp_h >= 0);
}

// This function reads the pixel values of an image.
void bmp_read_pixels(FIL *fp, image_t *img, int line_start, int line_end, bmp_read_settings_t *rs)
{
    if (rs->bmp_bpp == 8) {
        if ((rs->bmp_h < 0) && (rs->bmp_w >= 0) && (img->w == rs->bmp_row_bytes)) {
            File_Module_ReadData(fp, // Super Fast - Zoom, Zoom!
                      img->pixels + (line_start * img->w),
                      (line_end - line_start) * img->w);
        } else {
            for (int i = line_start; i < line_end; i++) {
                for (int j = 0; j < rs->bmp_row_bytes; j++) {
                    uint8_t pixel;
                    File_Module_ReadByte(fp, &pixel);
                    if (j < img->w) {
                        if (rs->bmp_h < 0) { // vertical flip (BMP file perspective)
                            if (rs->bmp_w < 0) { // horizontal flip (BMP file perspective)
                                IM_SET_GS_PIXEL(img, (img->w-j-1), i, pixel);
                            } else {
                                IM_SET_GS_PIXEL(img, j, i, pixel);
                            }
                        } else {
                            if (rs->bmp_w < 0) {
                                IM_SET_GS_PIXEL(img, (img->w-j-1), (img->h-i-1), pixel);
                            } else {
                                IM_SET_GS_PIXEL(img, j, (img->h-i-1), pixel);
                            }
                        }
                    }
                }
            }
        }
    } else if (rs->bmp_bpp == 16) {
        for (int i = line_start; i < line_end; i++) {
            for (int j = 0, jj = rs->bmp_row_bytes / 2; j < jj; j++) {
                uint16_t pixel;
                File_Module_ReadWord(fp, &pixel);
                pixel = IM_SWAP16(pixel);
                if (j < img->w) {
                    if (rs->bmp_h < 0) { // vertical flip (BMP file perspective)
                        if (rs->bmp_w < 0) { // horizontal flip (BMP file perspective)
                            IM_SET_RGB565_PIXEL(img, (img->w-j-1), i, pixel);
                        } else {
                            IM_SET_RGB565_PIXEL(img, j, i, pixel);
                        }
                    } else {
                        if (rs->bmp_w < 0) {
                            IM_SET_RGB565_PIXEL(img, (img->w-j-1), (img->h-i-1), pixel);
                        } else {
                            IM_SET_RGB565_PIXEL(img, j, (img->h-i-1), pixel);
                        }
                    }
                }
            }
        }
    } else if (rs->bmp_bpp == 24) {
        for (int i = line_start; i < line_end; i++) {
            for (int j = 0, jj = rs->bmp_row_bytes / 3; j < jj; j++) {
                uint8_t r, g, b;
                File_Module_ReadByte(fp, &b);
                File_Module_ReadByte(fp, &g);
                File_Module_ReadByte(fp, &r);
                uint16_t pixel = IM_RGB565(IM_R825(r), IM_G826(g), IM_B825(b));
                if (j < img->w) {
                    if (rs->bmp_h < 0) { // vertical flip
                        if (rs->bmp_w < 0) { // horizontal flip
                            IM_SET_RGB565_PIXEL(img, (img->w-j-1), i, pixel);
                        } else {
                            IM_SET_RGB565_PIXEL(img, j, i, pixel);
                        }
                    } else {
                        if (rs->bmp_w < 0) {
                            IM_SET_RGB565_PIXEL(img, (img->w-j-1), (img->h-i-1), pixel);
                        } else {
                            IM_SET_RGB565_PIXEL(img, j, (img->h-i-1), pixel);
                        }
                    }
                }
            }
            for (int j = 0, jj = rs->bmp_row_bytes % 3; j < jj; j++) {
                File_Module_IgnoreByte(fp);
            }
        }
    }
}

void bmp_read(image_t *img, const char *path)
{
    FIL fp;
    bmp_read_settings_t rs;
    File_Module_OpenRead(&fp, path);
    File_Module_RWBufOn(&fp);
    bmp_read_geometry(&fp, img, path, &rs);
    if (!img->pixels)
        img->pixels = RTE_MEM_Alloc0(MEM_AXIM,img->w * img->h * img->bpp);
    bmp_read_pixels(&fp, img, 0, img->h, &rs);
    File_Module_RWBufOff(&fp);
    File_Module_Close(&fp);
}
