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

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "access/martian/martian_resources.h"
#include "access/access.h"

namespace Access {

namespace Martian {

const char *const FILENAMES[] = {
	"R00.AP", "R01.AP", "R02.AP", "R03.AP", "R04.AP", "R05.AP", "R06.AP", "R07.AP",
	"R08.AP", "R09.AP", "R10.AP", "R11.AP", "R12.AP", "R13.AP", "R14.AP", "R15.AP",
	"R16.AP", "R17.AP", "R18.AP", "R19.AP", "R20.AP", "R21.AP", "R22.AP", "R23.AP",
	"R24.AP", "R25.AP", "R26.AP", "R27.AP", "R28.AP", "R29.AP", "R30.AP", "R31.AP",
	"R32.AP", "R33.AP", "R34.AP", "R35.AP", "R36.AP", "R37.AP", "R38.AP", "R39.AP",
	"R40.AP","TITLE.AP","R42.AP","S01.AP",  "R44.AP", "R45.AP","SOUND.AP","MUSIC.AP",
	"DEAD.AP","EST.AP", "W02.AP", "C02.AP", "C05.AP", "C04.AP", "C10.AP", "C03.AP",
	"C07.AP", "LOVE.AP","CAFE.AP","C08.AP", "C18.AP", "C19.AP", "C21.AP", "C23.AP",
	"C12.AP", "C16.AP","CAFE1.AP","C05A.AP","C06.AP","C11.AP", "C13.AP", "C20.AP",
	"C16A.AP","C09.AP", "R45.AP", "R46.AP", "R47.AP", "R48.AP", "R49.AP"
};

const byte MOUSE0[] = {
	0, 0, 0, 0, 0, 2, 0xF7, 5, 0, 3, 0xF7, 0xF7, 5, 0, 3,
	0xF7, 0xF7, 5, 0, 4, 0xF7, 0xF7, 0xF7, 5, 0, 4, 0xF7,
	0xF7, 0xF7, 5, 0, 5, 0xF7, 0xF7, 0xF7, 0xF7, 5, 0, 5,
	0xF7, 0xF7, 0xF7, 0xF7, 5, 0, 6, 0xF7, 0xF7, 0xF7, 0xF7,
	0xF7, 5, 0, 6, 0xF7, 0xF7, 0xF7, 0xF7, 0xF7, 5, 0, 7,
	0xF7, 0xF7, 0xF7, 0xF7, 0xF7, 0xF7, 5, 0, 6, 0xF7, 0xF7,
	0xF7, 0xF7, 0xF7, 5, 0, 5, 0xF7, 0xF7, 0xF7, 0xF7, 5,
	2, 3, 0xF7, 0xF7, 5, 3, 3, 0xF7, 0xF7, 5, 3, 3, 0xF7,
	0xF7, 5, 4, 2, 0xF7, 5
};
const byte MOUSE1[] = {
	7, 0, 7, 0, 6, 1, 0xF7, 4, 5, 0xFF, 0xFF, 0, 0xFF, 0xFF,
	3, 7, 0xFF, 0, 0, 0, 0, 0, 0xFF, 2, 9, 0xFF, 0, 0, 0,
	0xF7, 0, 0, 0, 0xFF, 1, 11, 0xFF, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0xFF, 1, 11, 0xFF, 0, 0, 0, 0, 0xF7, 0, 0,
	0, 0, 0xFF, 0, 13, 0xF7, 0, 0, 0xF7, 0, 0xF7, 0, 0xF7,
	0, 0xF7, 0, 0, 0xF7, 1, 11, 0xFF, 0, 0, 0, 0, 0xF7,
	0, 0, 0, 0, 0xFF, 1, 11, 0xFF, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0xFF, 2, 9, 0xFF, 0, 0, 0, 0xF7, 0, 0, 0, 0xFF,
	3, 7, 0xFF, 0, 0, 0, 0, 0, 0xFF, 4, 5, 0xFF, 0xFF, 0,
	0xFF, 0xFF, 6, 1, 0xF7, 0, 0, 0, 0, 0, 0
};
const byte MOUSE2[] = {
	8, 0, 8, 0, 0, 0, 0, 0, 7, 2, 4, 5, 7, 2, 4, 5, 7, 2,
	4, 5, 7, 2, 4, 5, 7, 2, 4, 5, 2, 12, 4, 4, 4, 4, 4,
	0, 4, 4, 4, 4, 4, 5, 7, 2, 4, 5, 7, 2, 4, 5, 7, 2, 4,
	5, 7, 2, 4, 5, 7, 2, 4, 5, 0, 0, 0, 0, 0, 0
};
const byte MOUSE3[] = {
	0, 0, 0, 0, 0, 11, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	0, 12, 6, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 5, 0, 12,
	6, 7, 7, 7, 7, 7, 7, 7, 7, 6, 5, 5, 0, 12, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6, 5, 0, 12, 6, 6, 6, 6, 6, 5,
	6, 6, 6, 6, 6, 5, 0, 12, 6, 6, 6, 6, 5, 0, 0, 6, 6,
	6, 6, 5, 0, 12, 6, 6, 6, 6, 6, 0, 6, 6, 6, 6, 6, 5,
	0, 12, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5, 0, 12,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5, 0, 12, 6, 6, 6,
	6, 6, 5, 6, 6, 6, 6, 6, 5, 0, 12, 6, 6, 6, 6, 6, 5,
	6, 6, 6, 6, 6, 5, 0, 12, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 5, 1, 11, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0,
	0, 0, 0, 0, 0
};
const byte *CURSORS[4] = { MOUSE0, MOUSE1, MOUSE2, MOUSE3 };

const int TRAVEL_POS[][2] = {
	{ -1, 0 },
	{ 228, 117 },
	{ 28, 98 },
	{ 161, 140 },
	{ 160, 116 },
	{ 34, 119 },
	{ 166, 105 },
	{ 260, 126 },
	{ 37, 107 },
	{ 78, 139 },
	{ 0, 0 },
	{ 13, 112 },
	{ 0, 0 },
	{ 16, 122 },
	{ 33, 126 },
	{ 10, 160 },
	{ 150, 102 },
	{ 134, 160 },
	{ 160, 76 },
	{ 0, 0 },
	{ 0, 0 },
	{ 36, 116 },
	{ 214, 113 },
	{ 30, 127 },
	{ 143, 131 },
	{ 163, 103 },
	{ 254, 106 },
	{ 28, 161 },
	{ 11, 164 },
	{ 276, 134 },
	{ 93, 118 },
	{ 22, 150 },
	{ 282, 156 },
	{ 149, 92 },
	{ 0, 0 },
	{ 43, 410 },
	{ 0, 0 },
	{ 10, 136 },
	{ 41, 100 },
	{ 157, 97 },
	{ -1, 5 },
	{ -1, 4 },
	{ -1, 10 },
	{ -1, 7 },
	{ -1, 3 },
	{ -1, 8 },
	{ -1, 6 },
	{ -1, 20 },
	{ -1, 18 },
	{ -1, 19 },
	{ -1, 21 }
};

const char *const INVENTORY_NAMES[] = {
	"CAMERA", "LENS", "PHOTOS", "MAIL", "GUN", "CASH", "COMLINK", "AMMO",
	"LOCKPICK KIT", "EARRING", "RECIEPTS", "PAPER", "LADDER", "BOOTS",
	"DOCUMENTS", "KNIFE", "DAGGER", "KEYS", "ROCK", "LOG", "SHOVEL",
	"STONE", "REMOTE CONTROL", "FOOD AND WATER", "DOOR CARD KEY",
	"FLASHLIGHT", "INTERLOCK KEY", "TOOLS", "REBREATHER", "JET PACK",
	"ROD", "HCL2", "SAFE CARD KEY", "TUNING FORK", "STONE", "ROSE",
	"KEY", "NOTE", "ALLEN WRENCH", "HOVER BOARD", "BLUE PRINTS",
	"LETTER", "MEMORANDUM", "MARKERS", "FILM", "ANDRETTI FILM",
	"GLASSES", "AMULET", "FACIAL KIT", "CAT FOOD", "MONKEY WRENCH",
	"BIG DICK CARD", "BRA", "BOLT"
};

const byte ROOM_TABLE1[] = {
	0x00, 0x2f, 0x00, 0x0d, 0x00, 0x30, 0x22, 0x30, 0x01, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x00, 0x03, 0x00, 0xff, 0x01, 0x00,
	0x02, 0x00, 0x01, 0x00, 0x01, 0x00, 0xc0, 0x00, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x2e, 0x00, 0x05, 0x00, 0x01, 0x00, 0xff, 0xff,
};
const byte ROOM_TABLE2[] = {
	0x00, 0x2f, 0x00, 0x0d, 0x00, 0x32, 0x28, 0x25, 0x02, 0x00,
	0x00, 0x00, 0x02, 0x02, 0x00, 0x03, 0x00, 0xff, 0x02, 0x00,
	0x02, 0x00, 0x02, 0x00, 0x01, 0x00, 0xc8, 0x00, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x2e, 0x00, 0x06, 0x00, 0x01, 0x00, 0xff, 0xff,
};
const byte ROOM_TABLE3[] = {
	0x00, 0x2f, 0x00, 0x0f, 0x00, 0x1e, 0x19, 0x24, 0x03, 0x00,
	0x00, 0x00, 0x03, 0x03, 0x00, 0x03, 0x00, 0xff, 0x03, 0x00,
	0x02, 0x00, 0x03, 0x00, 0x01, 0x00, 0x78, 0x00, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x02, 0x00,
	0x03, 0x00, 0x04, 0x00, 0x01, 0x00, 0x03, 0x00, 0x05, 0x00,
	0x01, 0x00, 0x03, 0x00, 0x06, 0x00, 0x01, 0x00, 0x2e, 0x00,
	0x01, 0x00, 0x01, 0x00, 0xff, 0xff,
};
const byte ROOM_TABLE4[] = {
	0x00, 0x2f, 0x00, 0x06, 0x00, 0x36, 0x27, 0x32, 0x04, 0x00,
	0x00, 0x00, 0x04, 0x04, 0x00, 0x03, 0x00, 0xff, 0x04, 0x00,
	0x02, 0x00, 0x04, 0x00, 0x01, 0x00, 0xc8, 0x00, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x2e, 0x00, 0x07, 0x00, 0x01, 0x00, 0x2e, 0x00, 0x05, 0x00,
	0x01, 0x00, 0xff, 0xff,
};
const byte ROOM_TABLE5[] = {
	0x00, 0x2f, 0x00, 0x00, 0x00, 0x28, 0x19, 0x36, 0x05, 0x00,
	0x00, 0x00, 0x05, 0x05, 0x00, 0x03, 0x00, 0xff, 0x05, 0x00,
	0x02, 0x00, 0x05, 0x00, 0x01, 0x00, 0xa0, 0x20, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x2e, 0x00, 0x02, 0x00, 0x01, 0x00, 0x2e, 0x00, 0x03, 0x00,
	0x01, 0x00, 0xff, 0xff,
};
const byte ROOM_TABLE6[] = {
	0x00, 0x2f, 0x00, 0x07, 0x00, 0x40, 0x36, 0x36, 0x06, 0x00,
	0x00, 0x00, 0x06, 0x06, 0x00, 0x03, 0x00, 0xff, 0x06, 0x00,
	0x02, 0x00, 0x06, 0x00, 0x01, 0x00, 0xfe, 0x00, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x2e, 0x00, 0x13, 0x00, 0x01, 0x00, 0x2e, 0x00, 0x08, 0x00,
	0x01, 0x00, 0x2e, 0x00, 0x14, 0x00, 0x01, 0x00, 0x2e, 0x00,
	0x07, 0x00, 0x01, 0x00, 0xff, 0xff,
};
const byte ROOM_TABLE7[] = {
	0x00, 0x2f, 0x00, 0x0e, 0x00, 0x40, 0x32, 0x3b, 0x07, 0x00,
	0x00, 0x00, 0x07, 0x07, 0x00, 0x03, 0x00, 0xff, 0x07, 0x00,
	0x02, 0x00, 0x07, 0x00, 0x01, 0x00, 0xfe, 0x00, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x2e, 0x00, 0x14, 0x00, 0x01, 0x00, 0x2e, 0x00, 0x07, 0x00,
	0x01, 0x00, 0xff, 0xff,
};
const byte ROOM_TABLE8[] = {
	0x00, 0x2f, 0x00, 0x0a, 0x00, 0x30, 0x22, 0x46, 0x08, 0x00,
	0x00, 0x00, 0x08, 0x08, 0x00, 0x03, 0x00, 0xff, 0x08, 0x00,
	0x02, 0x00, 0x08, 0x00, 0x01, 0x00, 0xc0, 0x00, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0xff, 0xff,
};
const byte ROOM_TABLE9[] = {
	0x00, 0x2f, 0x00, 0x07, 0x00, 0x32, 0x0c, 0x29, 0x09, 0x00,
	0x00, 0x00, 0x09, 0x09, 0x00, 0x03, 0x00, 0xff, 0x09, 0x00,
	0x02, 0x00, 0x09, 0x00, 0x01, 0x00, 0xc8, 0x00, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0xff, 0xff,
};
const byte ROOM_TABLE11[] = {
	0x00, 0x2f, 0x00, 0x00, 0x00, 0x40, 0x3a, 0x22, 0x0b, 0x00,
	0x00, 0x00, 0x0b, 0x0b, 0x00, 0x03, 0x00, 0xff, 0x0b, 0x00,
	0x02, 0x00, 0x0b, 0x00, 0x01, 0x00, 0xfe, 0x00, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0xff, 0xff,
};
const byte ROOM_TABLE13[] = {
	0x00, 0x2f, 0x00, 0x0c, 0x00, 0x40, 0x36, 0x2c, 0x0d, 0x00,
	0x00, 0x00, 0x0d, 0x0d, 0x00, 0x03, 0x00, 0xff, 0x0d, 0x00,
	0x02, 0x00, 0x0d, 0x00, 0x01, 0x00, 0xe6, 0x40, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x2e, 0x00, 0x0a, 0x00, 0x01, 0x00, 0x2e, 0x00, 0x07, 0x00,
	0x01, 0x00, 0x2e, 0x00, 0x0b, 0x00, 0x01, 0x00, 0x2e, 0x00,
	0x15, 0x00, 0x01, 0x00, 0xff, 0xff,
};
const byte ROOM_TABLE14[] = {
	0x00, 0x2f, 0x00, 0x05, 0x00, 0x40, 0x3e, 0x33, 0x0e, 0x00,
	0x00, 0x00, 0x0e, 0x0e, 0x00, 0x03, 0x00, 0xff, 0x0e, 0x00,
	0x02, 0x00, 0x0e, 0x00, 0x01, 0x00, 0xfe, 0x40, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x2e, 0x00, 0x09, 0x00, 0x01, 0x00, 0x2e, 0x00, 0x07, 0x00,
	0x01, 0x00, 0x2e, 0x00, 0x13, 0x00, 0x01, 0x00, 0x2e, 0x00,
	0x0a, 0x00, 0x01, 0x00, 0xff, 0xff,
};
const byte ROOM_TABLE15[] = {
	0x00, 0x2f, 0x00, 0x0c, 0x00, 0x28, 0x0c, 0x5e, 0x0f, 0x00,
	0x00, 0x00, 0x0f, 0x0f, 0x00, 0x03, 0x00, 0xff, 0x0f, 0x00,
	0x02, 0x00, 0x0f, 0x00, 0x01, 0x00, 0xb4, 0x00, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x2e, 0x00, 0x11, 0x00, 0x01, 0x00, 0xff, 0xff,
};
const byte ROOM_TABLE16[] = {
	0x00, 0x2f, 0x00, 0x05, 0x00, 0x28, 0x1e, 0x24, 0x10, 0x00,
	0x00, 0x00, 0x10, 0x10, 0x00, 0x03, 0x00, 0xff, 0x10, 0x00,
	0x02, 0x00, 0x10, 0x00, 0x01, 0x00, 0xa0, 0x00, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x2e, 0x00, 0x07, 0x00, 0x01, 0x00, 0xff, 0xff,
};
const byte ROOM_TABLE17[] = {
	0x00, 0x2f, 0x00, 0x06, 0x00, 0x28, 0x19, 0x2b, 0x11, 0x00,
	0x00, 0x00, 0x11, 0x11, 0x00, 0x03, 0x00, 0xff, 0x11, 0x00,
	0x02, 0x00, 0x11, 0x00, 0x01, 0x00, 0xa0, 0x00, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x2e, 0x00, 0x05, 0x00, 0x01, 0x00, 0xff, 0xff,
};
const byte ROOM_TABLE18[] = {
	0x00, 0x2f, 0x00, 0x00, 0x00, 0x2d, 0x14, 0x3c, 0x12, 0x00,
	0x00, 0x00, 0x12, 0x12, 0x00, 0x03, 0x00, 0xff, 0x12, 0x00,
	0x02, 0x00, 0x12, 0x00, 0x01, 0x00, 0xb1, 0x40, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x2e, 0x00, 0x05, 0x00, 0x01, 0x00, 0xff, 0xff,
};
const byte ROOM_TABLE21[] = {
	0x00, 0x2f, 0x00, 0x07, 0x00, 0x3c, 0x2a, 0x29, 0x15, 0x00,
	0x00, 0x00, 0x15, 0x15, 0x00, 0x03, 0x00, 0xff, 0x15, 0x00,
	0x02, 0x00, 0x15, 0x00, 0x01, 0x00, 0xf0, 0x00, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x2e, 0x00, 0x12, 0x00, 0x01, 0x00, 0x2e, 0x00, 0x07, 0x00,
	0x01, 0x00, 0xff, 0xff,
};
const byte ROOM_TABLE22[] = {
	0x00, 0x2f, 0x00, 0x0a, 0x00, 0x40, 0x2d, 0x27, 0x16, 0x00,
	0x00, 0x00, 0x16, 0x16, 0x00, 0x03, 0x00, 0xff, 0x16, 0x00,
	0x02, 0x00, 0x16, 0x00, 0x01, 0x00, 0xfe, 0x00, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x2e, 0x00, 0x16, 0x00, 0x01, 0x00, 0x2e, 0x00, 0x07, 0x00,
	0x01, 0x00, 0xff, 0xff,
};
const byte ROOM_TABLE23[] = {
	0x00, 0x2f, 0x00, 0x0a, 0x00, 0x40, 0x38, 0x24, 0x17, 0x00,
	0x00, 0x00, 0x17, 0x17, 0x00, 0x03, 0x00, 0xff, 0x17, 0x00,
	0x02, 0x00, 0x17, 0x00, 0x01, 0x00, 0xfe, 0x40, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x2e, 0x00, 0x0f, 0x00, 0x01, 0x00, 0x2e, 0x00, 0x17, 0x00,
	0x01, 0x00, 0xff, 0xff,
};
const byte ROOM_TABLE24[] = {
	0x00, 0x2f, 0x00, 0x06, 0x00, 0x3e, 0x10, 0x62, 0x18, 0x00,
	0x00, 0x00, 0x18, 0x18, 0x00, 0x03, 0x00, 0xff, 0x18, 0x00,
	0x02, 0x00, 0x18, 0x00, 0x01, 0x00, 0xf8, 0x00, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x2e, 0x00, 0x16, 0x00, 0x01, 0x00, 0xff, 0xff,
};
const byte ROOM_TABLE25[] = {
	0x00, 0x2f, 0x00, 0x0e, 0x00, 0x3e, 0x37, 0x19, 0x19, 0x00,
	0x00, 0x00, 0x19, 0x19, 0x00, 0x03, 0x00, 0xff, 0x19, 0x00,
	0x02, 0x00, 0x19, 0x00, 0x01, 0x00, 0xf8, 0x00, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x2e, 0x00, 0x10, 0x00, 0x01, 0x00, 0x2e, 0x00, 0x07, 0x00,
	0x01, 0x00, 0xff, 0xff,
};
const byte ROOM_TABLE26[] = {
	0x00, 0x2f, 0x00, 0x06, 0x00, 0x34, 0x28, 0x28, 0x1a, 0x00,
	0x00, 0x00, 0x1a, 0x1a, 0x00, 0x03, 0x00, 0xff, 0x1a, 0x00,
	0x02, 0x00, 0x1a, 0x00, 0x01, 0x00, 0xd0, 0x00, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x2e, 0x00, 0x07, 0x00, 0x01, 0x00, 0xff, 0xff,
};
const byte ROOM_TABLE27[] = {
	0x00, 0x2f, 0x00, 0x0f, 0x00, 0x1b, 0x16, 0x18, 0x1b, 0x00,
	0x00, 0x00, 0x1b, 0x1b, 0x00, 0x03, 0x00, 0xff, 0x1b, 0x00,
	0x02, 0x00, 0x1b, 0x00, 0x01, 0x00, 0x70, 0x00, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x2e, 0x00, 0x0d, 0x00, 0x01, 0x00, 0x2e, 0x00, 0x07, 0x00,
	0x01, 0x00, 0xff, 0xff,
};
const byte ROOM_TABLE28[] = {
	0x00, 0x2f, 0x00, 0x09, 0x00, 0x25, 0x10, 0x43, 0x1c, 0x00,
	0x00, 0x00, 0x1c, 0x1c, 0x00, 0x03, 0x00, 0xff, 0x1c, 0x00,
	0x02, 0x00, 0x1c, 0x00, 0x01, 0x00, 0x94, 0x00, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0xff, 0xff,
};
const byte ROOM_TABLE29[] = {
	0x00, 0x2f, 0x00, 0x0a, 0x00, 0x20, 0x18, 0x56, 0x1d, 0x00,
	0x00, 0x00, 0x1d, 0x1d, 0x00, 0x03, 0x00, 0xff, 0x1d, 0x00,
	0x02, 0x00, 0x1d, 0x00, 0x01, 0x00, 0x80, 0x00, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x2e, 0x00, 0x17, 0x00, 0x01, 0x00, 0x2e, 0x00, 0x10, 0x00,
	0x02, 0x00, 0xff, 0xff,
};
const byte ROOM_TABLE30[] = {
	0x00, 0x2f, 0x00, 0x07, 0x00, 0x3f, 0x1c, 0x27, 0x1e, 0x00,
	0x00, 0x00, 0x1e, 0x1e, 0x00, 0x03, 0x00, 0xff, 0x1e, 0x00,
	0x02, 0x00, 0x1e, 0x00, 0x01, 0x00, 0xfe, 0x00, 0xff, 0xff,
	0xff, 0xff, 0x1e, 0x00, 0x04, 0x00, 0xff, 0xff, 0x2e, 0x00,
	0x00, 0x00, 0x01, 0x00, 0x2e, 0x00, 0x07, 0x00, 0x01, 0x00,
	0x2e, 0x00, 0x15, 0x00, 0x01, 0x00, 0xff, 0xff,
};
const byte ROOM_TABLE31[] = {
	0x00, 0x2f, 0x00, 0x0d, 0x00, 0x32, 0x2e, 0x69, 0x1f, 0x00,
	0x00, 0x00, 0x1f, 0x1f, 0x00, 0x03, 0x00, 0xff, 0x1f, 0x00,
	0x02, 0x00, 0x1f, 0x00, 0x01, 0x00, 0xc8, 0x00, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0xff, 0xff,
};
const byte ROOM_TABLE32[] = {
	0x00, 0x2f, 0x00, 0x07, 0x00, 0x40, 0x3b, 0x4b, 0x20, 0x00,
	0x00, 0x00, 0x20, 0x20, 0x00, 0x03, 0x00, 0xff, 0x20, 0x00,
	0x02, 0x00, 0x20, 0x00, 0x01, 0x00, 0xfe, 0x00, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x2e, 0x00, 0x05, 0x00, 0x01, 0x00, 0xff, 0xff,
};
const byte ROOM_TABLE33[] = {
	0x00, 0x2f, 0x00, 0x0b, 0x00, 0x30, 0x10, 0x51, 0x21, 0x00,
	0x00, 0x00, 0x21, 0x21, 0x00, 0x03, 0x00, 0xff, 0x21, 0x00,
	0x02, 0x00, 0x21, 0x00, 0x01, 0x00, 0xc0, 0x40, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0xff, 0xff,
};
const byte ROOM_TABLE35[] = {
	0x00, 0x2f, 0x00, 0x0f, 0x00, 0x1e, 0x18, 0x25, 0x23, 0x00,
	0x00, 0x00, 0x23, 0x23, 0x00, 0x03, 0x00, 0xff, 0x23, 0x00,
	0x02, 0x00, 0x23, 0x00, 0x01, 0x00, 0x78, 0x18, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x2e, 0x00, 0x0e, 0x00, 0x01, 0x00, 0x2e, 0x00, 0x07, 0x00,
	0x01, 0x00, 0x2e, 0x00, 0x16, 0x00, 0x01, 0x00, 0x2e, 0x00,
	0x0c, 0x00, 0x01, 0x00, 0xff, 0xff,
};
const byte ROOM_TABLE37[] = {
	0x00, 0x2f, 0x00, 0x0f, 0x00, 0x3f, 0x3a, 0x1a, 0x25, 0x00,
	0x00, 0x00, 0x25, 0x25, 0x00, 0x03, 0x00, 0xff, 0x25, 0x00,
	0x02, 0x00, 0x25, 0x00, 0x01, 0x00, 0xfe, 0x40, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x2e, 0x00, 0x0d, 0x00, 0x01, 0x00, 0xff, 0xff,
};
const byte ROOM_TABLE38[] = {
	0x00, 0x2f, 0x00, 0x0d, 0x00, 0x40, 0x32, 0x32, 0x26, 0x00,
	0x00, 0x00, 0x26, 0x26, 0x00, 0x03, 0x00, 0xff, 0x26, 0x00,
	0x02, 0x00, 0x26, 0x00, 0x01, 0x00, 0xf0, 0x00, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x2e, 0x00, 0x0b, 0x00, 0x01, 0x00, 0xff, 0xff,
};
const byte ROOM_TABLE39[] = {
	0x00, 0x2f, 0x00, 0x0a, 0x00, 0x3c, 0x10, 0x4c, 0x27, 0x00,
	0x00, 0x00, 0x27, 0x27, 0x00, 0x03, 0x00, 0xff, 0x27, 0x00,
	0x02, 0x00, 0x27, 0x00, 0x01, 0x00, 0xf0, 0x00, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0x2e, 0x00, 0x00, 0x00, 0x01, 0x00,
	0x2e, 0x00, 0x11, 0x00, 0x01, 0x00, 0x2e, 0x00, 0x0f, 0x00,
	0x01, 0x00, 0xff, 0xff,
};
const byte ROOM_TABLE47[] = {
	0x00, 0x2f, 0x00, 0x06, 0x00, 0x28, 0x1e, 0x32, 0x2b, 0x00,
	0x00, 0x00, 0x46, 0x2b, 0x00, 0x03, 0x00, 0xff, 0x2b, 0x00,
	0x02, 0x00, 0x2b, 0x00, 0x01, 0x00, 0xf0, 0x00, 0xff, 0xff,
	0xff, 0xff, 0x2b, 0x00, 0x04, 0x00, 0xff, 0xff, 0x2e, 0x00,
	0x04, 0x00, 0x01, 0x00, 0xff, 0xff, 0x00,
};
const byte *ROOM_TABLE[] = {
	nullptr, ROOM_TABLE1, ROOM_TABLE2, ROOM_TABLE3, ROOM_TABLE4, ROOM_TABLE5, ROOM_TABLE6,
	ROOM_TABLE7, ROOM_TABLE8, ROOM_TABLE9, nullptr, ROOM_TABLE11, nullptr, ROOM_TABLE13,
	ROOM_TABLE14, ROOM_TABLE15, ROOM_TABLE16, ROOM_TABLE17, ROOM_TABLE18, nullptr, nullptr,
	ROOM_TABLE21, ROOM_TABLE22, ROOM_TABLE23, ROOM_TABLE24, ROOM_TABLE25, ROOM_TABLE26, ROOM_TABLE27,
	ROOM_TABLE28, ROOM_TABLE29, ROOM_TABLE30, ROOM_TABLE31, ROOM_TABLE32, ROOM_TABLE33, nullptr,
	ROOM_TABLE35, nullptr, ROOM_TABLE37, ROOM_TABLE38, ROOM_TABLE39, nullptr, nullptr,
	nullptr, nullptr, nullptr, nullptr, nullptr, ROOM_TABLE47
};

const char *ROOM_DESCR[] = {
	nullptr, "TBD ROOM_TABLE1", "TBD ROOM_TABLE2", "TBD ROOM_TABLE3", "TBD ROOM_TABLE4",
	"TBD ROOM_TABLE5", "TBD ROOM_TABLE6", "TBD ROOM_TABLE7", "TBD ROOM_TABLE8", "TBD ROOM_TABLE9",
	nullptr, "TBD ROOM_TABLE11", nullptr, "TBD ROOM_TABLE13", "TBD ROOM_TABLE14",
	"TBD ROOM_TABLE15", "TBD ROOM_TABLE16", "TBD ROOM_TABLE17", "TBD ROOM_TABLE18", nullptr,
	nullptr, "TBD ROOM_TABLE21", "TBD ROOM_TABLE22", "TBD ROOM_TABLE23", "TBD ROOM_TABLE24",
	"TBD ROOM_TABLE25", "TBD ROOM_TABLE26", "TBD ROOM_TABLE27", "TBD ROOM_TABLE28", "TBD ROOM_TABLE29",
	"TBD ROOM_TABLE30", "TBD ROOM_TABLE31", "TBD ROOM_TABLE32", "TBD ROOM_TABLE33", nullptr,
	"TBD ROOM_TABLE35", nullptr, "TBD ROOM_TABLE37", "TBD ROOM_TABLE38", "TBD ROOM_TABLE39",
	nullptr, nullptr, nullptr, nullptr, nullptr,
	nullptr, nullptr, "TBD ROOM_TABLE47"
};

const int ROOM_NUMB = 48;

const byte CHAR_TABLE0[] = {
	0x02, 0x31, 0x00, 0x08, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
	0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
};
const byte CHAR_TABLE2[] = {
	0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0x00, 0x00, 0x00, 0x00, 0x32, 0x33, 0x00, 0x01, 0x00,
	0xff, 0xff, 0xff, 0xff, 0xff, 0x33, 0x00, 0x00, 0x00, 0x33,
	0x00, 0x02, 0x00, 0x33, 0x00, 0x0b, 0x00, 0x33, 0x00, 0x03,
	0x00, 0x33, 0x00, 0x0c, 0x00, 0x33, 0x00, 0x04, 0x00, 0x33,
	0x00, 0x0d, 0x00, 0x33, 0x00, 0x05, 0x00, 0x33, 0x00, 0x0e,
	0x00, 0x33, 0x00, 0x06, 0x00, 0x33, 0x00, 0x0f, 0x00, 0x33,
	0x00, 0x07, 0x00, 0x33, 0x00, 0x10, 0x00, 0x33, 0x00, 0x08,
	0x00, 0x33, 0x00, 0x11, 0x00, 0x33, 0x00, 0x09, 0x00, 0x33,
	0x00, 0x12, 0x00, 0x33, 0x00, 0x0a, 0x00, 0x33, 0x00, 0x13,
	0x00, 0xff, 0xff,
};
const byte CHAR_TABLE3[] = {
	0x02, 0x31, 0x00, 0x03, 0x00, 0x35, 0x00, 0x37, 0x00, 0x02,
	0x00, 0x80, 0x00, 0xf7, 0x00, 0x4b, 0x37, 0x00, 0x01, 0x00,
	0xff, 0x37, 0x00, 0x03, 0x00, 0x37, 0x00, 0x00, 0x00, 0xff,
	0xff,
};
const byte CHAR_TABLE4[] = {
	0x01, 0x31, 0x00, 0x0a, 0x00, 0x36, 0x00, 0x35, 0x00, 0x02,
	0x00, 0x80, 0x00, 0xf7, 0x00, 0x49, 0x35, 0x00, 0x01, 0x00,
	0xff, 0xff, 0xff, 0xff, 0xff, 0x35, 0x00, 0x00, 0x00, 0x35,
	0x00, 0x03, 0x00, 0x35, 0x00, 0x0c, 0x00, 0x35, 0x00, 0x04,
	0x00, 0x35, 0x00, 0x0d, 0x00, 0x35, 0x00, 0x05, 0x00, 0x35,
	0x00, 0x0e, 0x00, 0x35, 0x00, 0x06, 0x00, 0x35, 0x00, 0x0f,
	0x00, 0x35, 0x00, 0x07, 0x00, 0x35, 0x00, 0x10, 0x00, 0x35,
	0x00, 0x08, 0x00, 0x35, 0x00, 0x11, 0x00, 0x35, 0x00, 0x09,
	0x00, 0x35, 0x00, 0x12, 0x00, 0x35, 0x00, 0x0a, 0x00, 0x35,
	0x00, 0x13, 0x00, 0x35, 0x00, 0x0b, 0x00, 0x35, 0x00, 0x14,
	0x00, 0xff, 0xff,
};
const byte CHAR_TABLE5[] = {
	0x01, 0x31, 0x00, 0x08, 0x00, 0x37, 0x00, 0x34, 0x00, 0x02,
	0x00, 0x80, 0x00, 0xf7, 0x00, 0x48, 0x34, 0x00, 0x01, 0x00,
	0xff, 0xff, 0xff, 0xff, 0xff, 0x34, 0x00, 0x00, 0x00, 0x43,
	0x00, 0x00, 0x00, 0x34, 0x00, 0x03, 0x00, 0x43, 0x00, 0x01,
	0x00, 0x34, 0x00, 0x04, 0x00, 0x43, 0x00, 0x02, 0x00, 0x34,
	0x00, 0x05, 0x00, 0x43, 0x00, 0x03, 0x00, 0x34, 0x00, 0x06,
	0x00, 0x43, 0x00, 0x04, 0x00, 0x34, 0x00, 0x07, 0x00, 0x43,
	0x00, 0x05, 0x00, 0x34, 0x00, 0x08, 0x00, 0x43, 0x00, 0x06,
	0x00, 0x34, 0x00, 0x09, 0x00, 0x43, 0x00, 0x07, 0x00, 0x34,
	0x00, 0x0a, 0x00, 0x43, 0x00, 0x08, 0x00, 0x34, 0x00, 0x0b,
	0x00, 0x43, 0x00, 0x09, 0x00, 0x34, 0x00, 0x0c, 0x00, 0x43,
	0x00, 0x0a, 0x00, 0x34, 0x00, 0x0d, 0x00, 0x43, 0x00, 0x0b,
	0x00, 0x34, 0x00, 0x0e, 0x00, 0x43, 0x00, 0x0c, 0x00, 0x34,
	0x00, 0x0f, 0x00, 0x43, 0x00, 0x0d, 0x00, 0x34, 0x00, 0x10,
	0x00, 0xff, 0xff,
};
const byte CHAR_TABLE6[] = {
	0x02, 0x31, 0x00, 0x03, 0x00, 0x38, 0x00, 0x44, 0x00, 0x03,
	0x00, 0x80, 0x00, 0xf7, 0x00, 0x4e, 0x44, 0x00, 0x01, 0x00,
	0xff, 0x44, 0x00, 0x02, 0x00, 0x44, 0x00, 0x00, 0x00, 0xff,
	0xff,
};
const byte CHAR_TABLE7[] = {
	0x02, 0x31, 0x00, 0x01, 0x00, 0x39, 0x00, 0x38, 0x00, 0x02,
	0x00, 0x80, 0x00, 0xf7, 0x00, 0x4c, 0x38, 0x00, 0x01, 0x00,
	0xff, 0x38, 0x00, 0x03, 0x00, 0x38, 0x00, 0x00, 0x00, 0xff,
	0xff,
};
const byte CHAR_TABLE8[] = {
	0x03, 0xff, 0xff, 0xff, 0xff, 0x3a, 0x00, 0xff, 0xff, 0xff,
	0xff, 0x00, 0x00, 0x00, 0x00, 0x4d, 0x3b, 0x00, 0x01, 0x00,
	0xff, 0x3b, 0x00, 0x02, 0x00, 0x3b, 0x00, 0x00, 0x00, 0xff,
	0xff,
};
const byte CHAR_TABLE9[] = {
	0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0x00, 0x00, 0x00, 0x00, 0x59, 0x4a, 0x00, 0x01, 0x00,
	0xff, 0x4a, 0x00, 0x02, 0x00, 0x4a, 0x00, 0x00, 0x00, 0xff,
	0xff,
};
const byte CHAR_TABLE10[] = {
	0x01, 0x31, 0x00, 0x0a, 0x00, 0x3c, 0x00, 0x36, 0x00, 0x02,
	0x00, 0x80, 0x00, 0xf7, 0x00, 0x4a, 0x36, 0x00, 0x01, 0x00,
	0xff, 0xff, 0xff, 0xff, 0xff, 0x36, 0x00, 0x00, 0x00, 0x36,
	0x00, 0x03, 0x00, 0x36, 0x00, 0x13, 0x00, 0x36, 0x00, 0x04,
	0x00, 0x36, 0x00, 0x14, 0x00, 0x36, 0x00, 0x05, 0x00, 0x36,
	0x00, 0x15, 0x00, 0x36, 0x00, 0x06, 0x00, 0x36, 0x00, 0x16,
	0x00, 0x36, 0x00, 0x07, 0x00, 0x36, 0x00, 0x17, 0x00, 0x36,
	0x00, 0x08, 0x00, 0x36, 0x00, 0x18, 0x00, 0x36, 0x00, 0x09,
	0x00, 0x36, 0x00, 0x19, 0x00, 0x36, 0x00, 0x0a, 0x00, 0x36,
	0x00, 0x1a, 0x00, 0x36, 0x00, 0x0b, 0x00, 0x36, 0x00, 0x1b,
	0x00, 0x36, 0x00, 0x0c, 0x00, 0x36, 0x00, 0x1c, 0x00, 0x36,
	0x00, 0x0d, 0x00, 0x36, 0x00, 0x1d, 0x00, 0x36, 0x00, 0x0e,
	0x00, 0x36, 0x00, 0x1e, 0x00, 0x36, 0x00, 0x0f, 0x00, 0x36,
	0x00, 0x1f, 0x00, 0x36, 0x00, 0x10, 0x00, 0x36, 0x00, 0x20,
	0x00, 0x36, 0x00, 0x11, 0x00, 0x36, 0x00, 0x21, 0x00, 0x36,
	0x00, 0x12, 0x00, 0x36, 0x00, 0x22, 0x00, 0xff, 0xff,
};
const byte CHAR_TABLE11[] = {
	0x03, 0xff, 0xff, 0xff, 0xff, 0x3d, 0x00, 0xff, 0xff, 0xff,
	0xff, 0x00, 0x00, 0x00, 0x00, 0x55, 0x45, 0x00, 0x01, 0x00,
	0xff, 0x45, 0x00, 0x02, 0x00, 0x45, 0x00, 0x00, 0x00, 0xff,
	0xff,
};
const byte CHAR_TABLE12[] = {
	0x03, 0xff, 0xff, 0xff, 0xff, 0x3e, 0x00, 0xff, 0xff, 0xff,
	0xff, 0x00, 0x00, 0x00, 0x00, 0x4f, 0x40, 0x00, 0x01, 0x00,
	0xff, 0x40, 0x00, 0x02, 0x00, 0x40, 0x00, 0x00, 0x00, 0xff,
	0xff,
};
const byte CHAR_TABLE13[] = {
	0x00, 0xff, 0xff, 0xff, 0xff, 0x3f, 0x00, 0x46, 0x00, 0x02,
	0x00, 0x80, 0x00, 0xf7, 0x00, 0x56, 0x46, 0x00, 0x01, 0x00,
	0xff, 0x46, 0x00, 0x03, 0x00, 0x46, 0x00, 0x00, 0x00, 0x46,
	0x00, 0x04, 0x00, 0x46, 0x00, 0x0d, 0x00, 0x46, 0x00, 0x05,
	0x00, 0x46, 0x00, 0x0e, 0x00, 0x46, 0x00, 0x06, 0x00, 0x46,
	0x00, 0x0f, 0x00, 0x46, 0x00, 0x07, 0x00, 0x46, 0x00, 0x10,
	0x00, 0x46, 0x00, 0x08, 0x00, 0x46, 0x00, 0x11, 0x00, 0x46,
	0x00, 0x09, 0x00, 0x46, 0x00, 0x12, 0x00, 0x46, 0x00, 0x0a,
	0x00, 0x46, 0x00, 0x13, 0x00, 0x46, 0x00, 0x0b, 0x00, 0x46,
	0x00, 0x14, 0x00, 0x46, 0x00, 0x0c, 0x00, 0x46, 0x00, 0x15,
	0x00, 0xff, 0xff,
};
const byte CHAR_TABLE15[] = {
	0x00, 0xff, 0xff, 0xff, 0xff, 0x41, 0x00, 0xff, 0xff, 0xff,
	0xff, 0x00, 0x00, 0x00, 0x00, 0x57, 0x47, 0x00, 0x01, 0x00,
	0xff, 0xff, 0xff, 0xff, 0xff, 0x47, 0x00, 0x00, 0x00, 0x47,
	0x00, 0x02, 0x00, 0x47, 0x00, 0x05, 0x00, 0x47, 0x00, 0x03,
	0x00, 0x47, 0x00, 0x06, 0x00, 0x47, 0x00, 0x04, 0x00, 0x47,
	0x00, 0x07, 0x00, 0xff, 0xff,
};
const byte CHAR_TABLE16[] = {
	0x03, 0xff, 0xff, 0xff, 0xff, 0x42, 0x00, 0xff, 0xff, 0xff,
	0xff, 0x00, 0x00, 0x00, 0x00, 0x54, 0x41, 0x00, 0x01, 0x00,
	0xff, 0x41, 0x00, 0x02, 0x00, 0x41, 0x00, 0x00, 0x00, 0xff,
	0xff,
};
const byte CHAR_TABLE18[] = {
	0x02, 0x31, 0x00, 0x07, 0x00, 0x44, 0x00, 0x3c, 0x00, 0x03,
	0x00, 0x80, 0x00, 0xf7, 0x00, 0x50, 0x3c, 0x00, 0x01, 0x00,
	0xff, 0x3c, 0x00, 0x02, 0x00, 0x3c, 0x00, 0x00, 0x00, 0xff,
	0xff,
};
const byte CHAR_TABLE19[] = {
	0x02, 0x31, 0x00, 0x07, 0x00, 0x45, 0x00, 0x3d, 0x00, 0x03,
	0x00, 0x80, 0x00, 0xf7, 0x00, 0x51, 0x3d, 0x00, 0x01, 0x00,
	0xff, 0x3d, 0x00, 0x02, 0x00, 0x3d, 0x00, 0x00, 0x00, 0xff,
	0xff,
};
const byte CHAR_TABLE20[] = {
	0x02, 0x31, 0x00, 0x02, 0x00, 0x46, 0x00, 0x48, 0x00, 0x02,
	0x00, 0x80, 0x00, 0xf7, 0x00, 0x58, 0x48, 0x00, 0x01, 0x00,
	0xff, 0x48, 0x00, 0x03, 0x00, 0x48, 0x00, 0x00, 0x00, 0xff,
	0xff,
};
const byte CHAR_TABLE21[] = {
	0x02, 0x31, 0x00, 0x07, 0x00, 0x47, 0x00, 0x3e, 0x00, 0x03,
	0x00, 0x80, 0x00, 0xf7, 0x00, 0x52, 0x3e, 0x00, 0x01, 0x00,
	0xff, 0x3e, 0x00, 0x02, 0x00, 0x3e, 0x00, 0x00, 0x00, 0xff,
	0xff,
};
const byte CHAR_TABLE23[] = {
	0x02, 0x31, 0x00, 0x08, 0x00, 0x49, 0x00, 0x3f, 0x00, 0x03,
	0x00, 0x80, 0x00, 0xf7, 0x00, 0x53, 0x3f, 0x00, 0x01, 0x00,
	0xff, 0x3f, 0x00, 0x02, 0x00, 0x3f, 0x00, 0x00, 0x00, 0xff,
	0xff,
};
const byte CHAR_TABLE24[] = {
	0x02, 0x32, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0x00, 0x00, 0x00, 0x00, 0x47, 0x32, 0x00, 0x02, 0x00,
	0xff, 0xff, 0xff, 0xff, 0xff, 0x32, 0x00, 0x01, 0x00, 0x32,
	0x00, 0x03, 0x00, 0x32, 0x00, 0x0a, 0x00, 0x32, 0x00, 0x04,
	0x00, 0x32, 0x00, 0x0b, 0x00, 0x32, 0x00, 0x05, 0x00, 0x32,
	0x00, 0x0c, 0x00, 0x32, 0x00, 0x06, 0x00, 0x32, 0x00, 0x0d,
	0x00, 0x32, 0x00, 0x07, 0x00, 0x32, 0x00, 0x0e, 0x00, 0x32,
	0x00, 0x08, 0x00, 0x32, 0x00, 0x0f, 0x00, 0x32, 0x00, 0x09,
	0x00, 0x32, 0x00, 0x10, 0x00, 0xff, 0xff
};
const byte CHAR_TABLE25[] = {
	0x02, 0x39, 0x00, 0x01, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x39, 0x00, 0x00, 0x00, 0x39, 0x00, 0x02, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xFF, 0xFF
};
const byte CHAR_TABLE26[] = {
	0x01, 0x3a, 0x00, 0x01, 0x00, 0x0a, 0x00, 0xff, 0xff, 0xff,
	0xff, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x3a, 0x00, 0x02, 0x00,
	0xff, 0x3a, 0x00, 0x03, 0x00, 0x3a, 0x00, 0x00, 0x00, 0x42,
	0x00, 0x00, 0x00, 0x3a, 0x00, 0x04, 0x00, 0x42, 0x00, 0x01,
	0x00, 0x3a, 0x00, 0x05, 0x00, 0x42, 0x00, 0x02, 0x00, 0x3a,
	0x00, 0x06, 0x00, 0x42, 0x00, 0x03, 0x00, 0x3a, 0x00, 0x07,
	0x00, 0x42, 0x00, 0x04, 0x00, 0x3a, 0x00, 0x08, 0x00, 0x42,
	0x00, 0x05, 0x00, 0x3a, 0x00, 0x09, 0x00, 0x42, 0x00, 0x06,
	0x00, 0x3a, 0x00, 0x0a, 0x00, 0x42, 0x00, 0x07, 0x00, 0x3a,
	0x00, 0x0b, 0x00, 0x42, 0x00, 0x08, 0x00, 0x3a, 0x00, 0x0c,
	0x00, 0x42, 0x00, 0x09, 0x00, 0x3a, 0x00, 0x0d, 0x00, 0x42,
	0x00, 0x0a, 0x00, 0x3a, 0x00, 0x0e, 0x00, 0x42, 0x00, 0x0b,
	0x00, 0x3a, 0x00, 0x0f, 0x00, 0x42, 0x00, 0x0c, 0x00, 0x3a,
	0x00, 0x10, 0x00, 0x42, 0x00, 0x0d, 0x00, 0x3a, 0x00, 0x11,
	0x00, 0x42, 0x00, 0x0e, 0x00, 0x3a, 0x00, 0x12, 0x00, 0x42,
	0x00, 0x0f, 0x00, 0x3a, 0x00, 0x13, 0x00, 0x42, 0x00, 0x10,
	0x00, 0x3a, 0x00, 0x14, 0x00, 0x42, 0x00, 0x11, 0x00, 0x3a,
	0x00, 0x15, 0x00, 0xff, 0xff
};
const byte CHAR_TABLE27[] = {
	0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0x00, 0x00, 0x00, 0x00, 0x58, 0x49, 0x00, 0x01, 0x00,
	0xff, 0xff, 0xff, 0xff, 0xff, 0x49, 0x00, 0x00, 0x00, 0x49,
	0x00, 0x02, 0x00, 0x49, 0x00, 0x0a, 0x00, 0x49, 0x00, 0x03,
	0x00, 0x49, 0x00, 0x0b, 0x00, 0x49, 0x00, 0x04, 0x00, 0x49,
	0x00, 0x0c, 0x00, 0x49, 0x00, 0x05, 0x00, 0x49, 0x00, 0x0d,
	0x00, 0x49, 0x00, 0x06, 0x00, 0x49, 0x00, 0x0e, 0x00, 0x49,
	0x00, 0x07, 0x00, 0x49, 0x00, 0x0f, 0x00, 0x49, 0x00, 0x08,
	0x00, 0x49, 0x00, 0x10, 0x00, 0x49, 0x00, 0x09, 0x00, 0x49,
	0x00, 0x11, 0x00, 0xff, 0xff,
};
const byte *CHAR_TABLE[] = {
	CHAR_TABLE0, nullptr, CHAR_TABLE2, CHAR_TABLE3, CHAR_TABLE4, CHAR_TABLE5,
	CHAR_TABLE6, CHAR_TABLE7, CHAR_TABLE8, CHAR_TABLE9, CHAR_TABLE10,
	CHAR_TABLE11, CHAR_TABLE12, CHAR_TABLE13, nullptr, CHAR_TABLE15,
	CHAR_TABLE16, nullptr, CHAR_TABLE18, CHAR_TABLE19, CHAR_TABLE20,
	CHAR_TABLE21, nullptr, CHAR_TABLE23, CHAR_TABLE24, CHAR_TABLE25,
	CHAR_TABLE26, CHAR_TABLE27
};

// TODO: Fix that array
const int COMBO_TABLE[54][4] = {
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 },
	{ -1, -1, -1, -1 }
};

} // End of namespace Martian

} // End of namespace Access
