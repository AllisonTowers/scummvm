/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "efh/efh.h"

namespace Efh {

int16 EfhEngine::sub1C219(Common::String str, int16 menuType, int16 displayOption, bool displayTeamWindowFl) {
	debug("sub1C219 %s %d %d %s", str.c_str(), menuType, displayOption, displayTeamWindowFl ? "True" : "False");

	int16 varA = 0xFF;
	int16 minX, maxX, minY, maxY;

	switch (menuType) {
	case 0:
		minX = 129;
		minY = 9;
		maxX = 302;
		maxY = 18;
		break;
	case 1:
		minX = 129;
		minY = 9;
		maxX = 302;
		maxY = 110;
		break;
	case 2:
		minX = 129;
		minY = 112;
		maxX = 302;
		maxY = 132;
		break;
	case 3:
		minX = 129;
		minY = 79;
		maxX = 303;
		maxY = 107;
		break;
	default:
		minX = minY = 0;
		maxX = 320;
		maxY = 200;
		break;
	}

	drawColoredRect(minX, minY, maxX, maxY, 0);
	if (!str.empty())
		varA = script_parse(str, minX, minY, maxX, maxY, true);

	if (displayTeamWindowFl)
		displayLowStatusScreen(false);

	if (displayOption != 0) {
		displayFctFullScreen();
		if (_word2C87A)
			_word2C87A = false;
		else {
			drawColoredRect(minX, minY, maxX, maxY, 0);
			if (!str.empty())
				script_parse(str, minX, minY, maxX, maxY, false);
		}

		if (displayTeamWindowFl)
			displayLowStatusScreen(false);

		if (displayOption >= 2)
			getLastCharAfterAnimCount(_guessAnimationAmount);

		if (displayOption == 3)
			drawColoredRect(minX, minY, maxX, maxY, 0);
	}

	return varA;
}

bool EfhEngine::handleDeathMenu() {
	debugC(3, kDebugEngine, "handleDeathMenu");

	_saveAuthorized = false;

	displayAnimFrames(20, true);
	_imageSetSubFilesIdx = 213;
	drawScreen();

	for (uint counter = 0; counter < 2; ++counter) {
		clearBottomTextZone(0);
		displayCenteredString("Darkness Prevails...Death Has Taken You!", 24, 296, 153);
		setTextPos(100, 162);
		setTextColorWhite();
		displayCharAtTextPos('L');
		setTextColorRed();
		displayStringAtTextPos("oad last saved game");
		setTextPos(100, 171);
		setTextColorWhite();
		displayCharAtTextPos('R');
		setTextColorRed();
		displayStringAtTextPos("estart from beginning");
		setTextPos(100, 180);
		setTextColorWhite();
		displayCharAtTextPos('Q');
		setTextColorRed();
		displayStringAtTextPos("uit for now");
		if (counter == 0)
			displayFctFullScreen();
	}

	for (bool found = false; !found;) {
		Common::KeyCode input = waitForKey();
		switch (input) {
		case Common::KEYCODE_l:
			// SaveEfhGame opens the GUI save/load screen. It's not possible to save at this point (_saveAuthorizd is false).
			// If the user actually loads a savegame, it'll get _saveAuthorized from the savegame (always true) and will set 'found' to true.
			// If 'found' remains false, it means the user cancelled the loading and still needs to take a decision
			// Original is calling loadEfhGame() because there's only one savegame, so it's not ambiguous
			saveEfhGame();
			found = _saveAuthorized;
			break;
		case Common::KEYCODE_q:
			_shouldQuit = true;
			return true;
			break;
		case Common::KEYCODE_r:
			loadEfhGame();
			resetGame();
			found = true;
			_saveAuthorized = true;
			break;
		case Common::KEYCODE_x: // mysterious and unexpected keycode ?
			found = true;
			break;
		default:
			break;
		}
	}

	displayAnimFrames(0xFE, true);
	return false;
}

void EfhEngine::displayCombatMenu(int16 charId) {
	debugC(6, kDebugEngine, "displayCombatMenu %d", charId);

	Common::String buffer = Common::String::format("%s:", _npcBuf[charId]._name);
	setTextColorWhite();
	setTextPos(144, 7);
	displayStringAtTextPos(buffer);
	setTextPos(152, 79);
	displayStringAtTextPos("A");
	setTextColorRed();
	displayStringAtTextPos("ttack");
	setTextPos(195, 79);
	setTextColorWhite();
	displayStringAtTextPos("H");
	setTextColorRed();
	displayStringAtTextPos("ide");
	setTextPos(152, 88);
	setTextColorWhite();
	displayStringAtTextPos("D");
	setTextColorRed();
	displayStringAtTextPos("efend");
	setTextPos(195, 88);
	setTextColorWhite();
	displayStringAtTextPos("R");
	setTextColorRed();
	displayStringAtTextPos("un");
	setTextPos(152, 97);
	setTextColorWhite();
	displayStringAtTextPos("S");
	setTextColorRed();
	displayStringAtTextPos("tatus");
}

void EfhEngine::displayMenuItemString(int16 menuBoxId, int16 thisBoxId, int16 minX, int16 maxX, int16 minY, const char *str) {
	debugC(6, kDebugEngine, "displayMenuItemString %d %d %d->%d %d %s", menuBoxId, thisBoxId, minX, maxX, minY, str);

	if (menuBoxId == thisBoxId) {
		if (_menuDepth == 0)
			setTextColorWhite();
		else
			setTextColorGrey();

		Common::String buffer = Common::String::format("> %s <", str);
		displayCenteredString(buffer, minX, maxX, minY);
		setTextColorRed();
	} else {
		if (_menuDepth == 0)
			setTextColorRed();
		else
			setTextColorGrey();

		displayCenteredString(str, minX, maxX, minY);
	}
}

void EfhEngine::displayStatusMenu(int16 windowId) {
	debugC(3, kDebugEngine, "displayStatusMenu %d", windowId);

	for (uint counter = 0; counter < 9; ++counter) {
		drawColoredRect(80, 39 + 14 * counter, 134, 47 + 14 * counter, 0);
	}

	if (_menuDepth != 0)
		setTextColorGrey();

	displayMenuItemString(windowId, 0, 80, 134, 39, "EQUIP");
	displayMenuItemString(windowId, 1, 80, 134, 53, "USE");
	displayMenuItemString(windowId, 2, 80, 134, 67, "GIVE");
	displayMenuItemString(windowId, 3, 80, 134, 81, "TRADE");
	displayMenuItemString(windowId, 4, 80, 134, 95, "DROP");
	displayMenuItemString(windowId, 5, 80, 134, 109, "INFO.");
	displayMenuItemString(windowId, 6, 80, 134, 123, "PASSIVE");
	displayMenuItemString(windowId, 7, 80, 134, 137, "ACTIVE");
	displayMenuItemString(windowId, 8, 80, 134, 151, "LEAVE");

	setTextColorRed();
}

void EfhEngine::prepareStatusRightWindowIndexes(int16 menuId, int16 charId) {
	debugC(6, kDebugEngine, "prepareStatusRightWindowIndexes %d %d", menuId, charId);

	int16 maxId = 0;
	int16 minId;
	_menuItemCounter = 0;

	switch (menuId) {
	case 5:
		minId = 26;
		maxId = 36;
		break;
	case 6:
		minId = 15;
		maxId = 25;
		break;
	case 7:
		minId = 0;
		maxId = 14;
		break;
	default:
		minId = -1;
		break;
	}

	if (minId == -1) {
		for (uint counter = 0; counter < 10; ++counter) {
			if (_npcBuf[charId]._inventory[counter]._ref != 0x7FFF) {
				_menuStatItemArr[_menuItemCounter++] = counter;
			}
		}
	} else {
		for (int16 counter = minId; counter < maxId; ++counter) {
			if (_npcBuf[charId]._activeScore[counter] != 0) {
				_menuStatItemArr[_menuItemCounter++] = counter;
			}
		}
	}
}

void EfhEngine::displayCharacterSummary(int16 curMenuLine, int16 npcId) {
	debugC(3, kDebugEngine, "displayCharacterSummary %d %d", curMenuLine, npcId);

	setTextColorRed();
	Common::String buffer1 = _npcBuf[npcId]._name;
	setTextPos(146, 27);
	displayStringAtTextPos("Name: ");
	displayStringAtTextPos(buffer1);
	buffer1 = Common::String::format("Level: %d", getXPLevel(_npcBuf[npcId]._xp));
	setTextPos(146, 36);
	displayStringAtTextPos(buffer1);
	buffer1 = Common::String::format("XP: %lu", _npcBuf[npcId]._xp);
	setTextPos(227, 36);
	displayStringAtTextPos(buffer1);
	buffer1 = Common::String::format("Speed: %d", _npcBuf[npcId]._speed);
	setTextPos(146, 45);
	displayStringAtTextPos(buffer1);
	buffer1 = Common::String::format("Defense: %d", getEquipmentDefense(npcId, false));
	setTextPos(146, 54);
	displayStringAtTextPos(buffer1);
	buffer1 = Common::String::format("Hit Points: %d", _npcBuf[npcId]._hitPoints);
	setTextPos(146, 63);
	displayStringAtTextPos(buffer1);
	buffer1 = Common::String::format("Max HP: %d", _npcBuf[npcId]._maxHP);
	setTextPos(227, 63);
	displayStringAtTextPos(buffer1);
	displayCenteredString("Inventory", 144, 310, 72);

	if (_menuItemCounter == 0) {
		if (curMenuLine != -1)
			setTextColorWhite();

		displayCenteredString("Nothing Carried", 144, 310, 117);
		setTextColorRed();
		return;
	}

	for (int counter = 0; counter < _menuItemCounter; ++counter) {
		if (_menuDepth == 0)
			setTextColorGrey();
		else {
			if (counter == curMenuLine)
				setTextColorWhite();
		}
		int16 textPosY = 81 + counter * 9;
		int16 itemId = _npcBuf[npcId]._inventory[_menuStatItemArr[counter]]._ref;
		if (itemId != 0x7FFF) {
			if (_npcBuf[npcId]._inventory[_menuStatItemArr[counter]]._stat1 & 0x80) {
				setTextPos(146, textPosY);
				displayCharAtTextPos('E');
			}
		}

		setTextPos(152, textPosY);
		if (counter == curMenuLine) {
			buffer1 = Common::String::format("%c>", 'A' + counter);
		} else {
			buffer1 = Common::String::format("%c)", 'A' + counter);
		}
		displayStringAtTextPos(buffer1);

		if (itemId != 0x7FFF) {
			setTextPos(168, textPosY);
			buffer1 = Common::String::format("  %s", _items[itemId]._name);
			displayStringAtTextPos(buffer1);
			setTextPos(262, textPosY);

			if (_items[itemId]._defense > 0) {
				int16 stat2 = _npcBuf[npcId]._inventory[_menuStatItemArr[counter]]._stat2;
				if (stat2 != 0xFF) {
					buffer1 = Common::String::format("%d", 1 + stat2 / 8);
					displayStringAtTextPos(buffer1);
					setTextPos(286, textPosY);
					displayStringAtTextPos("Def");
				}
				// useless code removed.
				// else {
				//	var54 = _items[_npcBuf[npcId]._inventory[_menuStatItemArr[counter]]._ref]._defense;
				// {
			} else if (_items[itemId]._uses != 0x7F) {
				int16 stat1 = _npcBuf[npcId]._inventory[_menuStatItemArr[counter]]._stat1;
				if (stat1 != 0x7F) {
					buffer1 = Common::String::format("%d", stat1);
					displayStringAtTextPos(buffer1);
					setTextPos(286, textPosY);
					if (stat1 == 1)
						displayStringAtTextPos("Use");
					else
						displayStringAtTextPos("Uses");
				}
			}
		}
		setTextColorRed();
	}
}

void EfhEngine::displayCharacterInformationOrSkills(int16 curMenuLine, int16 charId) {
	debugC(3, kDebugEngine, "displayCharacterInformationOrSkills %d %d", curMenuLine, charId);

	setTextColorRed();
	Common::String buffer = _npcBuf[charId]._name;
	setTextPos(146, 27);
	displayStringAtTextPos("Name: ");
	displayStringAtTextPos(buffer);
	if (_menuItemCounter <= 0) {
		if (curMenuLine != -1)
			setTextColorWhite();
		displayCenteredString("No Skills To Select", 144, 310, 96);
		setTextColorRed();
		return;
	}

	for (int counter = 0; counter < _menuItemCounter; ++counter) {
		if (counter == curMenuLine)
			setTextColorWhite();
		int16 textPosY = 38 + counter * 9;
		setTextPos(146, textPosY);
		if (counter == curMenuLine) {
			buffer = Common::String::format("%c>", 'A' + counter);
		} else {
			buffer = Common::String::format("%c)", 'A' + counter);
		}

		displayStringAtTextPos(buffer);
		setTextPos(163, textPosY);
		displayStringAtTextPos(kSkillArray[_menuStatItemArr[counter]]);
		buffer = Common::String::format("%d", _npcBuf[charId]._activeScore[_menuStatItemArr[counter]]);
		setTextPos(278, textPosY);
		displayStringAtTextPos(buffer);
		setTextColorRed();
	}
}

void EfhEngine::displayStatusMenuActions(int16 menuId, int16 curMenuLine, int16 npcId) {
	debugC(6, kDebugEngine, "displayStatusMenuActions %d %d %d", menuId, curMenuLine, npcId);

	drawColoredRect(144, 15, 310, 184, 0);
	displayCenteredString("(ESCape Aborts)", 144, 310, 175);
	_textColor = 0x0E;
	switch (menuId) {
	case 0:
		displayCenteredString("Select Item to Equip", 144, 310, 15);
		displayCharacterSummary(curMenuLine, npcId);
		break;
	case 1:
		displayCenteredString("Select Item to Use", 144, 310, 15);
		displayCharacterSummary(curMenuLine, npcId);
		break;
	case 2:
		displayCenteredString("Select Item to Give", 144, 310, 15);
		displayCharacterSummary(curMenuLine, npcId);
		break;
	case 3:
		displayCenteredString("Select Item to Trade", 144, 310, 15);
		displayCharacterSummary(curMenuLine, npcId);
		break;
	case 4:
		displayCenteredString("Select Item to Drop", 144, 310, 15);
		displayCharacterSummary(curMenuLine, npcId);
		break;
	case 5:
		displayCenteredString("Character Information", 144, 310, 15);
		displayCharacterInformationOrSkills(curMenuLine, npcId);
		break;
	case 6:
		displayCenteredString("Passive Skills", 144, 310, 15);
		displayCharacterInformationOrSkills(curMenuLine, npcId);
		break;
	case 7:
		displayCenteredString("Active Skills", 144, 310, 15);
		displayCharacterInformationOrSkills(curMenuLine, npcId);
		break;
	case 8:
	case 9:
		displayCenteredString("Character Summary", 144, 310, 15);
		displayCharacterSummary(curMenuLine, npcId);
		break;
	default:
		break;
	}
}

void EfhEngine::prepareStatusMenu(int16 windowId, int16 menuId, int16 curMenuLine, int16 charId, bool unusedFl, bool refreshFl) {
	debugC(6, kDebugEngine, "prepareStatusMenu %d %d %d %d %s", windowId, menuId, curMenuLine, charId, refreshFl ? "True" : "False");

	displayStatusMenu(windowId);

	prepareStatusRightWindowIndexes(menuId, charId);
	displayStatusMenuActions(menuId, curMenuLine, charId);

	if (refreshFl)
		displayFctFullScreen();
}

void EfhEngine::sub18E80(int16 charId, int16 windowId, int16 menuId, int16 curMenuLine) {
	debug("sub18E80 %d %d %d %d", charId, windowId, menuId, curMenuLine);

	for (int counter = 0; counter < 2; ++counter) {
		displayWindow(_menuBuf, 0, 0, _hiResImageBuf);
		prepareStatusMenu(windowId, menuId, curMenuLine, charId, true, false);

		if (counter == 0)
			displayFctFullScreen();
	}
}

int16 EfhEngine::displayString_3(Common::String str, bool animFl, int16 charId, int16 windowId, int16 menuId, int16 curMenuLine) {
	debug("displayString_3 %s %s %d %d %d %d", str.c_str(), animFl ? "True" : "False", charId, windowId, menuId, curMenuLine);

	int16 retVal = 0;

	for (uint counter = 0; counter < 2; ++counter) {
		prepareStatusMenu(windowId, menuId, curMenuLine, charId, true, false);
		displayWindow(_windowWithBorderBuf, 19, 113, _hiResImageBuf);

		if (counter == 0) {
			script_parse(str, 28, 122, 105, 166, false);
			displayFctFullScreen();
		} else {
			retVal = script_parse(str, 28, 122, 105, 166, true);
		}
	}

	if (animFl) {
		getLastCharAfterAnimCount(_guessAnimationAmount);
		sub18E80(charId, windowId, menuId, curMenuLine);
	}

	return retVal;
}

int16 EfhEngine::handleStatusMenu(int16 gameMode, int16 charId) {
	debug("handleStatusMenu %d %d", gameMode, charId);

	int16 menuId = 9;
	int16 selectedLine = -1;
	int16 windowId = -1;
	int16 curMenuLine = -1;
	bool var10 = false;
	bool var2 = false;

	saveAnimImageSetId();

	_statusMenuActive = true;
	_menuDepth = 0;

	sub18E80(charId, windowId, menuId, curMenuLine);

	for (;;) {
		if (windowId != -1)
			prepareStatusMenu(windowId, menuId, curMenuLine, charId, true, true);
		else
			windowId = 0;

		do {
			Common::KeyCode var19 = handleAndMapInput(false);
			if (_menuDepth == 0) {
				switch (var19) {
				case Common::KEYCODE_ESCAPE:
					windowId = 8;
					var19 = Common::KEYCODE_RETURN;
					break;
				case Common::KEYCODE_a:
					windowId = 7;
					var19 = Common::KEYCODE_RETURN;
					break;
				case Common::KEYCODE_d:
					windowId = 4;
					var19 = Common::KEYCODE_RETURN;
					break;
				case Common::KEYCODE_e:
					windowId = 0;
					var19 = Common::KEYCODE_RETURN;
					break;
				case Common::KEYCODE_g:
					windowId = 2;
					var19 = Common::KEYCODE_RETURN;
					break;
				case Common::KEYCODE_i:
					windowId = 5;
					var19 = Common::KEYCODE_RETURN;
					break;
				case Common::KEYCODE_l:
					windowId = 8;
					var19 = Common::KEYCODE_RETURN;
					break;
				case Common::KEYCODE_p:
					windowId = 6;
					var19 = Common::KEYCODE_RETURN;
					break;
				case Common::KEYCODE_t:
					windowId = 3;
					var19 = Common::KEYCODE_RETURN;
					break;
				case Common::KEYCODE_u:
					windowId = 1;
					var19 = Common::KEYCODE_RETURN;
					break;
				// case 0xFB: Joystick button 2
				default:
					//	warning("handleStatusMenu - unhandled keys (or joystick event?) 0xBA, 0xBB, 0xBC");
					break;
				}
			} else if (_menuDepth == 1) {
				// in the sub-menus, only a list of selectable items is displayed
				if (var19 >= Common::KEYCODE_a && var19 <= Common::KEYCODE_z) {
					int16 var8 = var19 - Common::KEYCODE_a;
					if (var8 < _menuItemCounter) {
						curMenuLine = var8;
						var19 = Common::KEYCODE_RETURN;
					}
				}
			}

			switch (var19) {
			case Common::KEYCODE_RETURN:
				// case 0xFA: Joystick button 1
				if (_menuDepth == 0) {
					menuId = windowId;
					if (menuId > 7)
						var10 = true;
					else {
						_menuDepth = 1;
						curMenuLine = 0;
					}
				} else if (_menuDepth == 1) {
					if (_menuItemCounter == 0) {
						_menuDepth = 0;
						curMenuLine = -1;
						menuId = 9;
						prepareStatusMenu(windowId, menuId, curMenuLine, charId, true, true);
					} else {
						selectedLine = curMenuLine;
						var10 = true;
					}
				}
				break;
			case Common::KEYCODE_ESCAPE:
				_menuDepth = 0;
				curMenuLine = -1;
				menuId = 9;
				prepareStatusMenu(windowId, menuId, curMenuLine, charId, true, true);
				break;
			case Common::KEYCODE_2:
			case Common::KEYCODE_6:
			// Added for ScummVM
			case Common::KEYCODE_DOWN:
			case Common::KEYCODE_RIGHT:
			case Common::KEYCODE_KP2:
			case Common::KEYCODE_KP6:
				// Original checks joystick axis: case 0xCC, 0xCF
				if (_menuDepth == 0) {
					if (++windowId > 8)
						windowId = 0;
				} else if (_menuDepth == 1) {
					if (_menuItemCounter != 0) {
						++curMenuLine;
						if (curMenuLine > _menuItemCounter - 1)
							curMenuLine = 0;
					}
				}
				break;
			case Common::KEYCODE_4:
			case Common::KEYCODE_8:
			// Added for ScummVM
			case Common::KEYCODE_LEFT:
			case Common::KEYCODE_UP:
			case Common::KEYCODE_KP4:
			case Common::KEYCODE_KP8:
				// Original checks joystick axis: case 0xC7, 0xCA
				if (_menuDepth == 0) {
					if (--windowId < 0)
						windowId = 8;
				} else if (_menuDepth == 1) {
					if (_menuItemCounter != 0) {
						--curMenuLine;
						if (curMenuLine < 0)
							curMenuLine = _menuItemCounter - 1;
					}
				}
				break;
			default:
				break;
			}

			if (curMenuLine == -1)
				prepareStatusMenu(windowId, menuId, curMenuLine, charId, false, true);
			else
				prepareStatusMenu(windowId, menuId, curMenuLine, charId, true, true);

		} while (!var10);

		bool validationFl = true;

		int16 objectId;
		int16 itemId;
		switch (menuId) {
		case 0:
			objectId = _menuStatItemArr[selectedLine];
			itemId = _npcBuf[charId]._inventory[objectId]._ref; // CHECKME: Useless?
			sub191FF(charId, objectId, windowId, menuId, curMenuLine);
			if (gameMode == 2) {
				restoreAnimImageSetId();
				_statusMenuActive = false;
				return 0x7D00;
			}
			break;
		case 1:
			objectId = _menuStatItemArr[selectedLine];
			itemId = _npcBuf[charId]._inventory[objectId]._ref;
			if (gameMode == 2) {
				restoreAnimImageSetId();
				_statusMenuActive = false;
				return objectId;
			}

			if (sub22293(_mapPosX, _mapPosY, charId, itemId, 2, -1)) {
				_statusMenuActive = false;
				return -1;
			}

			sub19E2E(charId, objectId, windowId, menuId, curMenuLine, 2);
			break;
		case 2:
			objectId = _menuStatItemArr[selectedLine];
			itemId = _npcBuf[charId]._inventory[objectId]._ref;
			if (hasObjectEquipped(charId, objectId) && isItemCursed(itemId)) {
				displayString_3("The item is cursed!  IT IS EVIL!!!!!!!!", true, charId, windowId, menuId, curMenuLine);
			} else if (hasObjectEquipped(charId, objectId)) {
				displayString_3("Item is Equipped!  Give anyway?", false, charId, windowId, menuId, curMenuLine);
				if (!getValidationFromUser())
					validationFl = false;
				sub18E80(charId, windowId, menuId, curMenuLine);

				if (validationFl) {
					if (gameMode == 2) {
						displayString_3("Not a Combat Option !", true, charId, windowId, menuId, curMenuLine);
					} else {
						removeObject(charId, objectId);
						int16 var8 = sub22293(_mapPosX, _mapPosY, charId, itemId, 3, -1);
						if (var8 != 0) {
							_statusMenuActive = false;
							return -1;
						}
					}
				}
			}

			break;
		case 3:
			objectId = _menuStatItemArr[selectedLine];
			itemId = _npcBuf[charId]._inventory[objectId]._ref;
			if (hasObjectEquipped(charId, objectId) && isItemCursed(itemId)) {
				displayString_3("The item is cursed!  IT IS EVIL!!!!!!!!", true, charId, windowId, menuId, curMenuLine);
			} else if (hasObjectEquipped(charId, objectId)) {
				displayString_3("Item is Equipped!  Trade anyway?", false, charId, windowId, menuId, curMenuLine);
				if (!getValidationFromUser())
					validationFl = false;
				sub18E80(charId, windowId, menuId, curMenuLine);

				if (validationFl) {
					bool var6;
					int16 var8;
					do {
						if (_teamCharId[2] != -1) {
							var8 = displayString_3("Who will you give the item to?", false, charId, windowId, menuId, curMenuLine);
							var2 = false;
						} else if (_teamCharId[1]) {
							var8 = 0x1A;
							var2 = false;
						} else {
							var2 = true;
							if (_teamCharId[0] == charId)
								var8 = 1;
							else
								var8 = 0;
						}

						if (var8 != 0x1A && var8 != 0x1B) {
							var6 = giveItemTo(_teamCharId[var8], objectId, charId);
							if (!var6) {
								displayString_3("That character cannot carry anymore!", false, charId, windowId, menuId, curMenuLine);
								getLastCharAfterAnimCount(_guessAnimationAmount);
							}
						} else {
							if (var8 == 0x1A) {
								displayString_3("No one to trade with!", false, charId, windowId, menuId, curMenuLine);
								getLastCharAfterAnimCount(_guessAnimationAmount);
								var8 = 0x1B;
							}
							var6 = false;
						}
					} while (!var6 && !var2 && var8 != 0x1B);

					if (var6) {
						removeObject(charId, objectId);
						if (gameMode == 2) {
							restoreAnimImageSetId();
							_statusMenuActive = false;
							return 0x7D00;
						}
					}

					sub18E80(charId, windowId, menuId, curMenuLine);
				}
			}
			break;
		case 4:
			objectId = _menuStatItemArr[selectedLine];
			itemId = _npcBuf[charId]._inventory[objectId]._ref;
			if (hasObjectEquipped(charId, objectId) && isItemCursed(itemId)) {
				displayString_3("The item is cursed!  IT IS EVIL!!!!!!!!", true, charId, windowId, menuId, curMenuLine);
			} else if (hasObjectEquipped(charId, objectId)) {
				displayString_3("Item Is Equipped!  Drop Anyway?", false, charId, windowId, menuId, curMenuLine);
				if (!getValidationFromUser())
					validationFl = false;
				sub18E80(charId, windowId, menuId, curMenuLine);

				if (validationFl) {
					removeObject(charId, objectId);
					if (gameMode == 2) {
						restoreAnimImageSetId();
						_statusMenuActive = false;
						return 0x7D00;
					}

					bool var8 = sub22293(_mapPosX, _mapPosY, charId, itemId, 1, -1);
					if (var8) {
						_statusMenuActive = false;
						return -1;
					}
				}
			}
			break;
		case 5:
			objectId = _menuStatItemArr[selectedLine];
			if (gameMode == 2) {
				displayString_3("Not a Combat Option!", true, charId, windowId, menuId, curMenuLine);
			} else {
				bool var8 = sub22293(_mapPosX, _mapPosY, charId, objectId, 4, -1);
				if (var8) {
					_statusMenuActive = false;
					return -1;
				}
			}
			break;
		case 6: // Identical to case 5?
			objectId = _menuStatItemArr[selectedLine];
			if (gameMode == 2) {
				displayString_3("Not a Combat Option!", true, charId, windowId, menuId, curMenuLine);
			} else {
				bool var8 = sub22293(_mapPosX, _mapPosY, charId, objectId, 4, -1);
				if (var8) {
					_statusMenuActive = false;
					return -1;
				}
			}
			break;
		case 7: // Identical to case 5?
			objectId = _menuStatItemArr[selectedLine];
			if (gameMode == 2) {
				displayString_3("Not a Combat Option!", true, charId, windowId, menuId, curMenuLine);
			} else {
				bool var8 = sub22293(_mapPosX, _mapPosY, charId, objectId, 4, -1);
				if (var8) {
					_statusMenuActive = false;
					return -1;
				}
			}
			break;
		default:
			break;
		}

		if (menuId != 8) {
			var10 = false;
			_menuDepth = 0;
			menuId = 9;
			selectedLine = -1;
			curMenuLine = -1;
		}

		if (menuId == 8) {
			restoreAnimImageSetId();
			_statusMenuActive = false;
			return 0x7FFF;
		}
	}

	return 0;
}

void EfhEngine::equipCursedItem(int16 charId, int16 objectId, int16 windowId, int16 menuId, int16 curMenuLine) {
	debug("equipCursedItem %d %d %d %d %d", charId, objectId, windowId, menuId, curMenuLine);

	int16 itemId = _npcBuf[charId]._inventory[objectId]._ref;

	if (isItemCursed(itemId)) {
		_npcBuf[charId]._inventory[objectId]._stat1 &= 0x7F;
	} else {
		displayString_3("Cursed Item Already Equipped!", true, charId, windowId, menuId, curMenuLine);
	}
}

void EfhEngine::sub191FF(int16 charId, int16 objectId, int16 windowId, int16 menuId, int16 curMenuLine) {
	debug("sub191FF %d %d %d %d %d", charId, objectId, windowId, menuId, curMenuLine);

	int16 itemId = _npcBuf[charId]._inventory[objectId]._ref;

	if (hasObjectEquipped(charId, objectId)) {
		equipCursedItem(charId, objectId, windowId, menuId, curMenuLine);
	} else {
		int16 var2 = _items[itemId].field_18;
		if (var2 != 4) {
			for (uint counter = 0; counter < 10; ++counter) {
				if (var2 == _items[_npcBuf[charId]._inventory[counter]._ref].field_18)
					equipCursedItem(charId, objectId, windowId, menuId, curMenuLine);
			}
		}

		_npcBuf[charId]._inventory[objectId]._stat1 |= 0x80;
	}
}

int16 EfhEngine::sub19E2E(int16 charId, int16 objectId, int16 windowId, int16 menuId, int16 curMenuLine, int16 argA) {
	debug("sub19E2E %d %d %d %d %d %d", charId, objectId, windowId, menuId, curMenuLine, argA);

	Common::String buffer1 = "";

	bool varA6 = false;
	bool retVal = false;

	int16 itemId = _npcBuf[charId]._inventory[objectId]._ref;
	switch (_items[itemId].field_16 - 1) {
	case 0: // "Demonic Powers", "MindDomination", "Guilt Trip", "Sleep Grenade", "SleepGrenader"
		if (argA == 2) {
			displayString_3("The item emits a low droning hum...", false, charId, windowId, menuId, curMenuLine);
		} else {
			int16 victims = 0;
			_messageToBePrinted += "  The item emits a low droning hum...";
			if (getRandom(100) < 50) {
				for (uint counter = 0; counter < 9; ++counter) {
					if (isMonsterActive(windowId, counter)) {
						++victims;
						_stru32686[windowId]._field0[counter] = 1;
						_stru32686[windowId]._field2[counter] = getRandom(8);
					}
				}
			} else {
				int16 NumberOfTargets = getRandom(9);
				for (uint counter = 0; counter < 9; ++counter) {
					if (NumberOfTargets == 0)
						break;

					if (isMonsterActive(windowId, counter)) {
						++victims;
						--NumberOfTargets;
						_stru32686[windowId]._field0[counter] = 1;
						_stru32686[windowId]._field2[counter] = getRandom(8);
					}
				}
			}
			// The original was duplicating this code in each branch of the previous random check.
			if (victims > 1) {
				buffer1 = Common::String::format("%d %ss fall asleep!", victims, kEncounters[_mapMonsters[_teamMonsterIdArray[windowId]]._monsterRef]._name);
			} else {
				buffer1 = Common::String::format("%d %s falls asleep!", victims, kEncounters[_mapMonsters[_teamMonsterIdArray[windowId]]._monsterRef]._name);
			}
			_messageToBePrinted += buffer1;
		}

		varA6 = true;
		break;
	case 1: // "Chilling Touch", "Guilt", "Petrify Rod", "Elmer's Gun"
		if (argA == 2) {
			displayString_3("The item grows very cold for a moment...", false, charId, windowId, menuId, curMenuLine);
		} else {
			_messageToBePrinted += "  The item emits a blue beam...";
			int16 victim = 0;
			if (getRandom(100) < 50) {
				for (uint varA8 = 0; varA8 < 9; ++varA8) {
					if (isMonsterActive(windowId, varA8)) {
						++victim;
						_stru32686[windowId]._field0[varA8] = 2;
						_stru32686[windowId]._field2[varA8] = getRandom(8);
					}
				}
			} else {
				int16 varAC = getRandom(9);
				for (uint varA8 = 0; varA8 < 9; ++varA8) {
					if (varAC == 0)
						break;

					if (isMonsterActive(windowId, varA8)) {
						++victim;
						--varAC;
						_stru32686[windowId]._field0[varA8] = 2;
						_stru32686[windowId]._field2[varA8] = getRandom(8);
					}
				}
			}
			// <CHECKME>: This part is only present in the original in the case < 50, but for me
			// it's missing in the other case as there's an effect (frozen enemies) but no feedback to the player
			if (victim > 1) {
				buffer1 = Common::String::format("%d %ss are frozen in place!", victim, kEncounters[_mapMonsters[_teamMonsterIdArray[windowId]]._monsterRef]._name);
			} else {
				buffer1 = Common::String::format("%d %s is frozen in place!", victim, kEncounters[_mapMonsters[_teamMonsterIdArray[windowId]]._monsterRef]._name);
			}
			_messageToBePrinted += buffer1;
			// </CHECKME>
		}

		varA6 = true;
		break;
	case 2:
		if (argA == 2) {
			displayString_3("A serene feeling passes through the air...", false, charId, windowId, menuId, curMenuLine);
		} else {
			_messageToBePrinted += "  The combat pauses...as there is a moment of forgiveness...";
			_unk2C8AA = 0;
		}

		varA6 = true;
		break;
	case 4: // "Unholy Sinwave", "Holy Water"
		if (argA == 2) {
			displayString_3("A dark sense fills your soul...then fades!", false, charId, windowId, menuId, curMenuLine);
		} else {
			_messageToBePrinted += "  A dark gray fiery whirlwind surrounds the poor victim...the power fades and death abounds!";
			if (getRandom(100) < 50) {
				for (uint counter = 0; counter < 9; ++counter) {
					if (getRandom(100) < 50) {
						_mapMonsters[_teamMonsterIdArray[windowId]]._pictureRef[counter] = 0;
					}
				}
			} else {
				for (uint counter = 0; counter < 9; ++counter) {
					if (isMonsterActive(windowId, counter)) {
						if (getRandom(100) < 50) {
							_mapMonsters[_teamMonsterIdArray[windowId]]._pictureRef[counter] = 0;
						}
						break;
					}
				}
			}
		}
		varA6 = true;
		break;
	case 5: // "Lucifer'sTouch", "Book of Death", "Holy Cross"
		if (argA == 2) {
			displayString_3("A dark sense fills your soul...then fades!", false, charId, windowId, menuId, curMenuLine);
		} else {
			if (getRandom(100) < 50) {
				_messageToBePrinted += "  A dark fiery whirlwind surrounds the poor victim...the power fades and all targeted die!";
				for (uint counter = 0; counter < 9; ++counter) {
					_mapMonsters[_teamMonsterIdArray[windowId]]._pictureRef[counter] = 0;
				}
			} else {
				_messageToBePrinted += "  A dark fiery whirlwind surrounds the poor victim...the power fades and one victim dies!";
				for (uint counter = 0; counter < 9; ++counter) {
					if (isMonsterActive(windowId, counter)) {
						_mapMonsters[_teamMonsterIdArray[windowId]]._pictureRef[counter] = 0;
					}
				}
			}
		}

		varA6 = true;
		break;
	case 12: // "Terror Gaze", "Servitude Rod", "Despair Ankh", "ConfusionPrism", "Pipe of Peace", "Red Cape", "Peace Symbol", "Hell Badge"
		if (argA == 2) {
			displayString_3("There is no apparent affect!", false, charId, windowId, menuId, curMenuLine);
		} else {
			_messageToBePrinted += "  The magic sparkles brilliant hues in the air!";
			setMapMonsterField8(windowId, _items[itemId].field17_attackTypeDefense, true);
		}
		varA6 = true;
		break;
	case 14: { // "Feathered Cap"
		int16 varAA;
		if (argA == 2) {
			displayString_3("Who will use the item?", false, charId, windowId, menuId, curMenuLine);
			varAA = selectOtherCharFromTeam();
		} else {
			varAA = windowId;
		}

		if (varAA != 0x1B) {
			buffer1 = "  The magic makes the user as quick and agile as a bird!";
			if (argA == 2) {
				displayString_3(buffer1, false, charId, windowId, menuId, curMenuLine);
			} else {
				_messageToBePrinted += buffer1;
			}
			_word32482[varAA] -= 50;
			if (_word32482[varAA] < 0)
				_word32482[varAA] = 0;
		}

		varA6 = true;
	} break;
	case 15: { // "Regal Crown"
		int16 teamCharId;
		if (argA == 2) {
			displayString_3("Who will use the item?", false, charId, windowId, menuId, curMenuLine);
			teamCharId = selectOtherCharFromTeam();
		} else {
			teamCharId = windowId;
		}

		if (teamCharId != 0x1B) {
			buffer1 = "  The magic makes the user invisible!";
			if (argA == 2) {
				displayString_3(buffer1, false, charId, windowId, menuId, curMenuLine);
			} else {
				_messageToBePrinted += buffer1;
			}

			_teamPctVisible[teamCharId] -= 50;
			if (_teamPctVisible[teamCharId] < 0)
				_teamPctVisible[teamCharId] = 0;
		}

		varA6 = true;
	} break;
	case 16: { // Fairy Dust
		_mapPosX = getRandom(_largeMapFlag ? 63 : 23);
		_mapPosY = getRandom(_largeMapFlag ? 63 : 23);
		int16 tileFactId = getTileFactId(_mapPosX, _mapPosY);

		if (_tileFact[tileFactId]._field0 == 0) {
			totalPartyKill();
			buffer1 = "The entire party vanishes in a flash... only to appear in stone !";
			if (argA == 2) {
				displayString_3(buffer1, false, charId, windowId, menuId, curMenuLine);
			} else {
				_messageToBePrinted += buffer1;
				retVal = true;
			}
		} else {
			if (tileFactId == 0 || tileFactId == 0x48) {
				buffer1 = "The entire party vanishes in a flash...but re-appears, as if nothing happened!";
				if (argA == 2) {
					displayString_3(buffer1, false, charId, windowId, menuId, curMenuLine);
				} else {
					_messageToBePrinted += buffer1;
					retVal = true;
				}
			} else {
				buffer1 = "The entire party vanishes in a flash...only to appear elsewhere!";
				if (argA == 2) {
					displayString_3(buffer1, false, charId, windowId, menuId, curMenuLine);
				} else {
					_messageToBePrinted += buffer1;
					retVal = true;
				}
			}
		}

		varA6 = true;
	} break;
	case 17: { // "Devil Dust"
		_mapPosX = _items[itemId].field_19;
		_mapPosY = _items[itemId].field_1A;
		int16 tileFactId = getTileFactId(_mapPosX, _mapPosY);
		if (_tileFact[tileFactId]._field0 == 0) {
			totalPartyKill();
			buffer1 = "The entire party vanishes in a flash... only to appear in stone !";
			if (argA == 2) {
				displayString_3(buffer1, false, charId, windowId, menuId, curMenuLine);
			} else {
				_messageToBePrinted += buffer1;
				retVal = true;
			}
		} else {
			if (tileFactId == 0 || tileFactId == 0x48) {
				buffer1 = "The entire party vanishes in a flash...but re-appears, as if nothing happened!";
				if (argA == 2) {
					displayString_3(buffer1, false, charId, windowId, menuId, curMenuLine);
				} else {
					_messageToBePrinted += buffer1;
					retVal = true;
				}
			} else {
				buffer1 = "The entire party vanishes in a flash...only to appear elsewhere!";
				if (argA == 2) {
					displayString_3(buffer1, false, charId, windowId, menuId, curMenuLine);
				} else {
					_messageToBePrinted += buffer1;
					retVal = true;
				}
			}
		}

		varA6 = true;
	} break;
	case 18:
		if (argA == 2) {
			displayString_3("The item makes a loud noise!", false, charId, windowId, menuId, curMenuLine);
		} else {
			int16 teamCharId = windowId;
			if (teamCharId != 0x1B) {
				if (_teamCharStatus[teamCharId]._status == 2) { // frozen
					_messageToBePrinted += "  The item makes a loud noise, awakening the character!";
					_teamCharStatus[teamCharId]._status = 0;
					_teamCharStatus[teamCharId]._duration = 0;
				} else {
					_messageToBePrinted += "  The item makes a loud noise, but has no effect!";
				}
			}
		}

		varA6 = true;
		break;
	case 19: // "Junk"
		buffer1 = "  * The item breaks!";
		if (argA == 2) {
			displayString_3(buffer1, false, charId, windowId, menuId, curMenuLine);
		} else {
			_messageToBePrinted += buffer1;
		}
		setCharacterObjectToBroken(charId, objectId);
		varA6 = true;
		break;
	case 23: // "Divining Rod"
		buffer1 = Common::String::format("The %s says, '", _items[itemId]._name);
		if (_items[itemId].field_19 < _mapPosX) {
			if (_items[itemId].field_1A < _mapPosY) {
				buffer1 += "North West!";
			} else if (_items[itemId].field_1A > _mapPosY) {
				buffer1 += "South West!";
			} else {
				buffer1 += "West!";
			}
		} else if (_items[itemId].field_19 > _mapPosX) {
			if (_items[itemId].field_1A < _mapPosY) {
				buffer1 += "North East!";
			} else if (_items[itemId].field_1A > _mapPosY) {
				buffer1 += "South East!";
			} else {
				buffer1 += "East!";
			}
		} else { // equals _mapPosX
			if (_items[itemId].field_1A < _mapPosY) {
				buffer1 += "North!";
			} else if (_items[itemId].field_1A > _mapPosY) {
				buffer1 += "South!";
			} else {
				buffer1 += "Here!!!";
			}
		}
		buffer1 += "'";
		if (argA == 2) {
			displayString_3(buffer1, false, charId, windowId, menuId, curMenuLine);
		} else {
			_messageToBePrinted += buffer1;
			retVal = true;
		}

		varA6 = true;
		break;
	case 24: {
		int16 teamCharId;
		if (argA == 2) {
			displayString_3("Who will use this item?", false, charId, windowId, menuId, curMenuLine);
			teamCharId = selectOtherCharFromTeam();
		} else
			teamCharId = windowId;

		if (teamCharId != 0x1B) {
			uint8 varAE = _items[itemId].field17_attackTypeDefense;
			uint8 effectPoints = getRandom(_items[itemId].field_19);
			_npcBuf[_teamCharId[teamCharId]]._activeScore[varAE] += effectPoints;
			if (_npcBuf[_teamCharId[teamCharId]]._activeScore[varAE] > 20) {
				_npcBuf[_teamCharId[teamCharId]]._activeScore[varAE] = 20;
			}
			if (effectPoints > 1)
				buffer1 = Common::String::format("%s increased %d points!", kSkillArray[varAE], effectPoints);
			else
				buffer1 = Common::String::format("%s increased 1 point!", kSkillArray[varAE]);

			if (argA == 2) {
				displayString_3(buffer1, false, charId, windowId, menuId, curMenuLine);
			} else {
				_messageToBePrinted += buffer1;
				retVal = true;
			}
		}

		varA6 = true;
	} break;
	case 25: {
		int16 teamCharId;
		if (argA == 2) {
			displayString_3("Who will use this item?", false, charId, windowId, menuId, curMenuLine);
			teamCharId = selectOtherCharFromTeam();
		} else
			teamCharId = windowId;

		if (teamCharId != 0x1B) {
			uint8 varAE = _items[itemId].field17_attackTypeDefense;
			uint8 effectPoints = getRandom(_items[itemId].field_19);
			_npcBuf[_teamCharId[teamCharId]]._activeScore[varAE] -= effectPoints;
			if (_npcBuf[_teamCharId[teamCharId]]._activeScore[varAE] > 20 || _npcBuf[_teamCharId[teamCharId]]._activeScore[varAE] < 0) {
				_npcBuf[_teamCharId[teamCharId]]._activeScore[varAE] = 1;
			}
			if (effectPoints > 1)
				buffer1 = Common::String::format("%s lowered %d points!", kSkillArray[varAE], effectPoints);
			else
				buffer1 = Common::String::format("%s lowered 1 point!", kSkillArray[varAE]);

			if (argA == 2) {
				displayString_3(buffer1, false, charId, windowId, menuId, curMenuLine);
			} else {
				_messageToBePrinted += buffer1;
				retVal = true;
			}
		}

		varA6 = true;
	} break;
	case 26: // "Black Sphere"
		buffer1 = "The entire party collapses, dead!!!";
		if (argA == 2) {
			displayString_3(buffer1, false, charId, windowId, menuId, curMenuLine);
		} else {
			_messageToBePrinted += buffer1;
			retVal = true;
		}
		totalPartyKill();
		varA6 = true;
		break;
	case 27: { // "Magic Pyramid", "Razor Blade"
		int16 teamCharId;
		if (argA == 2) {
			displayString_3("Who will use the item?", false, charId, windowId, menuId, curMenuLine);
			teamCharId = selectOtherCharFromTeam();
		} else {
			teamCharId = windowId;
		}

		if (teamCharId != 0x1B) {
			_npcBuf[_teamCharId[teamCharId]]._hitPoints = 0;
			buffer1 = Common::String::format("%s collapses, dead!!!", _npcBuf[_teamCharId[teamCharId]]._name);
			if (argA == 2) {
				displayString_3(buffer1, false, charId, windowId, menuId, curMenuLine);
			} else {
				_messageToBePrinted += buffer1;
				retVal = true;
			}
		}

		varA6 = true;
	} break;
	case 28: // "Bugle"
		if (argA == 2) {
			displayString_3("The item makes a loud noise!", false, charId, windowId, menuId, curMenuLine);
		} else {
			int16 teamCharId = windowId;
			if (teamCharId != 0x1B) {
				if (_teamCharStatus[teamCharId]._status == 0) {
					_messageToBePrinted += "  The item makes a loud noise, awakening the character!";
					_teamCharStatus[teamCharId]._status = 0;
					_teamCharStatus[teamCharId]._duration = 0;
				} else {
					_messageToBePrinted += "  The item makes a loud noise, but has no effect!";
				}
			}
		}

		varA6 = true;
		break;
	case 29: { // "Healing Spray", "Healing Elixir", "Curing Potion", "Magic Potion"
		int16 teamCharId;
		if (argA == 2) {
			displayString_3("Who will use the item?", false, charId, windowId, menuId, curMenuLine);
			teamCharId = selectOtherCharFromTeam();
		} else {
			teamCharId = windowId;
		}

		if (teamCharId != 0x1B) {
			int16 effectPoints = getRandom(_items[itemId].field17_attackTypeDefense);
			_npcBuf[_teamCharId[teamCharId]]._hitPoints += effectPoints;
			if (_npcBuf[_teamCharId[teamCharId]]._hitPoints > _npcBuf[_teamCharId[teamCharId]]._maxHP)
				_npcBuf[_teamCharId[teamCharId]]._hitPoints = _npcBuf[_teamCharId[teamCharId]]._maxHP;

			if (effectPoints > 1)
				buffer1 = Common::String::format("%s is healed %d points!", _npcBuf[_teamCharId[teamCharId]]._name, effectPoints);
			else
				buffer1 = Common::String::format("%s is healed 1 point!", _npcBuf[_teamCharId[teamCharId]]._name);
		}

		if (argA == 2) {
			displayString_3(buffer1, false, charId, windowId, menuId, curMenuLine);
		} else {
			_messageToBePrinted += buffer1;
			retVal = true;
		}

		varA6 = true;
	} break;
	case 30: {
		int16 teamCharId;
		if (argA == 2) {
			displayString_3("Who will use the item?", false, charId, windowId, menuId, curMenuLine);
			teamCharId = selectOtherCharFromTeam();
		} else {
			teamCharId = windowId;
		}

		if (teamCharId != 0x1B) {
			int16 effectPoints = getRandom(_items[itemId].field17_attackTypeDefense);
			_npcBuf[_teamCharId[teamCharId]]._hitPoints -= effectPoints;
			if (_npcBuf[_teamCharId[teamCharId]]._hitPoints < 0)
				_npcBuf[_teamCharId[teamCharId]]._hitPoints = 0;

			if (effectPoints > 1)
				buffer1 = Common::String::format("%s is harmed for %d points!", _npcBuf[_teamCharId[teamCharId]]._name, effectPoints);
			else
				buffer1 = Common::String::format("%s is harmed for 1 point!", _npcBuf[_teamCharId[teamCharId]]._name);
		}

		if (argA == 2) {
			displayString_3(buffer1, false, charId, windowId, menuId, curMenuLine);
		} else {
			_messageToBePrinted += buffer1;
			retVal = true;
		}

		varA6 = true;

	} break;
	case 3:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 13:
	case 20:
	case 21:
	case 22:
	default:
		break;
	}

	if (varA6) {
		if ((_npcBuf[charId]._inventory[objectId]._stat1 & 0x7F) != 0x7F) {
			int8 varA1 = (_npcBuf[charId]._inventory[objectId]._stat1 & 0x7F) - 1;
			if (varA1 <= 0) {
				buffer1 = "  * The item breaks!";
				if (argA == 2) {
					getLastCharAfterAnimCount(_guessAnimationAmount);
					displayString_3(buffer1, false, charId, windowId, menuId, curMenuLine);
				} else {
					_messageToBePrinted += buffer1;
				}
				setCharacterObjectToBroken(charId, objectId);
			} else {
				_npcBuf[charId]._inventory[objectId]._stat1 &= 0x80;
				_npcBuf[charId]._inventory[objectId]._stat1 |= 0xA1;
			}
		}

		if (argA == 2) {
			getLastCharAfterAnimCount(_guessAnimationAmount);
			sub18E80(charId, windowId, menuId, curMenuLine);
		}
	}

	return retVal;
}

} // End of namespace Efh

