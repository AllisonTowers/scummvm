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

#include <common/system.h>


#include "trecision/nl/define.h"
#include "trecision/nl/extern.h"
#include "trecision/nl/message.h"
#include "trecision/nl/proto.h"
#include "trecision/nl/struct.h"
#include "trecision/logic.h"
#include "trecision/trecision.h"

namespace Trecision {

void TrecisionEngine::initNames() {
	// Initial value of useful system texts.
	// Those values are overwritten by the ones from the game files later
	_sysText[kMessageSavePosition] = "SAVE POSITION";
	_sysText[kMessageEmptySpot] = "EMPTY SLOT";
	_sysText[kMessageLoadPosition] = "LOAD POSITION";
	_sysText[kMessageConfirmExit] = "Are you sure that you want to quit (y/n)?";
	_sysText[kMessageDemoOver] = "This demo is over.";
	_sysText[kMessageError] = "ERROR!";
	_sysText[kMessageUse] = "Use ";
	_sysText[kMessageWith] = " with ";
	_sysText[kMessageGoto] = "Go to ";
	_sysText[kMessageGoto2] = "Go to ... ";

	_sentence[0] = "          "; // Use it like a buffer !!!!
	_objName[0] = " ";
}

char *TrecisionEngine::getNextSentence() {
	while (*_textPtr) {
		*_textPtr = ~(*_textPtr);
		_textPtr++;
	}

	_textPtr++;
	return _textPtr;
}

void TrecisionEngine::addText(uint16 x, uint16 y, const char *text, uint16 tcol, uint16 scol) {
	_textStackTop++;
	if (_textStackTop >= MAXTEXTSTACK) {
		warning("MAXTEXTSTACK Reached!");
		return;
	}

	_textStack[_textStackTop].x = x;
	_textStack[_textStackTop].y = y;
	_textStack[_textStackTop].tcol = tcol;
	_textStack[_textStackTop].scol = scol;
	_textStack[_textStackTop].clear = false;
	strcpy(_textStack[_textStackTop].text, text);
}

void TrecisionEngine::clearText() {
	if (_textStackTop >= 0) {
		// The stack isn't empty
		if (!_textStack[_textStackTop].clear)
			// The previous is a string to write, return
			_textStackTop--;
	} else {
		// the stack is empty
		_textStackTop = 0;
		_textStack[_textStackTop].clear = true;
	}
}

void TrecisionEngine::drawString() {
	for (int16 i = 0; i <= _textStackTop; i++) {
		if (_textStack[i].clear)
			doClearText();
		else
			_textStack[i].doText();
	}
}

void TrecisionEngine::redrawString() {
	if (!_flagDialogActive && !_flagDialogMenuActive && !_flagSomeoneSpeaks && !_flagscriptactive && isCursorVisible()) {
		if (isInventoryArea(_mouseY))
			doEvent(MC_INVENTORY, ME_SHOWICONNAME, MP_DEFAULT, 0, 0, 0, 0);
		else {
			CheckMask(_mouseX, _mouseY);
			ShowObjName(_curObj, true);
		}
	}
}

void StackText::doText() {
	curString.x = x;
	curString.y = y;
	curString.dx = g_vm->TextLength(text, 0);
	if ((y == MAXY - CARHEI) && (curString.dx > 600))
		curString.dx = curString.dx * 3 / 5;
	else if ((y != MAXY - CARHEI) && (curString.dx > 960))
		curString.dx = curString.dx * 2 / 5;
	else if ((y != MAXY - CARHEI) && (curString.dx > 320))
		curString.dx = curString.dx * 3 / 5;

	curString.text = text;
	curString._subtitleRect.left = 0;
	curString._subtitleRect.top = 0;
	curString._subtitleRect.right = curString.dx;
	uint16 hstring = curString.checkDText();
	curString._subtitleRect.bottom = hstring;
	curString.dy = hstring;
	curString.tcol = tcol;
	curString.scol = scol;

	if (curString.y <= hstring)
		curString.y += hstring;
	else
		curString.y -= hstring;

	if (curString.y <= VIDEOTOP)
		curString.y = VIDEOTOP + 1;

	TextStatus |= TEXT_DRAW;
}

void TrecisionEngine::doClearText() {
	if (!oldString.text && curString.text) {
		oldString.set(curString);
		curString.text = nullptr;

		TextStatus |= TEXT_DEL;
	}
}

void TrecisionEngine::setRoom(uint16 r, bool b) {
	_logicMgr->setupAltRoom(r, b);
	RegenRoom();
}

/*-----------------17/02/95 09.53-------------------
 TextLength - Compute string length from character 0 to num
--------------------------------------------------*/
uint16 TrecisionEngine::TextLength(const char *text, uint16 num) {
	if (text == nullptr)
		return 0;

	uint16 len = (num == 0) ? strlen(text) : num;

	uint16 retVal = 0;
	for (uint16 c = 0; c < len; c++)
		retVal += _font[(uint8)text[c] * 3 + 2];

	return retVal;
}

char TrecisionEngine::GetKey() {
	Common::KeyCode key = _curKey;
	uint16 ascii = _curAscii;
	_curKey = Common::KEYCODE_INVALID;
	_curAscii = 0;

	switch (key) {
	case Common::KEYCODE_SPACE:
	case Common::KEYCODE_ESCAPE:
	case Common::KEYCODE_RETURN:
	case Common::KEYCODE_CLEAR:
	case Common::KEYCODE_BACKSPACE:
		return key;
	case Common::KEYCODE_F1:
	case Common::KEYCODE_F2:
	case Common::KEYCODE_F3:
	case Common::KEYCODE_F4:
	case Common::KEYCODE_F5:
	case Common::KEYCODE_F6:
		return 0x3B + key - Common::KEYCODE_F1;
	default:
		if (ascii) {
			return ascii;
		}

		return 0;
	}
}

char TrecisionEngine::waitKey() {
	while (_curKey == Common::KEYCODE_INVALID)
		checkSystem();

	Common::KeyCode t = _curKey;
	_curKey = Common::KEYCODE_INVALID;

	return t;
}

void TrecisionEngine::NlDelay(uint32 val) {
	uint32 sv = ReadTime();

	while (sv + val > ReadTime())
		checkSystem();
}

void TrecisionEngine::FreeKey() {
	_curKey = Common::KEYCODE_INVALID;
}

uint32 TrecisionEngine::ReadTime() {
	return (g_system->getMillis() * 3) / 50;
}

bool TrecisionEngine::CheckMask(uint16 mx, uint16 my) {
	for (int8 a = MAXOBJINROOM - 1; a >= 0; a--) {
		uint16 checkedObj = _room[_curRoom]._object[a];
		Common::Rect lim = _obj[checkedObj]._lim;
		lim.translate(0, TOP);
		// trecision includes the bottom and right coordinates
		lim.right++;
		lim.bottom++;

		if (checkedObj && (_obj[checkedObj]._mode & OBJMODE_OBJSTATUS)) {
			if (lim.contains(mx, my)) {

				if ((_obj[checkedObj]._mode & OBJMODE_FULL) || (_obj[checkedObj]._mode & OBJMODE_LIM)) {
					_curObj = checkedObj;
					return true;
				}

				if (_obj[checkedObj]._mode & OBJMODE_MASK) {
					uint8 *mask = MaskPointers[a];
					int16 d = _obj[checkedObj]._px;
					uint16 max = _obj[checkedObj]._py + _obj[checkedObj]._dy;

					for (uint16 b = _obj[checkedObj]._py; b < max; b++) {
						bool insideObj = false;
						int16 e = 0;
						while (e < _obj[checkedObj]._dx) {
							if (!insideObj) { // not inside an object
								if (b + TOP == my) {
									if ((mx >= d + e) && (mx < d + e + *mask)) {
										_curObj = 0;
									}
								}

								e += *mask;
								mask++;
								insideObj = true;
							} else { // inside an object
								if (b + TOP == my) {
									if ((mx >= d + e) && (mx < d + e + *mask)) {
										_curObj = checkedObj;
										return true;
									}
								}

								e += *mask;
								mask++;
								insideObj = false;
							}
						}
					}
				}
			}
		}
	}
	_curObj = 0;
	return false;
}

Graphics::Surface *TrecisionEngine::convertScummVMThumbnail(Graphics::Surface *thumbnail) {
	Graphics::Surface *thumbnailConverted = thumbnail->convertTo(g_system->getScreenFormat());
	Graphics::Surface *result = thumbnailConverted->scale(ICONDX, ICONDY);

	thumbnailConverted->free();
	delete thumbnailConverted;
	thumbnail->free();
	delete thumbnail;

	return result;
}

void TrecisionEngine::resetZBuffer(int x1, int y1, int x2, int y2) {
	if (x1 > x2 || y1 > y2)
		return;

	int size = (x2 - x1) * (y2 - y1);
	if (size * 2 > ZBUFFERSIZE)
		warning("Warning: _zBuffer size %d!\n", size * 2);

	int16 *d = _zBuffer;
	for (int i = 0; i < size; ++i)
		*d++ = 0x7FFF;
}

} // End of namespace Trecision
