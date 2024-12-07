/*******************************************************************************
 * Size: 20 px
 * Bpp: 1
 * Opts: --bpp 1 --size 20 --no-compress --font arialbd.ttf --symbols 0123456789 --range 48-57 --format lvgl -o arail_bold_40.c
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef ARAIL_BOLD_40
#define ARAIL_BOLD_40 1
#endif

#if ARAIL_BOLD_40

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0030 "0" */
    0x3c, 0x3f, 0x9d, 0xdc, 0x7e, 0x3f, 0x1f, 0x8f,
    0xc7, 0xe3, 0xf1, 0xf8, 0xfc, 0x77, 0x73, 0xf8,
    0xf0,

    /* U+0031 "1" */
    0xe, 0x1c, 0x7b, 0xff, 0xf9, 0xc3, 0x87, 0xe,
    0x1c, 0x38, 0x70, 0xe1, 0xc3, 0x80,

    /* U+0032 "2" */
    0x1f, 0x1f, 0xe7, 0x3f, 0x87, 0x21, 0xc0, 0x70,
    0x3c, 0x1e, 0xf, 0x7, 0x83, 0xc1, 0xe0, 0x70,
    0x3f, 0xff, 0xfc,

    /* U+0033 "3" */
    0x3e, 0x3f, 0xb8, 0xfc, 0x70, 0x38, 0x38, 0x38,
    0x1c, 0x7, 0x1, 0xc0, 0xfc, 0x7e, 0x7b, 0xf8,
    0xf8,

    /* U+0034 "4" */
    0x3, 0x81, 0xe0, 0x78, 0x3e, 0xf, 0x86, 0xe3,
    0xb8, 0xce, 0x73, 0xb8, 0xef, 0xff, 0xff, 0x3,
    0x80, 0xe0, 0x38,

    /* U+0035 "5" */
    0x3f, 0x8f, 0xe7, 0x1, 0xc0, 0x70, 0x1f, 0xc7,
    0xf9, 0x8e, 0x1, 0xc0, 0x70, 0x1f, 0x87, 0x73,
    0x9f, 0xe1, 0xe0,

    /* U+0036 "6" */
    0x1f, 0xf, 0xe7, 0x39, 0x86, 0xe0, 0x3b, 0xcf,
    0xfb, 0xce, 0xe1, 0xf8, 0x7e, 0x1f, 0x87, 0x73,
    0x8f, 0xe1, 0xe0,

    /* U+0037 "7" */
    0xff, 0xff, 0xc0, 0xc0, 0xe0, 0x60, 0x70, 0x38,
    0x38, 0x1c, 0xe, 0x6, 0x7, 0x3, 0x81, 0xc0,
    0xe0,

    /* U+0038 "8" */
    0x3e, 0x1f, 0xce, 0x3b, 0x8e, 0xe3, 0xb8, 0xe7,
    0xf1, 0xfc, 0xf3, 0xb8, 0x7e, 0x1f, 0x87, 0xf3,
    0xdf, 0xe3, 0xf0,

    /* U+0039 "9" */
    0x1e, 0x1f, 0xc7, 0x3b, 0x86, 0xe1, 0xf8, 0x7e,
    0x1f, 0xcf, 0x7f, 0xcf, 0x70, 0x1d, 0x86, 0x73,
    0x9f, 0xc3, 0xe0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 178, .box_w = 9, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 17, .adv_w = 178, .box_w = 7, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 31, .adv_w = 178, .box_w = 10, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 50, .adv_w = 178, .box_w = 9, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 67, .adv_w = 178, .box_w = 10, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 86, .adv_w = 178, .box_w = 10, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 105, .adv_w = 178, .box_w = 10, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 124, .adv_w = 178, .box_w = 9, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 141, .adv_w = 178, .box_w = 10, .box_h = 15, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 160, .adv_w = 178, .box_w = 10, .box_h = 15, .ofs_x = 1, .ofs_y = 0}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 48, .range_length = 10, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t arail_bold_40 = {
#else
lv_font_t arail_bold_40 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 15,          /*The maximum line height required by the font*/
    .base_line = 0,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 2,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if ARAIL_BOLD_40*/

