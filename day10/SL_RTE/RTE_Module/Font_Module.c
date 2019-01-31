#include "GUI_Font/lv_font.h"
#include "Font_Module.h"
#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_MODULE_H
#include FT_CACHE_H
#include FT_CACHE_MANAGER_H
#include "RTE_Include.h"
static osMutexId_t MutexIDFont;
static const uint8_t * lv_font_get_bitmap_freetype2(lv_font_t * font, uint32_t unicode_letter);
static int16_t lv_font_get_width_freetype2(lv_font_t * font, uint32_t unicode_letter);
lv_font_t lv_font_freetype2 = {
    .unicode_first = 0,    /*First Unicode letter in this font*/
    .unicode_last = 0,    /*Last Unicode letter in this font*/
    .h_px = 0,             /*Font height in pixels*/
    .glyph_bitmap = NULL, /*Bitmap of glyphs*/
    .glyph_dsc = NULL,       /*Description of glyphs*/
    .unicode_list = NULL,   /*Every character in the font from 'unicode_first' to 'unicode_last'*/
    .get_bitmap = lv_font_get_bitmap_freetype2,    /*Function pointer to get glyph's bitmap*/
    .get_width = lv_font_get_width_freetype2,  /*Function pointer to get glyph's width*/
    .bpp = 8,               /*Bit per pixel*/
    .next_page = NULL,      /*Pointer to a font extension*/
    .freetype = true,
};

/**
 * Freetype2 set fonts size
 */
uint8_t lv_font_setsize_freetype2(lv_font_t * font,uint8_t size)
{
    lv_font_freetype2.fontsize = size;
    FT_Set_Pixel_Sizes( font->face, size, 0 );
	//printf("[debug] size = %d\n", size);
	return size;
}
/**
 * Initialize the built-in fonts
 */
void lv_font_init_freetype2(lv_font_t * font,char *fontname,uint8_t size)
{
	int error;
	if( fontname == NULL )
		return ;
	printf("%s < %s\n", __func__, fontname);
	error = FT_Init_FreeType( &font->library ); /* initialize library */
	if(error)
    {
	    printf("FT_Init_FreeType fail\r\n");
	    return;
    }
	error = FT_New_Face( font->library, fontname, 0, &font->face );
	if(error)
    {
	    printf("FT_New_Face fail\r\n");
	    return;
    }
	lv_font_setsize_freetype2(font,size);
	MutexIDFont = osMutexNew(NULL);
}
/**
 * Generic bitmap get function used in 'font->get_bitmap' when the font contains all characters in the range
 * @param font pointer to font
 * @param unicode_letter an unicode letter which bitmap should be get
 * @return pointer to the bitmap or NULL if not found
 */
static uint32_t last_letter = 0;
static const uint8_t * lv_font_get_bitmap_freetype2(lv_font_t * font, uint32_t unicode_letter)
{
    osMutexAcquire(MutexIDFont,0);
    RTE_Printf("1\r\n");
//	int error;
//	if( last_letter == unicode_letter ) {
//        osMutexRelease(MutexIDFont);
//        //printf("%x [ %c%c%c ] < top %d left %d row %d col %d >\n", unicode_letter, unicode_letter&0xff, unicode_letter>>8, unicode_letter>>16, slot->bitmap_top, slot->bitmap_left, slot->bitmap.rows, slot->bitmap.width);
//		return font->face->glyph->bitmap.buffer;
//	}
//	last_letter = unicode_letter;
//	error = FT_Load_Char( font->face, unicode_letter, FT_LOAD_MONOCHROME );
//	if (error)
//	{
//	    osMutexRelease(MutexIDFont);
//		printf("FT_Load_Char error\n");
//		return NULL;
//	}
//	font->offset = (font->fontsize - ((font->face->glyph->metrics.height + 63)>>6));
//	osMutexRelease(MutexIDFont);
    return font->face->glyph->bitmap.buffer;
}

/**
 * Generic glyph width get function used in 'font->get_width' when the font contains all characters in the range
 * @param font pointer to font
 * @param unicode_letter an unicode letter which width should be get
 * @return width of the gylph or -1 if not found
 */
static int16_t lv_font_get_width_freetype2(lv_font_t * font, uint32_t unicode_letter)
{
    osMutexAcquire(MutexIDFont,0);
    RTE_Printf("2\r\n");
	int error;
	if( last_letter == unicode_letter ) {
        osMutexRelease(MutexIDFont);
		if( font->face->glyph->bitmap.width == 0 ) {
			if( unicode_letter == 0x09 )
				return font->fontsize*2;
			else
				return font->fontsize/2;
		}
		font->offset = (font->fontsize - ((font->face->glyph->metrics.height + 63)>>6));
		return font->face->glyph->bitmap.width;
	}
	last_letter = unicode_letter;
	error = FT_Load_Char( font->face, unicode_letter, FT_LOAD_RENDER );
	if (error)
	{
		printf("FT_Load_Char error\n");
		return 0;
	}
	osMutexRelease(MutexIDFont);
	// for get rows
	font->h_px = font->fontsize; // slot->bitmap.rows;
	font->bpp = 8;
	font->offset = (font->fontsize - ((font->face->glyph->metrics.height + 63)>>6));
	if( font->face->glyph->bitmap.width == 0 ) {
		if( unicode_letter == 0x09 )
			return font->fontsize*2;
		else
			return font->fontsize/2;
	}
    return font->face->glyph->bitmap.width;
}
