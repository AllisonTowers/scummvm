/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef AGS_LIB_ALLEGRO_GRAPHICS_H
#define AGS_LIB_ALLEGRO_GRAPHICS_H

#include "graphics/managed_surface.h"
#include "ags/lib/allegro/base.h"
#include "ags/lib/allegro/fixed.h"
#include "common/array.h"

namespace AGS3 {

#define GFX_TEXT                       -1
#define GFX_AUTODETECT                 0
#define GFX_AUTODETECT_FULLSCREEN      1
#define GFX_AUTODETECT_WINDOWED        2
#define GFX_SAFE                       AL_ID('S','A','F','E')
#define GFX_NONE                       AL_ID('N','O','N','E')

/* Bitfield for relaying graphics driver type information */
#define GFX_TYPE_UNKNOWN     0
#define GFX_TYPE_WINDOWED    1
#define GFX_TYPE_FULLSCREEN  2
#define GFX_TYPE_DEFINITE    4
#define GFX_TYPE_MAGIC       8

/* drawing modes for draw_sprite_ex() */
#define DRAW_SPRITE_NORMAL 0
#define DRAW_SPRITE_LIT 1
#define DRAW_SPRITE_TRANS 2

/* flipping modes for draw_sprite_ex() */
#define DRAW_SPRITE_NO_FLIP 0x0
#define DRAW_SPRITE_H_FLIP  0x1
#define DRAW_SPRITE_V_FLIP  0x2
#define DRAW_SPRITE_VH_FLIP 0x3

/* Blender mode defines, for the gfx_driver->set_blender_mode() function */
#define blender_mode_none            0
#define blender_mode_trans           1
#define blender_mode_add             2
#define blender_mode_burn            3
#define blender_mode_color           4
#define blender_mode_difference      5
#define blender_mode_dissolve        6
#define blender_mode_dodge           7
#define blender_mode_hue             8
#define blender_mode_invert          9
#define blender_mode_luminance      10
#define blender_mode_multiply       11
#define blender_mode_saturation     12
#define blender_mode_screen         13
#define blender_mode_alpha          14


#define SCREEN_W     (gfx_driver ? gfx_driver->w : 0)
#define SCREEN_H     (gfx_driver ? gfx_driver->h : 0)

#define VIRTUAL_W    (screen ? screen->w : 0)
#define VIRTUAL_H    (screen ? screen->h : 0)

#define COLORCONV_NONE              0

#define COLORCONV_8_TO_15           1
#define COLORCONV_8_TO_16           2
#define COLORCONV_8_TO_24           4
#define COLORCONV_8_TO_32           8

#define COLORCONV_15_TO_8           0x10
#define COLORCONV_15_TO_16          0x20
#define COLORCONV_15_TO_24          0x40
#define COLORCONV_15_TO_32          0x80

#define COLORCONV_16_TO_8           0x100
#define COLORCONV_16_TO_15          0x200
#define COLORCONV_16_TO_24          0x400
#define COLORCONV_16_TO_32          0x800

#define COLORCONV_24_TO_8           0x1000
#define COLORCONV_24_TO_15          0x2000
#define COLORCONV_24_TO_16          0x4000
#define COLORCONV_24_TO_32          0x8000

#define COLORCONV_32_TO_8           0x10000
#define COLORCONV_32_TO_15          0x20000
#define COLORCONV_32_TO_16          0x40000
#define COLORCONV_32_TO_24          0x80000

#define COLORCONV_32A_TO_8          0x100000
#define COLORCONV_32A_TO_15         0x200000
#define COLORCONV_32A_TO_16         0x400000
#define COLORCONV_32A_TO_24         0x800000

#define COLORCONV_DITHER_PAL        0x1000000
#define COLORCONV_DITHER_HI         0x2000000
#define COLORCONV_KEEP_TRANS        0x4000000

#define COLORCONV_DITHER            (COLORCONV_DITHER_PAL |          \
                                     COLORCONV_DITHER_HI)

#define COLORCONV_EXPAND_256        (COLORCONV_8_TO_15 |             \
                                     COLORCONV_8_TO_16 |             \
                                     COLORCONV_8_TO_24 |             \
                                     COLORCONV_8_TO_32)

#define COLORCONV_REDUCE_TO_256     (COLORCONV_15_TO_8 |             \
                                     COLORCONV_16_TO_8 |             \
                                     COLORCONV_24_TO_8 |             \
                                     COLORCONV_32_TO_8 |             \
                                     COLORCONV_32A_TO_8)

#define COLORCONV_EXPAND_15_TO_16    COLORCONV_15_TO_16

#define COLORCONV_REDUCE_16_TO_15    COLORCONV_16_TO_15

#define COLORCONV_EXPAND_HI_TO_TRUE (COLORCONV_15_TO_24 |            \
                                     COLORCONV_15_TO_32 |            \
                                     COLORCONV_16_TO_24 |            \
                                     COLORCONV_16_TO_32)

#define COLORCONV_REDUCE_TRUE_TO_HI (COLORCONV_24_TO_15 |            \
                                     COLORCONV_24_TO_16 |            \
                                     COLORCONV_32_TO_15 |            \
                                     COLORCONV_32_TO_16)

#define COLORCONV_24_EQUALS_32      (COLORCONV_24_TO_32 |            \
                                     COLORCONV_32_TO_24)

#define COLORCONV_TOTAL             (COLORCONV_EXPAND_256 |          \
                                     COLORCONV_REDUCE_TO_256 |       \
                                     COLORCONV_EXPAND_15_TO_16 |     \
                                     COLORCONV_REDUCE_16_TO_15 |     \
                                     COLORCONV_EXPAND_HI_TO_TRUE |   \
                                     COLORCONV_REDUCE_TRUE_TO_HI |   \
                                     COLORCONV_24_EQUALS_32 |        \
                                     COLORCONV_32A_TO_15 |           \
                                     COLORCONV_32A_TO_16 |           \
                                     COLORCONV_32A_TO_24)

#define COLORCONV_PARTIAL           (COLORCONV_EXPAND_15_TO_16 |     \
                                     COLORCONV_REDUCE_16_TO_15 |     \
                                     COLORCONV_24_EQUALS_32)

#define COLORCONV_MOST              (COLORCONV_EXPAND_15_TO_16 |     \
                                     COLORCONV_REDUCE_16_TO_15 |     \
                                     COLORCONV_EXPAND_HI_TO_TRUE |   \
                                     COLORCONV_REDUCE_TRUE_TO_HI |   \
                                     COLORCONV_24_EQUALS_32)

#define COLORCONV_KEEP_ALPHA        (COLORCONV_TOTAL                 \
                                     & ~(COLORCONV_32A_TO_8 |        \
                                             COLORCONV_32A_TO_15 |       \
                                             COLORCONV_32A_TO_16 |       \
                                             COLORCONV_32A_TO_24))

class BITMAP {
private:
	Graphics::ManagedSurface *_owner;
public:
	uint16 &w, &h;
	Graphics::PixelFormat &format;
	bool clip;
	int ct, cb, cl, cr;
	Common::Array<byte *> line;
public:
	BITMAP(Graphics::ManagedSurface *owner);
	virtual ~BITMAP() {
	}

	Graphics::ManagedSurface &operator*() const {
		return *_owner;
	}
	Graphics::ManagedSurface &getSurface() {
		return *_owner;
	}
	const Graphics::ManagedSurface &getSurface() const {
		return *_owner;
	}

	unsigned char *getPixels() const {
		return (unsigned char *)_owner->getPixels();
	}

	unsigned char *getBasePtr(uint16 x, uint16 y) const {
		return (unsigned char *)_owner->getBasePtr(x, y);
	}

	uint getTransparentColor() const {
		return format.RGBToColor(255, 0, 255);
	}

	int getpixel(int x, int y) const;

	void clear() {
		_owner->clear();
	}

	/**
	 * Draws a solid filled in circle
	 */
	void circlefill(int x, int y, int radius, int color);

	/**
	 * Fills an enclosed area starting at a given point
	 */
	void floodfill(int x, int y, int color);

	/**
	 * Draw a horizontal line
	 */
	void hLine(int x, int y, int x2, uint32 color) {
		_owner->hLine(x, y, x2, color);
	}

	/**
	 * Draw a vertical line.
	 */
	void vLine(int x, int y, int y2, uint32 color) {
		_owner->vLine(x, y, y2, color);
	}
};

/**
 * Derived surface class
 */
class Surface : public Graphics::ManagedSurface, public BITMAP {
public:
	Surface() : Graphics::ManagedSurface(), BITMAP(this) {
	}
	Surface(const Graphics::ManagedSurface &surf) : Graphics::ManagedSurface(surf), BITMAP(this) {
	}
	Surface(int width, int height) : Graphics::ManagedSurface(width, height), BITMAP(this) {
	}
	Surface(int width, int height, const Graphics::PixelFormat &pixelFormat) :
			Graphics::ManagedSurface(width, height, pixelFormat), BITMAP(this) {
		// Allegro uses 255, 0, 255 RGB as the transparent color
		if (pixelFormat.bytesPerPixel == 4)
			setTransparentColor(pixelFormat.RGBToColor(255, 0, 255));
	}
	Surface(Graphics::ManagedSurface &surf, const Common::Rect &bounds) :
			Graphics::ManagedSurface(surf, bounds), BITMAP(this) {
		// Allegro uses 255, 0, 255 RGB as the transparent color
		if (surf.format.bytesPerPixel == 4)
			setTransparentColor(surf.format.RGBToColor(255, 0, 255));
	}
	~Surface() override {
	}
};


AL_FUNC(void, set_color_conversion, (int mode));
AL_FUNC(int, get_color_conversion, ());
AL_FUNC(int, set_gfx_mode, (int card, int w, int h, int v_w, int v_h));
AL_FUNC(BITMAP *, create_bitmap, (int width, int height));
AL_FUNC(BITMAP *, create_bitmap_ex, (int color_depth, int width, int height));
AL_FUNC(BITMAP *, create_sub_bitmap, (BITMAP *parent, int x, int y, int width, int height));
AL_FUNC(BITMAP *, create_video_bitmap, (int width, int height));
AL_FUNC(BITMAP *, create_system_bitmap, (int width, int height));

AL_FUNC(void, destroy_bitmap, (BITMAP *bitmap));
AL_FUNC(void, set_clip_rect, (BITMAP *bitmap, int x1, int y1, int x2, int y2));
AL_FUNC(void, add_clip_rect, (BITMAP *bitmap, int x1, int y1, int x2, int y2));
AL_FUNC(void, get_clip_rect, (BITMAP *bitmap, int *x1, int *y1, int *x2, int *y2));
AL_FUNC(void, clear_bitmap, (BITMAP *bitmap));

AL_FUNC(void, acquire_bitmap, (BITMAP *bitmap));
AL_FUNC(void, release_bitmap, (BITMAP *bitmap));
AL_FUNC(void, draw_sprite, (BITMAP *bmp, const BITMAP *sprite, int x, int y));
AL_FUNC(void, stretch_sprite, (BITMAP *bmp, const BITMAP *sprite, int x, int y, int w, int h));

extern void clear_to_color(BITMAP *bitmap, int color);
extern int bitmap_color_depth(BITMAP *bmp);
extern int bitmap_mask_color(BITMAP *bmp);
extern void add_palette_if_needed(Graphics::ManagedSurface &surf);
extern void blit(const BITMAP *src, BITMAP *dest, int src_x, int src_y, int dst_x, int dst_y, int width, int height);
extern void masked_blit(const BITMAP *src, BITMAP *dest, int src_x, int src_y, int dst_x, int dst_y, int width, int height);
extern void stretch_blit(const BITMAP *src, BITMAP *dest, int source_x, int source_y, int source_width, int source_height,
	int dest_x, int dest_y, int dest_width, int dest_height);
extern void masked_stretch_blit(const BITMAP *src, BITMAP *dest, int source_x, int source_y, int source_width, int source_height,
	int dest_x, int dest_y, int dest_width, int dest_height);
extern void draw_trans_sprite(BITMAP *bmp, const BITMAP *sprite, int x, int y);
extern void draw_lit_sprite(BITMAP *bmp, const BITMAP *sprite, int x, int y, int color);
extern void draw_sprite_h_flip(BITMAP *bmp, const BITMAP *sprite, int x, int y);
extern void draw_sprite_v_flip(BITMAP *bmp, const BITMAP *sprite, int x, int y);
extern void draw_sprite_vh_flip(BITMAP *bmp, const BITMAP *sprite, int x, int y);
extern void rotate_sprite(BITMAP *bmp, const BITMAP *sprite, int x, int y, fixed angle);
extern void pivot_sprite(BITMAP *bmp, const BITMAP *sprite, int x, int y, int cx, int cy, fixed angle);

extern bool is_screen_bitmap(BITMAP *bmp);
extern bool is_video_bitmap(BITMAP *bmp);
extern bool is_linear_bitmap(BITMAP *bmp);
extern bool is_planar_bitmap(BITMAP *bmp);
extern void bmp_select(BITMAP *bmp);
extern byte *bmp_write_line(BITMAP *bmp, int line);
extern void bmp_unwrite_line(BITMAP *bmp);
extern void bmp_write8(byte *addr, int color);
extern void memory_putpixel(BITMAP *bmp, int x, int y, int color);
extern void putpixel(BITMAP *bmp, int x, int y, int color);
extern void _putpixel(BITMAP *bmp, int x, int y, int color);
extern void _putpixel15(BITMAP *bmp, int x, int y, int color);
extern void _putpixel16(BITMAP *bmp, int x, int y, int color);
extern void _putpixel24(BITMAP *bmp, int x, int y, int color);
extern void _putpixel32(BITMAP *bmp, int x, int y, int color);
extern int getpixel(BITMAP *bmp, int x, int y);
extern int _getpixel(BITMAP *bmp, int x, int y);
extern int _getpixel15(BITMAP *bmp, int x, int y);
extern int _getpixel16(BITMAP *bmp, int x, int y);
extern int _getpixel24(BITMAP *bmp, int x, int y);
extern int _getpixel32(BITMAP *bmp, int x, int y);
extern void line(BITMAP *bmp, int x1, int y_1, int x2, int y2, int color);
extern void rect(BITMAP *bmp, int x1, int y_1, int x2, int y2, int color);
extern void rectfill(BITMAP *bmp, int x1, int y_1, int x2, int y2, int color);
extern void triangle(BITMAP *bmp, int x1, int y_1, int x2, int y2, int x3, int y3, int color);
extern void circlefill(BITMAP *bmp, int x, int y, int radius, int color);

} // namespace AGS3

#endif
