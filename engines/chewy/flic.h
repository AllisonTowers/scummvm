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

#ifndef CHEWY_FLIC_H
#define CHEWY_FLIC_H 1

#include "chewy/ngshext.h"
#include "chewy/ngstypes.h"

namespace Chewy {

#define taste error("aborted")

#define FLI 0xAF11
#define FLC 0xAF12
#define BEENDEN 1
#define LOOPEN 2
#define PREFIX 0xF100
#define FRAME 0xF1FA
enum ChunkTypes {
	COLOR_256 = 4,
	DELTA_FLC = 7,
	COLOR_64 = 11,
	DELTA_FLI = 12,
	CLS = 13,
	BYTE_RUN = 15,
	UNPRESSED = 16,
	PSTAMP = 18
};

#define CUSTOM 0xFAF1
enum CustomTypes {
	FADE_IN = 0,
	FADE_OUT = 1,
	LOAD_MUSIC = 2,
	LOAD_RAW = 3,
	LOAD_VOC = 4,
	PLAY_MUSIC = 5,
	PLAY_SEQ = 6,
	PLAY_PATTERN = 7,
	STOP_MUSIC = 8,
	WAIT_MSTOP = 9,
	SET_MVOL = 10,
	SET_LOOPMODE = 11,
	PLAY_RAW = 12,
	PLAY_VOC = 13,
	SET_SVOL = 14,
	SET_CVOL = 15,
	FREE_EFFECT = 16,
	MFADE_IN = 17,
	MFADE_OUT = 18,
	SET_STEREO = 19,
	SET_SPEED = 20,
	CLEAR_SCREEN = 21
};

void decode_flc(byte *vscr, byte *dbuf);
void decode_rle(byte *vscr, byte *dbuf, int br, int h);

static int16(*custom_user)(int16) = 0;
static int16(*flic_user)(int16) = 0;

class flic {
public:
	flic();
	~flic();
	void play(const char *fname, byte *vscreen, byte *load_p);

	void play(Common::Stream *handle, byte *vscreen, byte *load_p);
	void set_flic_user_function(int16(*user_funktion)(int16 key));
	void remove_flic_user_function();
	void custom_play(CustomInfo *ci);
	void set_custom_user_function(int16(*user_funktion)(int16 key));
	void remove_custom_user_function();

private:
	void decode_frame();
	void decode_cframe();
	void col256_chunk(byte *tmp);
	void col64_chunk(byte *tmp);
	void delta_chunk_byte(byte *tmp);

	void decode_custom_frame(Common::SeekableReadStream *handle);
	void free_sound(int16 nr);

	byte *load_puffer;
	byte *virt_screen;
	byte *Music;
	byte *Sound;
	CustomInfo *Cinfo;
	FlicHead flic_header;
	CustomFlicHead custom_header;
	CustomFrameHead custom_frame;
	FrameHead frame_header;
	ChunkHead *chunk_header;
	bool fade_flag;
	bool cls_flag;
	byte fade_pal[768];
	int16 fade_delay;
	byte *sounds[50];
	uint32 Ssize[50];
	int16 CurrentFrame;
};

} // namespace Chewy

#endif
