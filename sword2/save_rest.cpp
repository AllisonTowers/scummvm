/* Copyright (C) 1994-2004 Revolution Software Ltd
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header$
 */

// ---------------------------------------------------------------------------
// SAVE_REST.CPP	save, restore & restart functions
//
// James 05feb97
//
// "Jesus Saves", but could he Restore or Restart? He can now...
//
// ---------------------------------------------------------------------------

#include "common/stdafx.h"
#include "sword2/sword2.h"
#include "sword2/defs.h"
#include "sword2/interpreter.h"
#include "sword2/logic.h"
#include "sword2/resman.h"

namespace Sword2 {

// A savegame consists of a header and the global variables

// Max length of a savegame filename, including full path
#define	MAX_FILENAME_LEN 128

#ifdef SCUMM_BIG_ENDIAN
// Quick macro to make swapping in-place easier to write
#define SWAP32(x)	x = SWAP_BYTES_32(x)

static void convertHeaderEndian(Sword2Engine::SaveGameHeader &header) {
	// SaveGameHeader
	SWAP32(header.checksum);
	SWAP32(header.varLength);
	SWAP32(header.screenId);
	SWAP32(header.runListId);
	SWAP32(header.feet_x);
	SWAP32(header.feet_y);
	SWAP32(header.music_id);
	
	// ObjectHub
	SWAP32(header.player_hub.type);
	SWAP32(header.player_hub.logic_level);
	for (int i = 0; i < TREE_SIZE; i++) {
		SWAP32(header.player_hub.logic[i]);
		SWAP32(header.player_hub.script_id[i]);
		SWAP32(header.player_hub.script_pc[i]);
	}

	// ObjectLogic
	SWAP32(header.logic.looping);
	SWAP32(header.logic.pause);

	// ObjectGraphic
	SWAP32(header.graphic.type);
	SWAP32(header.graphic.anim_resource);
	SWAP32(header.graphic.anim_pc);

	// ObjectMega
	SWAP32(header.mega.currently_walking);
	SWAP32(header.mega.walk_pc);
	SWAP32(header.mega.scale_a);
	SWAP32(header.mega.scale_b);
	SWAP32(header.mega.feet_x);
	SWAP32(header.mega.feet_y);
	SWAP32(header.mega.current_dir);
	SWAP32(header.mega.megaset_res);
}
#endif

/**
 * Save the game.
 */

uint32 Sword2Engine::saveGame(uint16 slotNo, byte *desc) {
	uint32 bufferSize = findBufferSize();
	byte *saveBufferMem = (byte *) malloc(bufferSize);

	fillSaveBuffer(saveBufferMem, bufferSize, desc);

	uint32 errorCode = saveData(slotNo, saveBufferMem, bufferSize);

	free(saveBufferMem);
	return errorCode;
}

/**
 * Calculate size of required savegame buffer
 */

uint32 Sword2Engine::findBufferSize(void) {
	// Size of savegame header + size of global variables
	return sizeof(_saveGameHeader) + _resman->fetchLen(1);
}

void Sword2Engine::fillSaveBuffer(byte *buffer, uint32 size, byte *desc) {
	// Set up the _saveGameHeader. Checksum gets filled in last of all.

	strcpy(_saveGameHeader.description, (char *) desc);
	_saveGameHeader.varLength = _resman->fetchLen(1);
	_saveGameHeader.screenId = _thisScreen.background_layer_id;
	_saveGameHeader.runListId = _logic->getRunList();
	_saveGameHeader.feet_x = _thisScreen.feet_x;
	_saveGameHeader.feet_y	= _thisScreen.feet_y;
	_saveGameHeader.music_id = _loopingMusicId;

	memcpy(&_saveGameHeader.player_hub, _resman->openResource(CUR_PLAYER_ID) + sizeof(StandardHeader), sizeof(ObjectHub));

	_resman->closeResource(CUR_PLAYER_ID);

	// Get the logic, graphic and mega structures
	getPlayerStructures();

#ifdef SCUMM_BIG_ENDIAN
	convertHeaderEndian(_saveGameHeader);
#endif

	// Copy the header to the buffer, even though it isn't quite complete
	memcpy(buffer, &_saveGameHeader, sizeof(_saveGameHeader));

	// Get the global variables

	byte *varsRes = _resman->openResource(1);
	memcpy(buffer + sizeof(_saveGameHeader), varsRes, FROM_LE_32(_saveGameHeader.varLength));

#ifdef SCUMM_BIG_ENDIAN
	uint32 *globalVars = (uint32 *) (buffer + sizeof(_saveGameHeader) + sizeof(StandardHeader));
	const uint numVars = (FROM_LE_32(_saveGameHeader.varLength) - sizeof(StandardHeader)) / 4;

	for (uint i = 0; i < numVars; i++)
		globalVars[i] = SWAP_BYTES_32(globalVars[i]);
#endif

 	_resman->closeResource(1);

	// Finally, get the checksum

	_saveGameHeader.checksum = TO_LE_32(calcChecksum(buffer + sizeof(_saveGameHeader.checksum), size - sizeof(_saveGameHeader.checksum)));

	// All done

	memcpy(buffer, &_saveGameHeader.checksum, sizeof(_saveGameHeader.checksum));
}

uint32 Sword2Engine::saveData(uint16 slotNo, byte *buffer, uint32 bufferSize) {
	char saveFileName[MAX_FILENAME_LEN];

	sprintf(saveFileName, "%s.%.3d", _targetName, slotNo);

	SaveFileManager *mgr = _system->get_savefile_manager();
	SaveFile *out;

	if (!(out = mgr->open_savefile(saveFileName, getSavePath(), true))) {
		delete mgr;
		return SR_ERR_FILEOPEN;
	}

	uint32 itemsWritten = out->write(buffer, bufferSize);

	delete out;
	delete mgr;

	if (itemsWritten == bufferSize)
		return SR_OK;

	return SR_ERR_WRITEFAIL;
}

/**
 * Restore the game.
 */

uint32 Sword2Engine::restoreGame(uint16 slotNo) {
	uint32 bufferSize = findBufferSize();
	byte *saveBufferMem = (byte *) malloc(bufferSize);

	uint32 errorCode = restoreData(slotNo, saveBufferMem, bufferSize);

	// If it was read in successfully, then restore the game from the
	// buffer & free the buffer. Note that restoreFromBuffer() frees the
	// buffer in order to clear it from memory before loading in the new
	// screen and runlist, so we only need to free it in case of failure.

	if (errorCode == SR_OK)
		errorCode = restoreFromBuffer(saveBufferMem, bufferSize);
	else
		free(saveBufferMem);

	// Force the game engine to pick a cursor. This appears to be needed
	// when using the -x command-line option to restore a game.
	_mouseTouching = 1;
	return errorCode;
}

uint32 Sword2Engine::restoreData(uint16 slotNo, byte *buffer, uint32 bufferSize) {
	char saveFileName[MAX_FILENAME_LEN];

	sprintf(saveFileName, "%s.%.3d", _targetName, slotNo);

	SaveFileManager *mgr = _system->get_savefile_manager();
	SaveFile *in;

	if (!(in = mgr->open_savefile(saveFileName, getSavePath(), false))) {
		// error: couldn't open file
		delete mgr;
		return SR_ERR_FILEOPEN;
	}

	// Read savegame into the buffer
	uint32 itemsRead = in->read(buffer, bufferSize);

	delete in;
	delete mgr;

	if (itemsRead != bufferSize) {
		// We didn't get all of it. At the moment we have no way of
		// knowing why, so assume that it's an incompatible savegame.

		delete in;
		delete mgr;
		return SR_ERR_INCOMPATIBLE;
	}

	return SR_OK;
}

uint32 Sword2Engine::restoreFromBuffer(byte *buffer, uint32 size) {
	// Get a copy of the header from the savegame buffer
	memcpy(&_saveGameHeader, buffer, sizeof(_saveGameHeader));

#ifdef SCUMM_BIG_ENDIAN
	convertHeaderEndian(_saveGameHeader);
#endif

	// Calc checksum & check that aginst the value stored in the header

	if (_saveGameHeader.checksum != calcChecksum(buffer + sizeof(_saveGameHeader.checksum), size - sizeof(_saveGameHeader.checksum))) {
		free(buffer);
		return SR_ERR_INCOMPATIBLE;
	}

	// Check savegame against length of current global variables resource
	// This would most probably be trapped by the checksum test anyway,
	// but it doesn't do any harm to check this as well.

	// Historical note: During development, earlier savegames would often
	// be shorter than the current expected length.

	if (_saveGameHeader.varLength != _resman->fetchLen(1)) {
		free(buffer);
		return SR_ERR_INCOMPATIBLE;
	}

	// Clean out system

	// Trash all resources from memory except player object & global vars
	_resman->killAll(false);

	// Clean out the system kill list (no more objects to kill)
	_logic->resetKillList();
	
	// Object hub is just after the standard header 
	memcpy(_resman->openResource(CUR_PLAYER_ID) + sizeof(StandardHeader), &_saveGameHeader.player_hub, sizeof(ObjectHub));

	_resman->closeResource(CUR_PLAYER_ID);

	// Fill in the player object structures from the header
	putPlayerStructures();

	// Copy variables from savegame buffer to memory
	byte *varsRes = _resman->openResource(1);

	memcpy(varsRes, buffer + sizeof(_saveGameHeader), _saveGameHeader.varLength);

#ifdef SCUMM_BIG_ENDIAN
	uint32 *globalVars = (uint32 *) (varsRes + sizeof(StandardHeader));
	const uint numVars = (_saveGameHeader.varLength - sizeof(StandardHeader)) / 4;

	for (uint i = 0; i < numVars; i++)
		globalVars[i] = SWAP_BYTES_32(globalVars[i]);
#endif

 	_resman->closeResource(1);

	// Free it now, rather than in RestoreGame, to unblock memory before
	// new screen & runlist loaded
	free(buffer);

	int32 pars[2];

	pars[0] = _saveGameHeader.screenId;
	pars[1] = 1;
	_logic->fnInitBackground(pars);

	// So palette not restored immediately after control panel - we want to
	// fade up instead!
	_thisScreen.new_palette = 99;

	// These need setting after the defaults get set in fnInitBackground.
	// Remember that these can change through the game, so need saving &
	// restoring too.

	_thisScreen.feet_x = _saveGameHeader.feet_x;
	_thisScreen.feet_y = _saveGameHeader.feet_y;

	// Start the new run list
	_logic->expressChangeSession(_saveGameHeader.runListId);

	// Force in the new scroll position, so unsightly scroll-catch-up does
	// not occur when screen first draws after returning from restore panel

	// set '_thisScreen's record of player position
	// - ready for setScrolling()

	_thisScreen.player_feet_x = _saveGameHeader.mega.feet_x;
	_thisScreen.player_feet_y = _saveGameHeader.mega.feet_y;

	// if this screen is wide, recompute the scroll offsets now
	if (_thisScreen.scroll_flag)
		setScrolling();

	// Any music required will be started after we've returned from
	// restoreControl() - see systemMenuMouse() in mouse.cpp!

	_loopingMusicId = _saveGameHeader.music_id;

	// Restart any looping music. Originally this was - and still is - done
	// in systemMenuMouse(), but with ScummVM we have other ways of
	// restoring savegames so it's easier to put it here as well.

	if (_loopingMusicId) {
		pars[0] = _loopingMusicId;
		pars[1] = FX_LOOP;
		_logic->fnPlayMusic(pars);
	} else
		_logic->fnStopMusic(NULL);

	return SR_OK;
}

/**
 * Get the description of a savegame
 */

uint32 Sword2Engine::getSaveDescription(uint16 slotNo, byte *description) {
	char saveFileName[MAX_FILENAME_LEN];

	sprintf(saveFileName, "%s.%.3d", _targetName, slotNo);

	SaveFileManager *mgr = _system->get_savefile_manager();
	SaveFile *in;

	if (!(in = mgr->open_savefile(saveFileName, getSavePath(), false))) {
		delete mgr;
		return SR_ERR_FILEOPEN;
	}

	SaveGameHeader dummy;

	in->read(&dummy, sizeof(dummy));
	delete in;
	delete mgr;

	strcpy((char *) description, dummy.description);
	return SR_OK;
}

bool Sword2Engine::saveExists(void) {
	for (int i = 0; i <= 99; i++)
		if (saveExists(i))
			return true;
	return false;
}

bool Sword2Engine::saveExists(uint16 slotNo) {
	char saveFileName[MAX_FILENAME_LEN];

	sprintf(saveFileName, "%s.%.3d", _targetName, slotNo);

	SaveFileManager *mgr = _system->get_savefile_manager();
	SaveFile *in;

	if (!(in = mgr->open_savefile(saveFileName, getSavePath(), false))) {
		delete mgr;
		return false;
	}

	delete in;
	delete mgr;
	return true;
}

/**
 * Request the player object structures which need saving.
 */

void Sword2Engine::getPlayerStructures(void) {
	StandardHeader *head = (StandardHeader *) _resman->openResource(CUR_PLAYER_ID);

	assert(head->fileType == GAME_OBJECT);

	char *raw_script_ad = (char *) head;

	// Script no. 7 - 'george_savedata_request' calls fnPassPlayerSaveData
	uint32 null_pc = 7;

	_logic->runScript(raw_script_ad, raw_script_ad, &null_pc);
	_resman->closeResource(CUR_PLAYER_ID);
}

/**
 * Fill out the player object structures from the savegame structures also run
 * the appropriate scripts to set up george's anim tables and walkdata, and
 * Nico's anim tables.
 */

void Sword2Engine::putPlayerStructures(void) {
	StandardHeader *head = (StandardHeader *) _resman->openResource(CUR_PLAYER_ID);

	assert(head->fileType == GAME_OBJECT);

	char *raw_script_ad = (char *) head;

	// Script no. 8 - 'george_savedata_return' calls fnGetPlayerSaveData

	uint32 null_pc = 8;
	_logic->runScript(raw_script_ad, raw_script_ad, &null_pc);

	// Script no. 14 - 'set_up_nico_anim_tables'

	null_pc = 14;
	_logic->runScript(raw_script_ad, raw_script_ad, &null_pc);

	// Which megaset was the player at the time of saving?

	switch (_saveGameHeader.mega.megaset_res) {
	case 36:		// GeoMega:
		null_pc = 9;	// script no.9	- 'player_is_george'
		break;
	case 2003:		// GeoMegaB:
		null_pc = 13;	// script no.13 - 'player_is_georgeB'
		break;
	case 1366:		// NicMegaA:
		null_pc = 11;	// script no.11 - 'player_is_nicoA'
		break;
	case 1437:		// NicMegaB:
		null_pc = 12;	// script no.12 - 'player_is_nicoB'
		break;
	case 1575:		// NicMegaC:
		null_pc = 10;	// script no.10 - 'player_is_nicoC'
		break;
	}

	_logic->runScript(raw_script_ad, raw_script_ad, &null_pc);
	_resman->closeResource(CUR_PLAYER_ID);
}

uint32 Sword2Engine::calcChecksum(byte *buffer, uint32 size) {
	uint32 total = 0;

	for (uint32 pos = 0; pos < size; pos++)
		total += buffer[pos];

	return total;
}

/**
 * Copies the 4 essential player structures into the savegame header - run
 * script 7 of player object to request this.
 *
 * Remember, we cannot simply read a compact any longer but instead must
 * request it from the object itself.
 */

int32 Logic::fnPassPlayerSaveData(int32 *params) {
	// params:	0 pointer to object's logic structure
	//		1 pointer to object's graphic structure
	//		2 pointer to object's mega structure

	// Copy from player object to savegame header

	memcpy(&_vm->_saveGameHeader.logic, _vm->_memory->decodePtr(params[0]), sizeof(ObjectLogic));
	memcpy(&_vm->_saveGameHeader.graphic, _vm->_memory->decodePtr(params[1]), sizeof(ObjectGraphic));
	memcpy(&_vm->_saveGameHeader.mega, _vm->_memory->decodePtr(params[2]), sizeof(ObjectMega));

	return IR_CONT;
}

/**
 * Reverse of fnPassPlayerSaveData() - run script 8 of player object.
 */

int32 Logic::fnGetPlayerSaveData(int32 *params) {
	// params:	0 pointer to object's logic structure
	//		1 pointer to object's graphic structure
	//		2 pointer to object's mega structure

	byte *logic_ptr = _vm->_memory->decodePtr(params[0]);
	byte *graphic_ptr = _vm->_memory->decodePtr(params[1]);
	byte *mega_ptr = _vm->_memory->decodePtr(params[2]);

	// Copy from savegame header to player object

	memcpy(logic_ptr, &_vm->_saveGameHeader.logic, sizeof(ObjectLogic));
	memcpy(graphic_ptr, &_vm->_saveGameHeader.graphic, sizeof(ObjectGraphic));
	memcpy(mega_ptr, &_vm->_saveGameHeader.mega, sizeof(ObjectMega));

 	// Any walk-data must be cleared - the player will be set to stand if
	// he was walking when saved.

	ObjectMega *ob_mega = (ObjectMega *) mega_ptr;

	if (ob_mega->currently_walking) {
		ob_mega->currently_walking = 0;

		int32 pars[3];

		pars[0] = params[1];			// ob_graphic;
		pars[1] = params[2];			// ob_mega
		pars[2] = ob_mega->current_dir;

		fnStand(pars);

		// Reset looping flag (which would have been 1 during fnWalk)
		ObjectLogic *ob_logic = (ObjectLogic *) logic_ptr;

		ob_logic->looping = 0;
	}

	return IR_CONT;
}

} // End of namespace Sword2
