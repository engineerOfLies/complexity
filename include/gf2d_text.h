#ifndef __MGL_TEXT__H__
#define __MGL_TEXT__H__
/**
 * gf2d_text
 * @license The MIT License (MIT)
   @copyright Copyright (c) 2015 EngineerOfLies
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

/**
 * @purpose the MGL text library provides common string support for fixed length
 *  strings.  For use with unlimited strings mgl will use glib's GString type
 */
#include <string.h>
#include "gf2d_types.h"
#include "gf2d_color.h"
/**
 * constant lenth character buffers.
 * These will be used when the convenience of GString is not needed
 */

/**
 * @brief 16 character buffer used for short tags
 */
typedef char TextWord[16];
#define GF2DWORDLEN   16
#define gf2d_word_cmp(a,b) (strncmp(a,b,GF2DWORDLEN))
#define gf2d_word_cpy(dst,src) (strncpy(dst,src,GF2DWORDLEN))
#define gf2d_word_clear(a)  (memset(a,0,sizeof(char)*GF2DWORDLEN))
/**
 * @brief 128 character buffer used for statements
 */
typedef char TextLine[128];
#define GF2DLINELEN   128
#define gf2d_line_cmp(a,b) (strncmp(a,b,GF2DLINELEN))
#define gf2d_line_cpy(dst,src) (strncpy(dst,src,GF2DLINELEN))
#define gf2d_line_clear(a)  (memset(a,0,sizeof(char)*GF2DLINELEN))
/**
 * @brief 512 character buffer used for text blocks
 */
typedef char TextBlock[512];
#define GF2DTEXTLEN   512
#define gf2d_block_cmp(a,b) (strncmp(a,b,GF2DTEXTLEN))
#define gf2d_block_cpy(dst,src) (strncpy(dst,src,GF2DTEXTLEN))
#define gf2d_block_clear(a)  (memset(a,0,sizeof(char)*GF2DTEXTLEN))

typedef enum
{
    FT_Normal,
    FT_Small,
    FT_H1,
    FT_H2,
    FT_H3,
    FT_H4,
    FT_H5,
    FT_H6,
    FT_MAX
}FontTypes;

/**
 * @brief initialized text drawing system
 * @param configFile the file to load font information from
 */
void gf2d_text_init(char *configFile);

void gf2d_text_draw_line_font(char *text,char *filename,Color color, Vector2D position);

void gf2d_text_draw_line(char *text,FontTypes tag,Color color, Vector2D position);

Vector2D gf2d_text_get_bounds(char *text,FontTypes tag);

#endif
