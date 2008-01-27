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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL$
 * $Id$
 *
 */
 
#include "kyra/kyra_v2.h"
#include "kyra/timer.h"

namespace Kyra {

#define TimerV2(x) new Functor1Mem<int, void, KyraEngine_v2>(this, &KyraEngine_v2::x)

void KyraEngine_v2::setupTimers() {
	debugC(9, kDebugLevelMain | kDebugLevelTimer, "KyraEngine_v2::setupTimers()");
	
	_timer->addTimer(0, 0, 5, 1);
	_timer->addTimer(1, TimerV2(timerFunc2), -1, 1);
	_timer->addTimer(2, TimerV2(timerCauldronAnimation), 1, 1);
	_timer->addTimer(3, TimerV2(timerFunc4), 1, 0);
	_timer->addTimer(4, TimerV2(timerFunc5), 1, 0);
	_timer->addTimer(5, TimerV2(timerFunc6), 1, 0);
}

void KyraEngine_v2::timerFunc2(int arg) {
	debugC(9, kDebugLevelMain | kDebugLevelTimer, "KyraEngine_v2::timerFunc2(%d)", arg);
	if (_shownMessage)
		_msgUnk1 = 1;
}

void KyraEngine_v2::timerCauldronAnimation(int arg) {
	debugC(9, kDebugLevelMain | kDebugLevelTimer, "KyraEngine_v2::timerCauldronAnimation(%d)", arg);
	int animation = -1;

	if (queryGameFlag(2) && _mainCharacter.sceneId != 34 && _mainCharacter.sceneId != 73 && !_invWsa.wsa && !_invWsa.running) {
		if (animation == -1)
			animation = _rnd.getRandomNumberRng(1, 6);

		char filename[13];
		strcpy(filename, "CAULD00.WSA");
		filename[5] = (animation / 10) + '0';
		filename[6] = (animation % 10) + '0';
		loadInvWsa(filename, 0, 8, 0, -1, -1, 1);
	}
}

void KyraEngine_v2::timerFunc4(int arg) {
	debugC(9, kDebugLevelMain | kDebugLevelTimer, "KyraEngine_v2::timerFunc4(%d)", arg);
	_timer->disable(3);
	setGameFlag(0xD8);
}

void KyraEngine_v2::timerFunc5(int arg) {
	debugC(9, kDebugLevelMain | kDebugLevelTimer, "KyraEngine_v2::timerFunc5(%d)", arg);
	_timer->disable(4);
	_screen->hideMouse();
	_specialSceneScriptState[5] = 1;
	for (int i = 68; i <= 75; ++i) {
		updateSceneAnim(4, i);
		delay(6);
	}
	//_unk1 = 4;
}

void KyraEngine_v2::timerFunc6(int arg) {
	debugC(9, kDebugLevelMain | kDebugLevelTimer, "KyraEngine_v2::timerFunc6(%d)", arg);
	_timer->disable(5);
	_screen->hideMouse();
	snd_playSoundEffect(0x2D);
	runTemporaryScript("_ZANBURN.EMC", 0, 1, 1, 0);
	//_unk1 = 7;
	snd_playWanderScoreViaMap(0x53, 1);
}

void KyraEngine_v2::setTimer1DelaySecs(int secs) {
	debugC(9, kDebugLevelMain | kDebugLevelTimer, "KyraEngine_v2::setTimer1DelaySecs(%d)", secs);

	if (secs == -1)
		secs = 32000;
	
	_timer->setCountdown(1, secs * 60);
}

void KyraEngine_v2::setWalkspeed(uint8 newSpeed) {
	debugC(9, kDebugLevelMain | kDebugLevelTimer, "KyraEngine_v2::setWalkspeed(%i)", newSpeed);

	if (newSpeed < 5)
		newSpeed = 3;
	else
		newSpeed = 5;

	_timer->setDelay(5, newSpeed);
}


} // end of namespace Kyra
