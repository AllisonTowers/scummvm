/* ScummVM - Scumm Interpreter
 * Copyright (C) 2003 The ScummVM project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header$
 *
 */

#include "stdafx.h"
#include "screen.h"
#include "logic.h"
#include "sworddefs.h"
#include "text.h"
#include "resman.h"
#include "objectman.h"
#include "scummsys.h"
#include "common/util.h"
#include "system.h"
#include "menu.h"

#define SCROLL_FRACTION 16
#define MAX_SCROLL_DISTANCE 8
#define FADE_UP 1
#define FADE_DOWN -1

SwordScreen::SwordScreen(OSystem *system, ResMan *pResMan, ObjectMan *pObjMan) {
	_system = system;
	_resMan = pResMan;
	_objMan = pObjMan;
	_screenBuf = _screenGrid = NULL;
	_backLength = _foreLength = _sortLength = 0;
	_fadingStep = 0;
}

void SwordScreen::useTextManager(SwordText *pTextMan) {
	_textMan = pTextMan;
}

int32 SwordScreen::inRange(int32 a, int32 b, int32 c) { // return b(!) so that: a <= b <= c
	return (a > b) ? (a) : ((b < c) ? b : c);
}

void SwordScreen::setScrolling(int16 offsetX, int16 offsetY) {
	if (!SwordLogic::_scriptVars[SCROLL_FLAG])
		return ; // screen is smaller than 640x400 => no need for scrolling

	uint32 scrlToX, scrlToY;

	offsetX = inRange(0, offsetX, SwordLogic::_scriptVars[MAX_SCROLL_OFFSET_X]);
	offsetY = inRange(0, offsetY, SwordLogic::_scriptVars[MAX_SCROLL_OFFSET_Y]);
	_oldScrollX = SwordLogic::_scriptVars[SCROLL_OFFSET_X];
	_oldScrollY = SwordLogic::_scriptVars[SCROLL_OFFSET_Y];
	scrlToX = (uint32)offsetX;
	scrlToY = (uint32)offsetY;

	if (SwordLogic::_scriptVars[SCROLL_FLAG] == 2) // first time on this screen - need absolute scroll immediately!
		SwordLogic::_scriptVars[SCROLL_FLAG] = 1;
	scrlToX = inRange(0, scrlToX, SwordLogic::_scriptVars[MAX_SCROLL_OFFSET_X]);
	scrlToY = inRange(0, scrlToY, SwordLogic::_scriptVars[MAX_SCROLL_OFFSET_Y]);
	if ((scrlToX != SwordLogic::_scriptVars[SCROLL_OFFSET_X]) || (scrlToY != SwordLogic::_scriptVars[SCROLL_OFFSET_Y])) {
		_fullRefresh = true;
		SwordLogic::_scriptVars[SCROLL_OFFSET_X] = scrlToX;
		SwordLogic::_scriptVars[SCROLL_OFFSET_Y] = scrlToY;
	} else
		_fullRefresh = false;
	if (SwordLogic::_scriptVars[SCROLL_FLAG] == 2) {
		_oldScrollX = scrlToX;
		_oldScrollY = scrlToY;
		SwordLogic::_scriptVars[SCROLL_FLAG] = 1;
	}
}

void SwordScreen::fadeDownPalette(void) {
	if (!_isBlack) { // don't fade down twice
		_fadingStep = 15;
		_fadingDirection = FADE_DOWN;
	}
}

void SwordScreen::fadeUpPalette(void) {
	_fadingStep = 15;
	_fadingDirection = FADE_UP;
}

void SwordScreen::fnSetPalette(uint8 start, uint16 length, uint32 id, bool fadeUp) {
	uint8 *palData = (uint8*)_resMan->openFetchRes(id);
	if (start == 0) // force color 0 to black
		palData[0] = palData[1] = palData[2] = 0;
	for (uint32 cnt = 0; cnt < length; cnt++) {
		_targetPalette[(start + cnt) * 4 + 0] = palData[cnt * 3 + 0] << 2;
		_targetPalette[(start + cnt) * 4 + 1] = palData[cnt * 3 + 1] << 2;
		_targetPalette[(start + cnt) * 4 + 2] = palData[cnt * 3 + 2] << 2;
	}
	_resMan->resClose(id);
	_isBlack = false;
	if (fadeUp) {
		_fadingStep = 1;
		_fadingDirection = FADE_UP;
	} else
		_system->set_palette(_targetPalette, start, length);
}

bool SwordScreen::stillFading(void) {
	return !_isBlack;
}

void SwordScreen::updateScreen(void) {
	if (SwordLogic::_scriptVars[NEW_PALETTE]) {
		_fadingStep = 1;
		_fadingDirection = FADE_UP;
		memcpy(_targetPalette, _resMan->openFetchRes(_roomDefTable[_currentScreen].palettes[0]), 184 * 3); // set colours 0-183 for background palette
		_resMan->resClose(_roomDefTable[_currentScreen].palettes[0]);
		memcpy(_targetPalette + 184 * 3, _resMan->openFetchRes(_roomDefTable[_currentScreen].palettes[1]),  72 * 3); // set colours 184-255 for sprite palette - george, icons & menubar
		_resMan->resClose(_roomDefTable[_currentScreen].palettes[1]);
		SwordLogic::_scriptVars[NEW_PALETTE] = 0;
	}
	if (_fadingStep) {
		fadePalette();
		_system->set_palette(_currentPalette, 0, 256);
	}

	uint16 scrlX = (uint16)SwordLogic::_scriptVars[SCROLL_OFFSET_X];
	uint16 scrlY = (uint16)SwordLogic::_scriptVars[SCROLL_OFFSET_Y];
	_fullRefresh = true;
	if (_fullRefresh) {
		_fullRefresh = false;
		uint16 copyWidth = SCREEN_WIDTH;
		uint16 copyHeight = SCREEN_DEPTH;
		if (scrlX + copyWidth > _scrnSizeX)
			copyWidth = _scrnSizeX - scrlX;
		if (scrlY + copyHeight > _scrnSizeY)
			copyHeight = _scrnSizeY - scrlY;
		_system->copy_rect(_screenBuf + scrlY * _scrnSizeX + scrlX, _scrnSizeX, 0, 40, copyWidth, copyHeight);
	} else {
		// partial screen update only. The screen coordinates probably won't fit to the
		// grid holding the informations on which blocks have to be updated.
		// as the grid will be X pixel higher and Y pixel more to the left, this can be cured
		// by first checking the top border, then the left column and then the remaining (aligned) part.
		uint8 *gridPos = _screenGrid + (scrlX / SCRNGRID_X) + (scrlY / SCRNGRID_Y) * _gridSizeX;
		uint8 *scrnBuf = _screenBuf + scrlY * _scrnSizeX + scrlX;
		uint8 diffX = (uint8)(scrlX % SCRNGRID_X);
		uint8 diffY = (uint8)(scrlY % SCRNGRID_Y);
		uint16 gridW = SCREEN_WIDTH / SCRNGRID_X;
		uint16 gridH = SCREEN_DEPTH / SCRNGRID_Y;
		if (diffY) {
			uint8 cpHeight = SCRNGRID_Y - diffY;
			uint16 cpWidth = 0;
			for (uint16 cntx = 0; cntx < gridW; cntx++) 
				if (gridPos[cntx] & 1) {
					gridPos[cntx] &= ~1;
					cpWidth++;
				} else if (cpWidth) {
					int16 xPos = (cntx - cpWidth) * SCRNGRID_X - diffX;
					if (xPos < 0)
						xPos = 0;
					_system->copy_rect(scrnBuf + xPos, _scrnSizeX, xPos, 40, cpWidth * SCRNGRID_X, cpHeight);
					cpWidth = 0;
				}
			if (cpWidth) {
				int16 xPos = (gridW - cpWidth) * SCRNGRID_X - diffX;
				if (xPos < 0)
					xPos = 0;
				_system->copy_rect(scrnBuf + xPos, _scrnSizeX, xPos, 40, SCREEN_WIDTH - xPos, cpHeight);
			}
		} // okay, y scrolling is compensated. check x now.
		gridPos += _gridSizeX;
		scrnBuf = _screenBuf + scrlX + diffY * _scrnSizeX;
		if (diffX) {
			uint8 cpWidth = SCRNGRID_X - diffX;
			uint16 cpHeight = 0;
			for (uint16 cnty = 0; cnty < gridH; cnty++)
				if (gridPos[cnty * SCRNGRID_X] & 1) {
					gridPos[cnty * SCRNGRID_X] &= ~1;
					cpHeight++;
				} else if (cpHeight) {
					uint16 yPos = (cnty - cpHeight) * SCRNGRID_Y;
					_system->copy_rect(scrnBuf + yPos * _scrnSizeX, _scrnSizeX, 0, yPos + diffY + 40, cpWidth, cpHeight * SCRNGRID_Y);
				}
			if (cpHeight) {
				uint16 yPos = (gridH - cpHeight) * SCRNGRID_Y;
				_system->copy_rect(scrnBuf + yPos * _scrnSizeX, _scrnSizeX, 0, yPos + diffY + 40, cpWidth, SCREEN_DEPTH - (yPos + diffY));
			}			
		} // x scroll is compensated, too. check the rest of the screen, now.
		scrlX = (scrlX + SCRNGRID_X - 1) &~ (SCRNGRID_X - 1);
		scrlY = (scrlY + SCRNGRID_Y - 1) &~ (SCRNGRID_Y - 1);
		scrnBuf = _screenBuf + scrlY * _scrnSizeX + scrlX;
		gridPos++;
		for (uint16 cnty = 0; cnty < gridH; cnty++) {
			uint16 cpWidth = 0;
			uint16 cpHeight = SCRNGRID_Y;
			if (cnty == gridH - 1)
				cpHeight = SCRNGRID_Y - diffY;
			for (uint16 cntx = 0; cntx < gridW; cntx++)
				if (gridPos[cntx] & 1) {
					gridPos[cntx] &= ~1;
					cpWidth++;
				} else if (cpWidth) {
					_system->copy_rect(scrnBuf + (cntx - cpWidth) * SCRNGRID_X, _scrnSizeX, (cntx - cpWidth) * SCRNGRID_X + diffX, cnty * SCRNGRID_Y + diffY + 40, cpWidth * SCRNGRID_X, cpHeight);
				}
			if (cpWidth) {
				uint16 xPos = (gridW - cpWidth) * SCRNGRID_X;
				_system->copy_rect(scrnBuf + xPos, _scrnSizeX, xPos + diffX, cnty * SCRNGRID_Y + diffY + 40, SCREEN_WIDTH - (xPos + diffX), cpHeight);
			}
			gridPos += _gridSizeX;
			scrnBuf += _scrnSizeX * SCRNGRID_Y;
		}
	}
	_system->update_screen();
}

void SwordScreen::newScreen(uint32 screen) {
	// set sizes and scrolling, initialize/load screengrid, force screen refresh
	// force palette fadeup?
	_currentScreen = screen;
	_scrnSizeX = _roomDefTable[screen].sizeX;
	_scrnSizeY = _roomDefTable[screen].sizeY;
	_gridSizeX = _scrnSizeX / SCRNGRID_X;
	_gridSizeY = _scrnSizeY / SCRNGRID_Y;
	if ((_scrnSizeX % SCRNGRID_X) || (_scrnSizeY % SCRNGRID_Y))
		error("Illegal screensize: %d: %d/%d", screen, _scrnSizeX, _scrnSizeY);
	if ((_scrnSizeX > SCREEN_WIDTH) || (_scrnSizeY > SCREEN_DEPTH)) {
		SwordLogic::_scriptVars[SCROLL_FLAG] = 2;
		SwordLogic::_scriptVars[MAX_SCROLL_OFFSET_X] = _scrnSizeX - SCREEN_WIDTH;
		SwordLogic::_scriptVars[MAX_SCROLL_OFFSET_Y] = _scrnSizeY - SCREEN_DEPTH;
	} else {
		SwordLogic::_scriptVars[SCROLL_FLAG] = 0;
		SwordLogic::_scriptVars[MAX_SCROLL_OFFSET_X] = 0;
		SwordLogic::_scriptVars[MAX_SCROLL_OFFSET_Y] = 0;
		SwordLogic::_scriptVars[SCROLL_OFFSET_X] = 0;
		SwordLogic::_scriptVars[SCROLL_OFFSET_Y] = 0;
	}
	_screenBuf = (uint8*)malloc(_scrnSizeX * _scrnSizeY);
	_screenGrid = (uint8*)malloc(_gridSizeX * _gridSizeY);
	memset(_screenGrid, 0x80, _gridSizeX * _gridSizeY); // force refresh
	for (uint8 cnt = 0; cnt < _roomDefTable[_currentScreen].totalLayers; cnt++) {
		// open and lock all resources, will be closed in quitScreen()
		_layerBlocks[cnt] = (uint8*)_resMan->openFetchRes(_roomDefTable[_currentScreen].layers[cnt]);
		if (cnt > 0)
			_layerBlocks[cnt] += sizeof(Header);
	}
	for (uint8 cnt = 0; cnt < _roomDefTable[_currentScreen].totalLayers - 1; cnt++) {
		// there's no grid for the background layer, so it's totalLayers - 1
		_layerGrid[cnt] = (uint16*)_resMan->openFetchRes(_roomDefTable[_currentScreen].grids[cnt]);
 		_layerGrid[cnt] += 14;
	}
	_parallax[0] = _parallax[1] = NULL;
	if (_roomDefTable[_currentScreen].parallax[0])
		_parallax[0] = (uint8*)_resMan->openFetchRes(_roomDefTable[_currentScreen].parallax[0]);
	if (_roomDefTable[_currentScreen].parallax[1])
		_parallax[1] = (uint8*)_resMan->openFetchRes(_roomDefTable[_currentScreen].parallax[1]);

	fnSetPalette(0, 184, _roomDefTable[_currentScreen].palettes[0], true);
	fnSetPalette(184, 72, _roomDefTable[_currentScreen].palettes[1], true);
}

void SwordScreen::quitScreen(void) {
	for (uint8 cnt = 0; cnt < _roomDefTable[_currentScreen].totalLayers; cnt++)
		_resMan->resClose(_roomDefTable[_currentScreen].layers[cnt]);
	for (uint8 cnt = 0; cnt < _roomDefTable[_currentScreen].totalLayers - 1; cnt++)
		_resMan->resClose(_roomDefTable[_currentScreen].grids[cnt]);
	if (_roomDefTable[_currentScreen].parallax[0])
		_resMan->resClose(_roomDefTable[_currentScreen].parallax[0]);
	if (_roomDefTable[_currentScreen].parallax[1])
		_resMan->resClose(_roomDefTable[_currentScreen].parallax[1]);
}

void SwordScreen::recreate() {
	memcpy(_screenBuf, _layerBlocks[0], _scrnSizeX * _scrnSizeY);
}

void SwordScreen::spritesAndParallax(void) {
	if ((_currentScreen == 54) && _parallax[0])
		renderParallax(_parallax[0]); // rm54 has a BACKGROUND parallax layer in parallax[0]

	for (uint8 cnt = 0; cnt < _backLength; cnt++)
		processImage(_backList[cnt]);

	SortSpr temp;
	for (uint8 cnt = 0; cnt < _sortLength - 1; cnt++)
		for (uint8 sCnt = 0; sCnt < _sortLength - 1; sCnt++)
			if (_sortList[sCnt].y > _sortList[sCnt + 1].y) {
                temp = _sortList[sCnt];
				_sortList[sCnt] = _sortList[sCnt + 1];
				_sortList[sCnt + 1] = temp;
			}
	for (uint8 cnt = 0; cnt < _sortLength; cnt++)
		processImage(_sortList[cnt].id);

	if ((_currentScreen != 54) && _parallax[0])
		renderParallax(_parallax[0]); // screens other than 54 have FOREGROUND parallax layer in parallax[0]
	if (_parallax[1])
		renderParallax(_parallax[1]);

	for (uint8 cnt = 0; cnt < _foreLength; cnt++)
		processImage(_foreList[cnt]);

	_backLength = _sortLength = _foreLength = 0;
}

void SwordScreen::processImage(uint32 id) {
	BsObject *compact;
	FrameHeader *frameHead;
	int scale;

	compact = _objMan->fetchObject(id);
	if (compact->o_type == TYPE_TEXT)
		frameHead = _textMan->giveSpriteData((uint8)compact->o_target);
	else
		frameHead = _resMan->fetchFrame(_resMan->openFetchRes(compact->o_resource), compact->o_frame);
	
	uint8 *sprData = ((uint8*)frameHead) + sizeof(FrameHeader);

	uint16 spriteX = compact->o_anim_x;
	uint16 spriteY = compact->o_anim_y;
	if (compact->o_status & STAT_SHRINK) {
		scale = (compact->o_scale_a * compact->o_ycoord + compact->o_scale_b) / 256;
		spriteX += (FROM_LE_16(frameHead->offsetX) * scale) / 256;
		spriteY += (FROM_LE_16(frameHead->offsetY) * scale) / 256;
	} else {
		scale = 256;
		spriteX += FROM_LE_16(frameHead->offsetX);
		spriteY += FROM_LE_16(frameHead->offsetY);
	}
	if (scale > 512)
		debug(1, "compact %d is oversized: scale = %d", id, scale);

	uint8 *tonyBuf = NULL;
	if (frameHead->runTimeComp[3] == '7') { // RLE7 encoded?
		decompressRLE7(sprData, FROM_LE_32(frameHead->compSize), _rleBuffer);
		sprData = _rleBuffer;
	} else if (frameHead->runTimeComp[3] == '0') { // RLE0 encoded?
		decompressRLE0(sprData, FROM_LE_32(frameHead->compSize), _rleBuffer, FROM_LE_16(frameHead->width));
		sprData = _rleBuffer;
	} else if (frameHead->runTimeComp[1] == 'I') { // new type
		tonyBuf = (uint8*)malloc(FROM_LE_16(frameHead->width) * FROM_LE_16(frameHead->height));
		decompressTony(sprData, FROM_LE_32(frameHead->compSize), tonyBuf);
		sprData = tonyBuf;
	}

	uint16 sprSizeX, sprSizeY;
	if (compact->o_status & STAT_SHRINK) {
		sprSizeX = (scale * FROM_LE_16(frameHead->width)) / 256;
		sprSizeY = (scale * FROM_LE_16(frameHead->height)) / 256;
		fastShrink(sprData, FROM_LE_16(frameHead->width), FROM_LE_16(frameHead->height), scale, _shrinkBuffer);
		sprData = _shrinkBuffer;
	} else {
		sprSizeX = FROM_LE_16(frameHead->width);
		sprSizeY = FROM_LE_16(frameHead->height);
	}
	if (!(compact->o_status & STAT_OVERRIDE)) {
		//mouse size linked to exact size & coordinates of sprite box - shrink friendly
		if ((frameHead->offsetX) || (frameHead->offsetY)) {
			//for megas the mouse area is reduced to account for sprite not
			//filling the box size is reduced to 1/2 width, 4/5 height
			compact->o_mouse_x1 = spriteX + sprSizeX / 4;
			compact->o_mouse_x2 = spriteX + (3 * sprSizeX) / 4;
			compact->o_mouse_y1 = spriteY + sprSizeY / 10;
			compact->o_mouse_y2 = spriteY + (9 * sprSizeY) / 10;
		} else {
			compact->o_mouse_x1 = spriteX;
			compact->o_mouse_x2 = spriteX + sprSizeX;
			compact->o_mouse_y1 = spriteY;
			compact->o_mouse_y2 = spriteY + sprSizeY;
		}
	}
	uint16 sprPitch = sprSizeX;
	uint16 incr;
	spriteClipAndSet(&spriteX, &spriteY, &sprSizeX, &sprSizeY, &incr);
	if ((sprSizeX > 0) && (sprSizeY > 0)) {
		drawSprite(sprData + incr, spriteX, spriteY, sprSizeX, sprSizeY, sprPitch);
		if (!(compact->o_status&STAT_FORE))
			verticalMask(spriteX, spriteY, sprSizeX, sprSizeY);
	}
	if (compact->o_type != TYPE_TEXT)
		_resMan->resClose(compact->o_resource);
	if (tonyBuf)
		free(tonyBuf);
}

void SwordScreen::verticalMask(uint16 x, uint16 y, uint16 bWidth, uint16 bHeight) {
	if (_roomDefTable[_currentScreen].totalLayers <= 1)
		return;
	bWidth = (bWidth + (SCRNGRID_X - 1)) / SCRNGRID_X;
	bHeight = (bHeight + (SCRNGRID_Y - 1)) / SCRNGRID_Y;
	
	if (x & (SCRNGRID_X - 1))
		bWidth++;
	if (y & (SCRNGRID_Y - 1))
		bHeight++;

	x /= SCRNGRID_X;
	y /= SCRNGRID_Y;
	if (x + bWidth > _gridSizeX)
		bWidth = _gridSizeX - x;
	if (y + bHeight > _gridSizeY)
		bHeight = _gridSizeY - y;

	uint16 gridY = y + SCREEN_TOP_EDGE / SCRNGRID_Y; // imaginary screen on top
	uint16 gridX = x + SCREEN_LEFT_EDGE / SCRNGRID_X; // imaginary screen left
	uint16 lGridSizeX = _gridSizeX + 2 * (SCREEN_LEFT_EDGE / SCRNGRID_X); // width of the grid for the imaginary screen

	for (uint16 blkx = 0; blkx < bWidth; blkx++) {
		uint16 level = 0;
		while ((level < _roomDefTable[_currentScreen].totalLayers - 1) && 
			(!_layerGrid[level][gridX + blkx + (gridY + bHeight - 1)* lGridSizeX]))
			level++;
		if (level < _roomDefTable[_currentScreen].totalLayers - 1) {
			uint16 *grid = _layerGrid[level] + gridX + blkx + gridY * lGridSizeX;
			for (uint16 blky = 0; blky < bHeight; blky++) {
				if (*grid) {
					uint8 *blkData = _layerBlocks[level + 1] + (READ_LE_UINT16(grid) - 1) * 128;
			        blitBlockClear(x + blkx, y + blky, blkData);
				}
				grid += lGridSizeX;
			}
		}
	}
}

void SwordScreen::blitBlockClear(uint16 x, uint16 y, uint8 *data) {
	uint8 *dest = _screenBuf + (y * SCRNGRID_Y) * _scrnSizeX + (x * SCRNGRID_X);
	for (uint8 cnty = 0; cnty < SCRNGRID_Y; cnty++) {
		for (uint8 cntx = 0; cntx < SCRNGRID_X; cntx++)
			if (data[cntx])
				dest[cntx] = data[cntx];
		data += SCRNGRID_X;
		dest += _scrnSizeX;
	}
}

void SwordScreen::renderParallax(uint8 *data) {
	ParallaxHeader *header = (ParallaxHeader*)data;
	assert((FROM_LE_16(header->sizeX) >= SCREEN_WIDTH) && (FROM_LE_16(header->sizeY) >= SCREEN_DEPTH));
	double scrlfx =  FROM_LE_16(header->sizeX) / ((double)_scrnSizeX );
	double scrlfy = FROM_LE_16(header->sizeY) / ((double)_scrnSizeY );
	uint16 scrlX = (uint16)(SwordLogic::_scriptVars[SCROLL_OFFSET_X] * scrlfx);
	uint16 scrlY = (uint16)(SwordLogic::_scriptVars[SCROLL_OFFSET_Y] * scrlfy);

	for (uint16 cnty = 0; cnty < SCREEN_DEPTH; cnty++) {
		uint8 *src = data + READ_LE_UINT32(header->lineIndexes + cnty + scrlY);
		uint8 *dest = _screenBuf + SwordLogic::_scriptVars[SCROLL_OFFSET_X] + cnty * _scrnSizeX;
		uint16 remain = scrlX;
		uint16 xPos = 0;
		bool copyFirst = false;
		while (remain) { // skip past the first part of the parallax to get to the right scrolling position
			uint8 doSkip = *src++;
			if (doSkip <= remain)
				remain -= doSkip;
			else {
                xPos = doSkip - remain;
				dest += xPos;
				remain = 0;
			}
			if (remain) {
				uint8 doCopy = *src++;
				if (doCopy <= remain) {
					remain -= doCopy;
					src += doCopy;
				} else {
					uint16 remCopy = doCopy - remain;
					memcpy(dest, src + remain, remCopy);
					dest += remCopy;
					src += doCopy;
					xPos = remCopy;
					remain = 0;
				}
			} else
				copyFirst = true;
		}
		while (xPos < SCREEN_WIDTH) {
			if (!copyFirst) {
				if (uint8 skip = *src++) {
					dest += skip;
					xPos += skip;
				}
			} else
				copyFirst = false;
			if (xPos < SCREEN_WIDTH) {
				if (uint8 doCopy = *src++) {
					if (xPos + doCopy > SCREEN_WIDTH)
						doCopy = SCREEN_WIDTH - xPos;
					memcpy(dest, src, doCopy);
					dest += doCopy;
					xPos += doCopy;
					src += doCopy;
				}
			}
		}
	}
}

void SwordScreen::drawSprite(uint8 *sprData, uint16 sprX, uint16 sprY, uint16 sprWidth, uint16 sprHeight, uint16 sprPitch) {
	uint8 *dest = _screenBuf + (sprY * _scrnSizeX) + sprX;
	for (uint16 cnty = 0; cnty < sprHeight; cnty++) {
		for (uint16 cntx = 0; cntx < sprWidth; cntx++)
			if (sprData[cntx])
				dest[cntx] = sprData[cntx];
		sprData += sprPitch;
		dest += _scrnSizeX;
	}
}

// nearest neighbor filter:
void SwordScreen::fastShrink(uint8 *src, uint32 width, uint32 height, uint32 scale, uint8 *dest) {
	uint32 resHeight = (height * scale) >> 8;
	uint32 resWidth = (width * scale) >> 8;
	uint32 step = 0x10000 / scale;
	uint8 columnTab[160];
	uint32 res = step >> 1;
	for (uint16 cnt = 0; cnt < resWidth; cnt++) {
		columnTab[cnt] = (uint8)(res >> 8);
		res += step;
	}

	uint32 newRow = step >> 1;
	uint32 oldRow = 0;

    uint8 *destPos = dest;
	for (uint16 lnCnt = 0; lnCnt < resHeight; lnCnt++) {
		while (oldRow < (newRow >> 8)) {
			oldRow++;
			src += width;
		}
		for (uint16 colCnt = 0; colCnt < resWidth; colCnt++) {
			*destPos++ = src[columnTab[colCnt]];
		}
		newRow += step;
	}
	// scaled, now stipple shadows if there are any
	for (uint16 lnCnt = 0; lnCnt < resHeight; lnCnt++) {
		uint16 xCnt = lnCnt & 1;
		destPos = dest + lnCnt * resWidth + (lnCnt & 1);
		while (xCnt < resWidth) {
			if (*destPos == 200)
				*destPos = 0;
			destPos += 2;
			xCnt += 2;
		}
	}
}

void SwordScreen::addToGraphicList(uint8 listId, uint32 objId) {
	if (listId == 0) {
		_foreList[_foreLength++] = objId;
		if (_foreLength > MAX_FORE)
			error("foreList exceeded!");
	}
	if (listId == 1) {
		BsObject *cpt = _objMan->fetchObject(objId);
		_sortList[_sortLength].id = objId;
		_sortList[_sortLength].y = cpt->o_anim_y; // gives feet coords if boxed mega, otherwise top of sprite box
		if (!(cpt->o_status & STAT_SHRINK)) {     // not a boxed mega using shrinking
			Header *frameRaw = (Header*)_resMan->openFetchRes(cpt->o_resource);
			FrameHeader *frameHead = _resMan->fetchFrame(frameRaw, cpt->o_frame);
			_sortList[_sortLength].y += frameHead->height - 1; // now pointing to base of sprite
			_resMan->resClose(cpt->o_resource);
		}
		_sortLength++;
		if (_sortLength > MAX_SORT)
			error("sortList exceeded!");
	}
	if (listId == 2) {
		_backList[_backLength++] = objId;
		if (_backLength > MAX_BACK)
			error("backList exceeded!");
	}
}

void SwordScreen::decompressTony(uint8 *src, uint32 compSize, uint8 *dest) {
	uint8 *endOfData = src + compSize;
	while (src < endOfData) {
		uint8 numFlat = *src++;
		if (numFlat) {
			memset(dest, *src, numFlat);
			src++;
			dest += numFlat;
		}
		if (src < endOfData) {
			uint8 numNoFlat = *src++;
			memcpy(dest, src, numNoFlat);
			src += numNoFlat;
			dest += numNoFlat;
		}
	}
}

void SwordScreen::decompressRLE7(uint8 *src, uint32 compSize, uint8 *dest) {
	uint8 *compBufEnd = src + compSize;
	while (src < compBufEnd) {
		uint8 code = *src++;
		if ((code > 127) || (code == 0))
			*dest++ = code;
		else {
			code++;
			memset(dest, *src++, code);
			dest += code;
		}
	}
}

void SwordScreen::decompressRLE0(uint8 *src, uint32 compSize, uint8 *dest, uint16 width) {
	// these are saved vertically flipped. *SIIIIIIIGH*
	uint8 *srcBufEnd = src + compSize;
	uint16 destX = width-1;
	while (src < srcBufEnd) {
		uint8 color = *src++;
		if (color) {
			dest[destX] = color;
			if (destX == 0) {
				destX = width-1;
				dest += width;
			} else
				destX--;
		} else {
			uint8 skip = *src++;
			for (uint16 cnt = 0; cnt < skip; cnt++) {
				dest[destX] = 0;
				if (destX == 0) {
					destX = width-1;
					dest += width;
				} else
					destX--;
			}
		}
	}
}

void SwordScreen::fadePalette(void) {
	if (_fadingStep == 16)
		memcpy(_currentPalette, _targetPalette, 256 * 4);
	else
		for (uint16 cnt = 0; cnt < 256 * 4; cnt++)
			_currentPalette[cnt] = (_targetPalette[cnt] * _fadingStep) >> 4;

	_fadingStep += _fadingDirection;
	if (_fadingStep == 17) {
		_fadingStep = 0;
		_isBlack = false;
	} else if (_fadingStep == 0)
		_isBlack = true;
}

void SwordScreen::fnSetParallax(uint32 screen, uint32 resId) {
	if ((screen == _currentScreen) && (resId != _roomDefTable[screen].parallax[0]))
		warning("fnSetParallax: setting parallax for current room!!");
	_roomDefTable[screen].parallax[0] = resId;
}

void SwordScreen::spriteClipAndSet(uint16 *pSprX, uint16 *pSprY, uint16 *pSprWidth, uint16 *pSprHeight, uint16 *incr) {
	int16 sprX = *pSprX - SCREEN_LEFT_EDGE;
	int16 sprY = *pSprY - SCREEN_TOP_EDGE;
	int16 sprW = *pSprWidth;
	int16 sprH = *pSprHeight;
	
	if (sprY < 0) {
		*incr = (uint16)((-sprY) * sprW);
		sprH += sprY;
		sprY = 0;
	} else
		*incr = 0;
	if (sprX < 0) {
		*incr -= sprX;
		sprW += sprX;
		sprX = 0;
	}
	
	if (sprY + sprH > _scrnSizeY)
		sprH = _scrnSizeY - sprY;
	if (sprX + sprW > _scrnSizeX)
		sprW = _scrnSizeX - sprX;
    
	if (sprH < 0)
		*pSprHeight = 0;
	else
		*pSprHeight = (uint16)sprH;
	if (sprW < 0)
		*pSprWidth = 0;
	else
		*pSprWidth = (uint16)sprW;
	*pSprX = (uint16)sprX;
	*pSprY = (uint16)sprY;
	
	uint16 gridH = (*pSprHeight + SCRNGRID_Y - 1) / SCRNGRID_Y;
	uint16 gridW = (*pSprWidth + SCRNGRID_X - 1) / SCRNGRID_X;
	uint16 gridX = sprX / SCRNGRID_X;
	uint16 gridY = sprY / SCRNGRID_Y;
	uint8 *gridBuf = _screenGrid + gridX + gridY * _gridSizeX;
	if (gridX + gridW > _gridSizeX)
		gridW = _gridSizeX - gridX;
	if (gridY + gridH > _gridSizeY)
		gridH = _gridSizeY - gridY;

	for (uint16 cnty = 0; cnty < gridH; cnty++) {
		for (uint16 cntx = 0; cntx < gridW; cntx++)
			gridBuf[cntx] |= 0x80;
		gridBuf += _gridSizeX;
	}
}

void SwordScreen::fnFlash(uint8 color) {
	warning("stub: SwordScreen::fnFlash(%d)", color);
}

// ------------------- SwordMenu screen interface ---------------------------

void SwordScreen::showFrame(uint16 x, uint16 y, uint32 resId, uint32 frameNo) {
	FrameHeader *frameHead = _resMan->fetchFrame(_resMan->openFetchRes(resId), frameNo);
	uint8 *frameData = ((uint8*)frameHead) + sizeof(FrameHeader);
	_system->copy_rect(frameData, FROM_LE_16(frameHead->width), x, y, FROM_LE_16(frameHead->width), FROM_LE_16(frameHead->height));
	_resMan->resClose(resId);
}

void SwordScreen::clearMenu(uint8 menuType) {
	// isn't there a better way to do this?
	uint8 *tmp = (uint8*)malloc(640 * 40);
	memset(tmp, 0, 640 * 40);
	if (menuType == MENU_BOT)
		_system->copy_rect(tmp, 640, 0, 440, 640, 40);
	else
		_system->copy_rect(tmp, 640, 0, 0, 640, 40);
	free(tmp);
}

// ------------------- router debugging code --------------------------------

void SwordScreen::vline(uint16 x, uint16 y1, uint16 y2) {
	for (uint16 cnty = y1; cnty <= y2; cnty++)
		_screenBuf[x + _scrnSizeX * cnty] = 0;
}

void SwordScreen::hline(uint16 x1, uint16 x2, uint16 y) {
    for (uint16 cntx = x1; cntx <= x2; cntx++)
		_screenBuf[y * _scrnSizeX + cntx] = 0;
}

void SwordScreen::bsubline_1(uint16 x1, uint16 y1, uint16 x2, uint16 y2) {
	int x, y, ddx, ddy, e;
    ddx = abs(x2 - x1);
    ddy = abs(y2 - y1) << 1;
    e = ddx - ddy;
    ddx <<= 1;
    
    if (x1 > x2) {
		uint16 tmp;
		tmp = x1; x1 = x2; x2 = tmp;
		tmp = y1; y1 = y2; y2 = tmp;
    }
    
    for (x = x1, y = y1; x <= x2; x++) {
		_screenBuf[y * _scrnSizeX + x] = 0;
		if (e < 0) {
		    y++;
		    e += ddx - ddy;
		} else {
		    e -= ddy;
		}
    }
}

void SwordScreen::bsubline_2(uint16 x1, uint16 y1, uint16 x2, uint16 y2) {
	int x, y, ddx, ddy, e;
    ddx = abs(x2 - x1) << 1;
    ddy = abs(y2 - y1);
    e = ddy - ddx;
    ddy <<= 1;
    
    if (y1 > y2) {
		uint16 tmp;
		tmp = x1; x1 = x2; x2 = tmp;
		tmp = y1; y1 = y2; y2 = tmp;
    }
    
    for (y = y1, x = x1; y <= y2; y++) {
		_screenBuf[y * _scrnSizeX + x] = 0;
		if (e < 0) {
			x++;
			e += ddy - ddx;
		} else {
			e -= ddx;
		}
    }
}

void SwordScreen::bsubline_3(uint16 x1, uint16 y1, uint16 x2, uint16 y2) {
	int x, y, ddx, ddy, e;
    ddx = abs(x1 - x2) << 1;
    ddy = abs(y2 - y1);
    e = ddy - ddx;
    ddy <<= 1;
    
    if (y1 > y2) {
		uint16 tmp;
		tmp = x1; x1 = x2; x2 = tmp;
		tmp = y1; y1 = y2; y2 = tmp;
    }

    for (y = y1, x = x1; y <= y2; y++) {
		_screenBuf[y * _scrnSizeX + x] = 0;
		if (e < 0) {
			x--;
		    e += ddy - ddx;
		} else {
			e -= ddx;
		}
    }
}

void SwordScreen::bsubline_4(uint16 x1, uint16 y1, uint16 x2, uint16 y2) {
	int x, y, ddx, ddy, e;
    ddy = abs(y2 - y1) << 1;
    ddx = abs(x1 - x2);
    e = ddx - ddy;
    ddx <<= 1;
    
    if (x1 > x2) {
		uint16 tmp;
		tmp = x1; x1 = x2; x2 = tmp;
		tmp = y1; y1 = y2; y2 = tmp;
    }
    
    for (x = x1, y = y1; x <= x2; x++) {
		_screenBuf[y * _scrnSizeX + x] = 0;
		if (e < 0) {
		    y--;
		    e += ddx - ddy;
		} else {
		    e -= ddy;
		}
    }
}

void SwordScreen::drawLine(uint16 x1, uint16 y1, uint16 x2, uint16 y2) {
	if ((x1 == x2) && (y1 == y2)) {
		_screenBuf[x1 + y1 * _scrnSizeX] = 0;
	}
    if (x1 == x2) {
		vline(x1, MIN(y1, y2), MAX(y1, y2));
		return;
    }
    
    if (y1 == y2) {
		hline(MIN(x1, x2), MAX(x1, x2), y1);
		return;
    }

    float k = float(y2 - y1) / float(x2 - x1);
    
    if ((k >= 0) && (k <= 1)) {
		bsubline_1(x1, y1, x2, y2);
    } else if (k > 1) {
		bsubline_2(x1, y1, x2, y2);
    } else if ((k < 0) && (k >= -1)) {
		bsubline_4(x1, y1, x2, y2);
    } else {
		bsubline_3(x1, y1, x2, y2);
    }
}
