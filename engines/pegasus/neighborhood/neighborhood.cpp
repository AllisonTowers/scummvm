/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * Additional copyright for this file:
 * Copyright (C) 1995-1997 Presto Studios, Inc.
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

#include "common/debug.h"
#include "common/stream.h"

#include "pegasus/compass.h"
#include "pegasus/cursor.h"
#include "pegasus/energymonitor.h"
#include "pegasus/gamestate.h"
#include "pegasus/input.h"
#include "pegasus/interface.h"
#include "pegasus/pegasus.h"
#include "pegasus/ai/ai_area.h"
#include "pegasus/neighborhood/neighborhood.h"

namespace Pegasus {

StriderCallBack::StriderCallBack(Neighborhood *neighborhood) {
	_neighborhood = neighborhood;
}

void StriderCallBack::callBack() {
	_neighborhood->checkStriding();
}

static const TimeValue kStridingSlop = 39;

Neighborhood *g_neighborhood = 0;

Neighborhood::Neighborhood(InputHandler *nextHandler, PegasusEngine *vm, const Common::String &resName, tNeighborhoodID id)
		: InputHandler(nextHandler), IDObject(id), _vm(vm), _resName(resName), _navMovie(kNavMovieID), _stridingCallBack(this),
		_neighborhoodNotification(kNeighborhoodNotificationID, (NotificationManager *)vm), _pushIn(kNoDisplayElement),
		_turnPush(kTurnPushID), _croppedMovie(kCroppedMovieID) {
	GameState.setOpenDoorLocation(kNoRoomID, kNoDirection);
	_currentAlternate = 0;
	_interruptionFilter = kFilterAllInput;
	g_neighborhood = this;
}

Neighborhood::~Neighborhood() {
	for (HotspotIterator it = _neighborhoodHotspots.begin(); it != _neighborhoodHotspots.end(); it++)
		g_allHotspots.remove(*it);

	_neighborhoodHotspots.deleteHotspots();
	g_neighborhood = 0;
}

void Neighborhood::init() {
	debug(0, "Loading '%s' neighborhood resources", _resName.c_str());

	Common::SeekableReadStream *stream = _vm->_resFork->getResource(_doorTable.getResTag(), _resName);
	if (!stream)
		error("Failed to load doors");
	_doorTable.loadFromStream(stream);
	delete stream;

	stream = _vm->_resFork->getResource(_exitTable.getResTag(), _resName);
	if (!stream)
		error("Failed to load exits");
	_exitTable.loadFromStream(stream);
	delete stream;

	stream = _vm->_resFork->getResource(_extraTable.getResTag(), _resName);
	if (!stream)
		error("Failed to load extras");
	_extraTable.loadFromStream(stream);
	delete stream;

	stream = _vm->_resFork->getResource(_hotspotInfoTable.getResTag(), _resName);
	if (!stream)
		error("Failed to load hotspot info");
	_hotspotInfoTable.loadFromStream(stream);
	delete stream;

	stream = _vm->_resFork->getResource(_spotTable.getResTag(), _resName);
	if (!stream)
		error("Failed to load spots");
	_spotTable.loadFromStream(stream);
	delete stream;

	stream = _vm->_resFork->getResource(_turnTable.getResTag(), _resName);
	if (!stream)
		error("Failed to load turns");
	_turnTable.loadFromStream(stream);
	delete stream;

	stream = _vm->_resFork->getResource(_viewTable.getResTag(), _resName);
	if (!stream)
		error("Failed to load views");
	_viewTable.loadFromStream(stream);
	delete stream;

	stream = _vm->_resFork->getResource(_zoomTable.getResTag(), _resName);
	if (!stream)
		error("Failed to load zooms");
	_zoomTable.loadFromStream(stream);
	delete stream;

	createNeighborhoodSpots();

	// TODO: Load _navMovie
	_navMovie.setVolume(_vm->getSoundFXLevel());

	loadSoundSpots();
	_spotSounds.setVolume(_vm->getSoundFXLevel());

	_navMovie.setDisplayOrder(kNavMovieOrder);
	_navMovie.startDisplaying();

	Common::Rect bounds;
	_navMovie.getBounds(bounds);
	_pushIn.allocateSurface(bounds);

	_turnPush.setInAndOutElements(&_pushIn, &_navMovie);
	_turnPush.setDisplayOrder(kTurnPushOrder);
	_turnPush.startDisplaying();
	_navMovieCallBack.initCallBack(&_navMovie, kCallBackAtExtremes);
	_stridingCallBack.initCallBack(&_navMovie, kCallBackAtTime);
	_turnPushCallBack.initCallBack(&_turnPush, kCallBackAtExtremes);
	_delayCallBack.initCallBack(&_delayTimer, kCallBackAtExtremes);

	// TODO
	//_spotSoundCallBack.initCallBack(&_spotSounds, kCallBackAtExtremes);

	setUpAIRules();

	if (g_compass)
		g_compass->setFaderValue(getStaticCompassAngle(GameState.getCurrentRoom(), GameState.getCurrentDirection()));

	_soundLoop1.attachFader(&_loop1Fader);
	_soundLoop2.attachFader(&_loop2Fader);
	startIdling();
}

void Neighborhood::start() {
	GameState.setCurrentRoom(GameState.getLastRoom());
	GameState.setCurrentDirection(GameState.getLastDirection());
	arriveAt(GameState.getNextRoom(), GameState.getNextDirection());
}

void Neighborhood::receiveNotification(Notification *, const tNotificationFlags flags) {
	if ((flags & (kNeighborhoodMovieCompletedFlag | kTurnCompletedFlag)) != 0 && g_AIArea)
		g_AIArea->unlockAI();
	if (flags & kMoveForwardCompletedFlag)
		arriveAt(GameState.getNextRoom(), GameState.getNextDirection());
	if (flags & kTurnCompletedFlag)
		turnTo(GameState.getNextDirection());
	if (flags & kSpotCompletedFlag)
		spotCompleted();
	if (flags & kDoorOpenCompletedFlag)
		doorOpened();
	if (flags & kActionRequestCompletedFlag)
		popActionQueue();
	if (flags & kDeathExtraCompletedFlag)
		die(_extraDeathReason);
}

void Neighborhood::arriveAt(tRoomID room, tDirectionConstant direction) {
	// TODO: Map

	GameState.setCurrentNeighborhood(getObjectID());

	_currentActivation = kActivateHotSpotAlways;
	_interruptionFilter = kFilterAllInput;

	if (room != GameState.getCurrentRoom() || direction != GameState.getCurrentDirection()) {
		GameState.setCurrentRoom(room);
		GameState.setCurrentDirection(direction);
		loadAmbientLoops();
		activateCurrentView(room, direction, kSpotOnArrivalMask);
	} else {
		loadAmbientLoops();
		showViewFrame(getViewTime(GameState.getCurrentRoom(), GameState.getCurrentDirection()));
	}

	if (GameState.getOpenDoorRoom() != kNoRoomID) {
		// Arriving always closes a door.
		loadAmbientLoops();
		closeDoorOffScreen(GameState.getOpenDoorRoom(), GameState.getOpenDoorDirection());
		GameState.setOpenDoorLocation(kNoRoomID, kNoDirection);
	}

	if (g_compass)
		g_compass->setFaderValue(getStaticCompassAngle(GameState.getCurrentRoom(), GameState.getCurrentDirection()));

	if (g_AIArea)
		g_AIArea->checkMiddleArea();

	checkContinuePoint(room, direction);
}

//	These functions can be overridden to tweak the exact frames used.

void Neighborhood::getExitEntry(const tRoomID room, const tDirectionConstant direction, ExitTable::Entry &entry) {
	entry = _exitTable.findEntry(room, direction, _currentAlternate);

	if (entry.isEmpty())
		entry = _exitTable.findEntry(room, direction, kNoAlternateID);
}

TimeValue Neighborhood::getViewTime(const tRoomID room, const tDirectionConstant direction) {
	if (GameState.getOpenDoorRoom() == room && GameState.getOpenDoorDirection() == direction) {
		// If we get here, the door entry for this location must exist.
		DoorTable::Entry doorEntry = _doorTable.findEntry(room, direction, _currentAlternate);

		if (doorEntry.isEmpty())
			doorEntry = _doorTable.findEntry(room, direction, kNoAlternateID);

		return doorEntry.movieEnd - 1;
	}

	ViewTable::Entry viewEntry = _viewTable.findEntry(room, direction, _currentAlternate);

	if (viewEntry.isEmpty())
		viewEntry = _viewTable.findEntry(room, direction, kNoAlternateID);

	return viewEntry.time;
}

void Neighborhood::getDoorEntry(const tRoomID room, const tDirectionConstant direction, DoorTable::Entry &doorEntry) {
	doorEntry = _doorTable.findEntry(room, direction, _currentAlternate);

	if (doorEntry.isEmpty())
		doorEntry = _doorTable.findEntry(room, direction, kNoAlternateID);
}

tDirectionConstant Neighborhood::getTurnEntry(const tRoomID room, const tDirectionConstant direction, const tTurnDirection turnDirection) {
	TurnTable::Entry turnEntry = _turnTable.findEntry(room, direction, turnDirection, _currentAlternate);

	if (turnEntry.isEmpty())
		turnEntry = _turnTable.findEntry(room, direction, turnDirection, kNoAlternateID);

	return turnEntry.turnDirection;
}

void Neighborhood::findSpotEntry(const tRoomID room, const tDirectionConstant direction, tSpotFlags flags, SpotTable::Entry &spotEntry) {
	spotEntry = _spotTable.findEntry(room, direction, flags, _currentAlternate);

	if (spotEntry.isEmpty())
		spotEntry = _spotTable.findEntry(room, direction, flags, kNoAlternateID);
}

void Neighborhood::getZoomEntry(const tHotSpotID id, ZoomTable::Entry &zoomEntry) {
	zoomEntry = _zoomTable.findEntry(id);
}

void Neighborhood::getHotspotEntry(const tHotSpotID id, HotspotInfoTable::Entry &hotspotEntry) {
	hotspotEntry = _hotspotInfoTable.findEntry(id);
}

void Neighborhood::getExtraEntry(const uint32 id, ExtraTable::Entry &extraEntry) {
	extraEntry = _extraTable.findEntry(id);
}

/////////////////////////////////////////////
//
//	"Can" functions: Called to see whether or not a user is allowed to do something

tCanMoveForwardReason Neighborhood::canMoveForward(ExitTable::Entry &entry) {
	DoorTable::Entry door;
	
	getExitEntry(GameState.getCurrentRoom(), GameState.getCurrentDirection(), entry);
	getDoorEntry(GameState.getCurrentRoom(), GameState.getCurrentDirection(), door);

	//	Fixed this so that doors that don't lead anywhere can be opened, but not walked
	//	through.
	if (door.flags & kDoorPresentMask) {
		if (GameState.isCurrentDoorOpen()) {
			if (entry.exitRoom == kNoRoomID)
				return kCantMoveBlocked;
			else
				return kCanMoveForward;
		} else if (door.flags & kDoorLockedMask) {
			return kCantMoveDoorLocked;
		} else {
			return kCantMoveDoorClosed;
		}
	} else if (entry.exitRoom == kNoRoomID) {
		return kCantMoveBlocked;
	}

	return kCanMoveForward;
}

tCanTurnReason Neighborhood::canTurn(tTurnDirection turnDirection, tDirectionConstant &nextDir) {
	nextDir = getTurnEntry(GameState.getCurrentRoom(), GameState.getCurrentDirection(), turnDirection);

	if (nextDir == kNoDirection)
		return kCantTurnNoTurn;

	return kCanTurn;
}

tCanOpenDoorReason Neighborhood::canOpenDoor(DoorTable::Entry &entry) {
	getDoorEntry(GameState.getCurrentRoom(), GameState.getCurrentDirection(), entry);

	if (entry.flags & kDoorPresentMask) {
		if (GameState.isCurrentDoorOpen())
			return kCantOpenAlreadyOpen;

		if (entry.flags & kDoorLockedMask)
			return kCantOpenLocked;

		return kCanOpenDoor;
	}

	return kCantOpenNoDoor;
}

void Neighborhood::createNeighborhoodSpots() {
	Common::SeekableReadStream *hotspotList = _vm->_resFork->getResource(MKTAG('H', 'S', 'L', 's'), _resName);
	if (!hotspotList)
		error("Could not load neighborhood hotspots");

	uint32 hotspotCount = hotspotList->readUint32BE();

	while (hotspotCount--) {
		uint16 id = hotspotList->readUint16BE();
		uint32 flags = hotspotList->readUint32BE();
		uint32 rgnSize = hotspotList->readUint32BE();

		int32 startPos = hotspotList->pos();

		debug(0, "Hotspot %d:", id);
		Region region(hotspotList);

		hotspotList->seek(startPos + rgnSize);

		Hotspot *hotspot = new Hotspot(id);
		hotspot->setHotspotFlags(flags);
		hotspot->setArea(region);

		g_allHotspots.push_back(hotspot);
		_neighborhoodHotspots.push_back(hotspot);
	}

	delete hotspotList;
}

void Neighborhood::loadSoundSpots() {
	// TODO: Eventually push to the subclasses

	Common::String fileName = "Sounds/";

	switch (getObjectID()) {
	case kCaldoriaID:
		fileName += "Caldoria/Caldoria Spots";
		break;
	case kFullTSAID:
	case kFinalTSAID:
	case kTinyTSAID:
		fileName += "TSA/TSA Spots";
		break;
	case kPrehistoricID:
		fileName += "Prehistoric/Prehistoric Spots";
		break;
	case kMarsID:
		fileName += "Mars/Mars Spots";
		break;
	case kWSCID:
		fileName += "World Science Center/WSC Spots";
		break;
	case kNoradAlphaID:
		fileName += "Norad/Norad Alpha Spots";
		break;
	case kNoradDeltaID:
		fileName += "Norad/Norad Delta Spots";
		break;
	}

	_spotSounds.initFromQuickTime(fileName);
}

void Neighborhood::popActionQueue() {	
	if (!_actionQueue.empty()) {
		tQueueRequest topRequest = _actionQueue.pop();

		switch (topRequest.requestType) {
			case kNavExtraRequest:
				// TODO
				break;
			case kSpotSoundRequest:
				_spotSounds.stopSound();
				break;
			case kDelayRequest:
				// TODO
				break;
		}

		serviceActionQueue();
	}
}

void Neighborhood::serviceActionQueue() {
	if (!_actionQueue.empty()) {
		tQueueRequest &topRequest = _actionQueue.front();

		if (!topRequest.playing) {
			topRequest.playing = true;
			switch (topRequest.requestType) {
			case kNavExtraRequest:
				// TODO
				break;
			case kSpotSoundRequest:
				_spotSounds.stopSound();
				_spotSounds.playSoundSegment(topRequest.start, topRequest.stop);
				_interruptionFilter = topRequest.interruptionFilter;
				// TODO: stop trigger
				break;
			case kDelayRequest:
				// TODO
				break;
			}
		}
	} else {
		_interruptionFilter = kFilterAllInput;
	}
}

void Neighborhood::requestAction(const tQueueRequestType requestType, const tExtraID extra, const TimeValue in, const TimeValue out,
		const tInputBits interruptionFilter, const tNotificationFlags flags) {

	tQueueRequest request;
	
	request.requestType = requestType;
	request.extra = extra;
	request.start = in;
	request.stop = out;
	request.interruptionFilter = interruptionFilter;
	request.playing = false;
	request.flags = flags | kActionRequestCompletedFlag;

	// TODO: notification

	_actionQueue.push(request);
	if (_actionQueue.size() == 1)
		serviceActionQueue();
}

void Neighborhood::requestExtraSequence(const tExtraID whichExtra, const tNotificationFlags flags, const tInputBits interruptionFilter) {
	requestAction(kNavExtraRequest, whichExtra, 0, 0, interruptionFilter, flags);
}

void Neighborhood::requestSpotSound(const TimeValue in, const TimeValue out, const tInputBits interruptionFilter, const tNotificationFlags flags) {
	requestAction(kSpotSoundRequest, 0xFFFFFFFF, in, out, interruptionFilter, flags);
}

void Neighborhood::playSpotSoundSync(const TimeValue in, const TimeValue out) {
	// Let the action queue play out first...
	while (!actionQueueEmpty()) {
		_vm->refreshDisplay();
		_vm->checkNotifications();
		_vm->_system->delayMillis(10);
	}

	_spotSounds.stopSound();
	_spotSounds.playSoundSegment(in * 1000 / 600, out * 1000 / 600);

	while (_spotSounds.isPlaying()) {
		_vm->refreshDisplay();
		_vm->_system->delayMillis(10);
	}
}

void Neighborhood::requestDelay(const TimeValue delayDuration, const TimeScale delayScale, const tInputBits interruptionFilter, const tNotificationFlags flags) {
	requestAction(kDelayRequest, 0xFFFFFFFF, delayDuration, delayScale, interruptionFilter, flags);
}

bool operator==(const tQueueRequest &arg1, const tQueueRequest &arg2) {
	return arg1.requestType == arg2.requestType && arg1.extra == arg2.extra &&
			arg1.start == arg2.start && arg1.stop == arg2.stop;
}

bool operator!=(const tQueueRequest &arg1, const tQueueRequest &arg2) {
	return !operator==(arg1, arg2);
}

Common::String Neighborhood::getBriefingMovie() {
	// TODO: Interaction check

	return Common::String();
}

Common::String Neighborhood::getEnvScanMovie() {
	// TODO: Interaction check

	return Common::String();
}

uint Neighborhood::getNumHints() {
	// TODO: Interaction check

	return 0;
}

Common::String Neighborhood::getHintMovie(uint hintNum) {
	// TODO: Interaction check

	return Common::String();
}

bool Neighborhood::canSolve() {
	// TODO: Interaction check

	return false;
}

void Neighborhood::doSolve() {
	// TODO: Interaction check
}

bool Neighborhood::okayToJump() {
	return !_vm->playerHasItemID(kGasCanister) && !_vm->playerHasItemID(kMachineGun);
}

tAirQuality Neighborhood::getAirQuality(const tRoomID) {
	return kAirQualityGood;
}

void Neighborhood::checkStriding() {
	if (stillMoveForward()) {
		ExitTable::Entry nextExit;
		getExitEntry(GameState.getNextRoom(), GameState.getNextDirection(), nextExit);
		keepStriding(nextExit);
	} else {
		stopStriding();
	}
}

bool Neighborhood::stillMoveForward() {
	Input input;

	InputHandler::readInputDevice(input);
	return input.upButtonAnyDown();
}

void Neighborhood::keepStriding(ExitTable::Entry &nextExitEntry) {
	FaderMoveSpec compassMove;

	// TODO: Map
	if (g_compass)
		getExitCompassMove(nextExitEntry, compassMove);

	GameState.setCurrentRoom(GameState.getNextRoom());
	GameState.setCurrentDirection(GameState.getNextDirection());
	GameState.setNextRoom(nextExitEntry.exitRoom);
	GameState.setNextDirection(nextExitEntry.exitDirection);

	if (nextExitEntry.movieEnd == nextExitEntry.exitEnd)
		scheduleNavCallBack(kNeighborhoodMovieCompletedFlag | kMoveForwardCompletedFlag);
	else
		scheduleStridingCallBack(nextExitEntry.movieEnd - kStridingSlop, kStrideCompletedFlag);

	if (g_compass)
		g_compass->startFader(compassMove);
}

void Neighborhood::stopStriding() {
	_navMovie.stop();
	_neighborhoodNotification.setNotificationFlags(kNeighborhoodMovieCompletedFlag |
			kMoveForwardCompletedFlag, kNeighborhoodMovieCompletedFlag | kMoveForwardCompletedFlag);
}

//	Compass support
uint16 Neighborhood::getStaticCompassAngle(const tRoomID, const tDirectionConstant dir) {
	//	North, south, east, west
	static const uint16 compassAngles[] = { 0, 180, 90, 270 };
	return compassAngles[dir];
}

void Neighborhood::getExitCompassMove(const ExitTable::Entry &exitEntry, FaderMoveSpec &compassMove) {	
	int32 startAngle = getStaticCompassAngle(exitEntry.room, exitEntry.direction);
	int32 stopAngle = getStaticCompassAngle(exitEntry.exitRoom, exitEntry.exitDirection);

	if (startAngle > stopAngle) {
		if (stopAngle + 180 < startAngle)
			stopAngle += 360;
	} else {
		if (startAngle + 180 < stopAngle)
			startAngle += 360;
	}

	compassMove.makeTwoKnotFaderSpec(_navMovie.getScale(), exitEntry.movieStart, startAngle, exitEntry.movieEnd, stopAngle);
}

void Neighborhood::scheduleNavCallBack(tNotificationFlags flags) {
	_navMovieCallBack.cancelCallBack();

	if (flags != 0) {
		_navMovieCallBack.setCallBackFlag(flags);
		_navMovieCallBack.scheduleCallBack(kTriggerAtStop, 0, 0);
	}
}

void Neighborhood::scheduleStridingCallBack(const TimeValue strideStop, tNotificationFlags flags) {	
	_stridingCallBack.cancelCallBack();

	if (flags != 0)
		_stridingCallBack.scheduleCallBack(kTriggerTimeFwd, strideStop, _navMovie.getScale());
}

void Neighborhood::moveNavTo(const tCoordType h, const tCoordType v) {
	tCoordType oldH, oldV;
	_navMovie.getLocation(oldH, oldV);

	tCoordType offH = h - oldH;
	tCoordType offV = v - oldV;

	_navMovie.moveElementTo(h, v);
	_turnPush.moveElementTo(h, v);

	if (offH != 0 || offV != 0)
		for (HotspotList::iterator it = _neighborhoodHotspots.begin(); it != _neighborhoodHotspots.end(); it++)
			if ((*it)->getHotspotFlags() & kNeighborhoodSpotFlag)
				(*it)->moveSpot(offH, offV);
}

void Neighborhood::activateHotspots() {	
	InputHandler::activateHotspots();

	for (HotspotInfoTable::iterator it = _hotspotInfoTable.begin(); it != _hotspotInfoTable.end(); it++) {
		HotspotInfoTable::Entry entry = *it;

		if (entry.hotspotRoom == GameState.getCurrentRoom() && entry.hotspotDirection == GameState.getCurrentDirection()
				&& (entry.hotspotActivation == _currentActivation || entry.hotspotActivation == kActivateHotSpotAlways)) {
			Hotspot *hotspot = g_allHotspots.findHotspotByID(entry.hotspot);
			if (hotspot)
				activateOneHotspot(entry, hotspot);
		}
	}
}

void Neighborhood::clickInHotspot(const Input &input, const Hotspot *clickedSpot) {	
	tHotSpotFlags flags = clickedSpot->getHotspotFlags();

	if ((flags & (kPickUpItemSpotFlag | kPickUpBiochipSpotFlag)) != 0) {
		tItemID itemID = kNoItemID;

		for (HotspotInfoTable::iterator it = _hotspotInfoTable.begin(); it != _hotspotInfoTable.end(); it++) {
			if (it->hotspot == clickedSpot->getObjectID()) {
				itemID = it->hotspotItem;
				break;
			}
		}

		if (itemID != kNoItemID) {
			Item *draggingItem = g_allItems.findItemByID(itemID);

			if (draggingItem) {
				takeItemFromRoom(draggingItem);

				if ((flags & kPickUpItemSpotFlag) != 0)
					_vm->dragItem(input, draggingItem, kDragInventoryPickup);
				else
					_vm->dragItem(input, draggingItem, kDragBiochipPickup);
			}
		}
	} else {
		// Check other flags here?
		if ((flags & kZoomSpotFlags) != 0) {
			zoomTo(clickedSpot);
		} else if ((flags & kPlayExtraSpotFlag) != 0) {
			HotspotInfoTable::Entry hotspotEntry;
			getHotspotEntry(clickedSpot->getObjectID(), hotspotEntry);
			startExtraSequence(hotspotEntry.hotspotExtra, kExtraCompletedFlag, kFilterNoInput);
		} else if ((flags & kOpenDoorSpotFlag) != 0) {
			openDoor();
		} else {
			InputHandler::clickInHotspot(input, clickedSpot);
		}
	}
}

void Neighborhood::cantMoveThatWay(tCanMoveForwardReason reason) {
	switch (reason) {
	case kCantMoveDoorClosed:
	case kCantMoveDoorLocked:
		openDoor();
		break;
	case kCantMoveBlocked:
		zoomUpOrBump();
		break;
	default:
		bumpIntoWall();
		break;
	}
}

void Neighborhood::cantOpenDoor(tCanOpenDoorReason) {
	bumpIntoWall();
}

void Neighborhood::turnTo(const tDirectionConstant direction) {
	// TODO: Map

	_pushIn.copyToCurrentPort();
	
	// Added 2/10/97. Shouldn't this be here? Shouldn't we set the current activation to
	// always when turning to a new view?
	_currentActivation = kActivateHotSpotAlways;
	
	_interruptionFilter = kFilterAllInput;

	if (direction != GameState.getCurrentDirection()) {
		GameState.setCurrentDirection(direction);
		activateCurrentView(GameState.getCurrentRoom(), direction, kSpotOnTurnMask);
	} else {
		showViewFrame(getViewTime(GameState.getCurrentRoom(), GameState.getCurrentDirection()));
	}

	if (GameState.getOpenDoorRoom() != kNoRoomID) {
		// Turning always closes a door.
		loadAmbientLoops();
		closeDoorOffScreen(GameState.getOpenDoorRoom(), GameState.getOpenDoorDirection());
		GameState.setOpenDoorLocation(kNoRoomID, kNoDirection);
	}

	if (g_AIArea)
		g_AIArea->checkMiddleArea();

	checkContinuePoint(GameState.getCurrentRoom(), direction);

	_vm->_cursor->hideUntilMoved();
}

void Neighborhood::spotCompleted() {
	_interruptionFilter = kFilterAllInput;
	showViewFrame(getViewTime(GameState.getCurrentRoom(), GameState.getCurrentDirection()));
}

void Neighborhood::doorOpened() {	
	_interruptionFilter = kFilterAllInput;

	// 2/23/97
	// Fixes funny bug with doors that are opened by dropping things on them...
	setCurrentActivation(kActivateHotSpotAlways);

	GameState.setOpenDoorLocation(GameState.getCurrentRoom(), GameState.getCurrentDirection());

	SpotTable::Entry entry;
	findSpotEntry(GameState.getCurrentRoom(), GameState.getCurrentDirection(), kSpotOnDoorOpenMask, entry);

	if (entry.dstFlags & kSpotOnDoorOpenMask) {
		startSpotOnceOnly(entry.movieStart, entry.movieEnd);
	} else {
		findSpotEntry(GameState.getCurrentRoom(), GameState.getCurrentDirection(), kSpotOnDoorOpenMask | kSpotLoopsMask, entry);

		if (entry.dstFlags & kSpotOnDoorOpenMask)
			startSpotLoop(entry.movieStart, entry.movieEnd);
	}

	loadAmbientLoops();

	// TODO: Map

	if (g_AIArea)
		g_AIArea->checkMiddleArea();
}

void Neighborhood::moveForward() {
	ExitTable::Entry exitEntry;
	tCanMoveForwardReason moveReason = canMoveForward(exitEntry);

	if (moveReason == kCanMoveForward)
		startExitMovie(exitEntry);
	else
		cantMoveThatWay(moveReason);
}

void Neighborhood::turn(const tTurnDirection turnDirection) {
	tDirectionConstant nextDir;	
	tCanTurnReason turnReason = canTurn(turnDirection, nextDir);

	if (turnReason == kCanTurn)
		startTurnPush(turnDirection, getViewTime(GameState.getCurrentRoom(), nextDir), nextDir);
	else
		cantTurnThatWay(turnReason);
}

void Neighborhood::turnLeft() {
	turn(kTurnLeft);
}

void Neighborhood::turnRight() {
	turn(kTurnRight);
}

void Neighborhood::turnUp() {
	turn(kTurnUp);
}

void Neighborhood::turnDown() {
	turn(kTurnDown);
}

void Neighborhood::openDoor() {
	DoorTable::Entry door;	
	tCanOpenDoorReason doorReason = canOpenDoor(door);

	if (doorReason == kCanOpenDoor)
		startDoorOpenMovie(door.movieStart, door.movieEnd);
	else
		cantOpenDoor(doorReason);
}

void Neighborhood::zoomTo(const Hotspot *hotspot) {
	ZoomTable::Entry zoomEntry;
	getZoomEntry(hotspot->getObjectID(), zoomEntry);
	if (!zoomEntry.isEmpty())
		startZoomMovie(zoomEntry);
}

void Neighborhood::updateViewFrame() {
	showViewFrame(getViewTime(GameState.getCurrentRoom(), GameState.getCurrentDirection()));
}

void Neighborhood::startSpotLoop(TimeValue startTime, TimeValue stopTime, tNotificationFlags flags) {
	_turnPush.hide();
	startMovieSequence(startTime, stopTime, flags, true, kFilterAllInput);
}

void Neighborhood::showViewFrame(TimeValue viewTime) {	
	if ((int32)viewTime >= 0) {
		_turnPush.hide();
		_navMovie.stop();
		_navMovie.setFlags(0);
		_navMovie.setSegment(0, _navMovie.getDuration());
		_navMovie.setTime(viewTime);

		Common::Rect pushBounds;
		_turnPush.getBounds(pushBounds);

		_navMovie.moveElementTo(pushBounds.left, pushBounds.top);
		_navMovie.show();
		_navMovie.redrawMovieWorld();
	}
}

void Neighborhood::startExtraSequence(const tExtraID extraID, const tNotificationFlags flags, const tInputBits interruptionFilter) {
	ExtraTable::Entry entry;
	getExtraEntry(extraID, entry);

	if (entry.movieStart != 0xffffffff)
		playExtraMovie(entry, flags, interruptionFilter);
}

bool Neighborhood::startExtraSequenceSync(const tExtraID extraID, const tInputBits interruptionFilter) {
	InputHandler::getCurrentInputDevice()->waitInput(interruptionFilter);
	return prepareExtraSync(extraID) && waitMovieFinish(&_navMovie, interruptionFilter);
}

void Neighborhood::loopExtraSequence(const uint32 extraID, tNotificationFlags flags) {
	ExtraTable::Entry entry;
	getExtraEntry(extraID, entry);
	
	if (entry.movieStart != 0xffffffff) {
		_lastExtra = extraID;
		startSpotLoop(entry.movieStart, entry.movieEnd, flags);
	}
}

bool Neighborhood::navMoviePlaying() {
	return _navMovie.isRunning();
}

void Neighborhood::playDeathExtra(tExtraID extra, tDeathReason deathReason) {
	_extraDeathReason = deathReason;
	startExtraSequence(extra, kDeathExtraCompletedFlag, kFilterNoInput);
}

void Neighborhood::die(const tDeathReason deathReason) {
	loadLoopSound1("");
	loadLoopSound2("");
	_vm->die(deathReason);
}

void Neighborhood::setSoundFXLevel(const uint16 fxLevel) {
	if (_navMovie.isSurfaceValid())
		_navMovie.setVolume(fxLevel);
	if (_spotSounds.isSoundLoaded())
		_spotSounds.setVolume(fxLevel);
	// TODO
	//if (_currentInteraction)
	//	_currentInteraction->setSoundFXLevel(fxLevel);
}

void Neighborhood::setAmbienceLevel(const uint16 ambientLevel) {
	if (_soundLoop1.isSoundLoaded())
		_loop1Fader.setMasterVolume(_vm->getAmbienceLevel());
	if (_soundLoop2.isSoundLoaded())
		_loop2Fader.setMasterVolume(_vm->getAmbienceLevel());
	// TODO
	//if (_currentInteraction)
	//	_currentInteraction->setAmbienceLevel(ambientLevel);
}

// Force the exit taken from (room, direction, alternate) to come to a stop.
void Neighborhood::forceStridingStop(const tRoomID room, const tDirectionConstant direction, const tAlternateID alternate) {
	ExitTable::Entry entry = _exitTable.findEntry(room, direction, alternate);

	if (entry.movieStart != 0xffffffff) {
		TimeValue strideStop = entry.exitEnd;
		TimeValue exitStop = entry.movieEnd;

		if (strideStop != exitStop) {
			for (ExitTable::iterator it = _exitTable.begin(); it != _exitTable.end(); it++) {
				entry = *it;

				if (entry.exitEnd == strideStop && entry.movieEnd <= exitStop) {
					entry.exitEnd = exitStop;
					*it = entry;
				}
			}
		}
	}
}

// Restore the exit taken from (room, direction, alternate) to stride.
void Neighborhood::restoreStriding(const tRoomID room, const tDirectionConstant direction, const tAlternateID alternate) {
	ExitTable::Entry entry = _exitTable.findEntry(room, direction, alternate);

	if (entry.movieStart != 0xffffffff) {
		TimeValue strideStop = entry.exitEnd;
		TimeValue exitStop = entry.movieEnd;

		if (strideStop != entry.originalEnd) {
			for (ExitTable::iterator it = _exitTable.begin(); it != _exitTable.end(); it++) {
				entry = *it;

				if (entry.exitEnd == strideStop && entry.movieEnd <= exitStop) {
					entry.exitEnd = entry.originalEnd;
					*it = entry;
				}
			}
		}
	}
}

HotspotInfoTable::Entry *Neighborhood::findHotspotEntry(const tHotSpotID id) {
	for (HotspotInfoTable::iterator it = _hotspotInfoTable.begin(); it != _hotspotInfoTable.end(); it++)
		if (it->hotspot == id)
			return &(*it);

	return 0;
}

void Neighborhood::hideNav() {
	_isRunning = _navMovie.isRunning();
	_navMovie.stop();
	_navMovie.hide();
	_turnPush.stopFader();
	_turnPush.hide();
}

void Neighborhood::showNav() {
	_navMovie.show();
	_turnPush.hide();
	if (_isRunning)
		_navMovie.start();
}

void Neighborhood::startExitMovie(const ExitTable::Entry &exitEntry) {
	FaderMoveSpec compassMove;
	
	if (g_compass)
		getExitCompassMove(exitEntry, compassMove);

	GameState.setNextRoom(exitEntry.exitRoom);
	GameState.setNextDirection(exitEntry.exitDirection);

	if (exitEntry.movieEnd == exitEntry.exitEnd) // Just a walk.
		startMovieSequence(exitEntry.movieStart, exitEntry.movieEnd, kMoveForwardCompletedFlag, kFilterNoInput, false);
	else // We're stridin'!
		startMovieSequence(exitEntry.movieStart, exitEntry.exitEnd, kStrideCompletedFlag, kFilterNoInput, false, exitEntry.movieEnd);

	if (g_compass)
		g_compass->startFader(compassMove);
}

void Neighborhood::startZoomMovie(const ZoomTable::Entry &zoomEntry) {
	FaderMoveSpec compassMove;
	
	if (g_compass)
		getZoomCompassMove(zoomEntry, compassMove);

	GameState.setNextRoom(zoomEntry.room);
	GameState.setNextDirection(zoomEntry.direction);

	startMovieSequence(zoomEntry.movieStart, zoomEntry.movieEnd, kMoveForwardCompletedFlag, kFilterNoInput, false);

	if (g_compass)
		g_compass->startFader(compassMove);
}

void Neighborhood::startDoorOpenMovie(const TimeValue startTime, const TimeValue stopTime) {
	startMovieSequence(startTime, stopTime, kDoorOpenCompletedFlag, kFilterNoInput, false);
}

void Neighborhood::startTurnPush(const tTurnDirection turnDirection, const TimeValue newView, const tDirectionConstant nextDir) {
	if (g_AIArea)
		g_AIArea->lockAIOut();

	_vm->_cursor->hide();

	GameState.setNextDirection(nextDir);

	_interruptionFilter = kFilterNoInput;
	_turnPush.stopFader();

	// Set up callback.
	_turnPushCallBack.setCallBackFlag(kTurnCompletedFlag);
	_turnPushCallBack.scheduleCallBack(kTriggerAtStop, 0, 0);

	// Stop nav movie.
	_navMovie.stop();
	_navMovie.setFlags(0);

	// Set segment of nav movie to whole movie, so that subsequent initFromMovieFrame
	// will work.
	_navMovie.setSegment(0, _navMovie.getDuration());

	// TODO
	//_pushIn.initFromMovieFrame(_navMovie.getMovie(), newView, kNoMask);

	_navMovie.hide();

	switch (turnDirection) {
	case kTurnLeft:
		_turnPush.setSlideDirection(kSlideRightMask);
		break;
	case kTurnRight:
		_turnPush.setSlideDirection(kSlideLeftMask);
		break;
	case kTurnUp:
		_turnPush.setSlideDirection(kSlideDownMask);
		break;
	case kTurnDown:
		_turnPush.setSlideDirection(kSlideUpMask);
		break;
	}

	_turnPush.show();

	FaderMoveSpec moveSpec;
	moveSpec.makeTwoKnotFaderSpec(60, 0, 0, 15, 1000);
	_turnPush.startFader(moveSpec);

	if (g_compass) {
		_turnPush.pauseFader();

		int32 startAngle = getStaticCompassAngle(GameState.getCurrentRoom(), GameState.getCurrentDirection());
		int32 stopAngle = getStaticCompassAngle(GameState.getCurrentRoom(), nextDir);

		if (turnDirection == kTurnLeft) {
			if (startAngle < stopAngle)
				startAngle += 360;
		} else {
			if (stopAngle < startAngle)
				stopAngle += 360;
		}

		FaderMoveSpec turnSpec;
		_turnPush.getCurrentFaderMove(turnSpec);

		FaderMoveSpec compassMove;
		compassMove.makeTwoKnotFaderSpec(turnSpec.getFaderScale(), turnSpec.getNthKnotTime(0), startAngle, turnSpec.getNthKnotTime(1), stopAngle);
		g_compass->startFader(compassMove);
	}

	_turnPushCallBack.cancelCallBack();
	_turnPush.continueFader();

	do {
		_vm->refreshDisplay();
		_vm->_system->delayMillis(10);
	} while (_turnPush.isFading());

	_turnPush.stopFader();
	_neighborhoodNotification.setNotificationFlags(kTurnCompletedFlag, kTurnCompletedFlag);
}

void Neighborhood::playExtraMovie(const ExtraTable::Entry &extraEntry, const tNotificationFlags flags, const tInputBits interruptionInput) {
	FaderMoveSpec compassMove;

	if (g_compass)
		getExtraCompassMove(extraEntry, compassMove);

	_lastExtra = extraEntry.extra;
	_turnPush.hide();
	startMovieSequence(extraEntry.movieStart, extraEntry.movieEnd, flags, false, interruptionInput);

	if (g_compass)
		g_compass->startFader(compassMove);
}

void Neighborhood::activateCurrentView(const tRoomID room, const tDirectionConstant direction, tSpotFlags flag) {
	SpotTable::Entry entry;
	findSpotEntry(room, direction, flag, entry);

	if (entry.dstFlags & flag) {
		startSpotOnceOnly(entry.movieStart, entry.movieEnd);
	} else {
		findSpotEntry(room, direction, flag | kSpotLoopsMask, entry);

		if (entry.dstFlags & flag)
			startSpotLoop(entry.movieStart, entry.movieEnd);
		else
			showViewFrame(getViewTime(room, direction));
	}
}

void Neighborhood::activateOneHotspot(HotspotInfoTable::Entry &entry, Hotspot *hotspot) {
	switch (_vm->getDragType()) {
	case kDragInventoryUse:
		if ((hotspot->getHotspotFlags() & kDropItemSpotFlag) != 0 &&
				_vm->getDraggingItem()->getObjectID() == entry.hotspotItem)
			hotspot->setActive();
		break;
	case kDragInventoryPickup:
	case kDragBiochipPickup:
		// Do nothing -- neighborhoods activate no hot spots in this case...
		break;
	default:
		if ((hotspot->getHotspotFlags() & kPickUpBiochipSpotFlag) != 0) {
			Item *item = g_allItems.findItemByID(entry.hotspotItem);
			if (item &&	item->getItemNeighborhood() == getObjectID())
				hotspot->setActive();
		} else {
			tHotSpotFlags flags = hotspot->getHotspotFlags();

			if ((flags & kNeighborhoodSpotFlag) != 0) {
				if (flags & kOpenDoorSpotFlag) {
					if (!GameState.isCurrentDoorOpen())
						hotspot->setActive();
				} else if ((flags & (kZoomSpotFlags | kClickSpotFlag | kPlayExtraSpotFlag)) != 0) {
					hotspot->setActive();
				} else if ((flags & kPickUpItemSpotFlag) != 0) {
					// Changed this 2/19/96
					// Should only light up this hot spot if the item's taken flag is not
					// set. It's not based on neighborhood ID since that can be reset by the
					// destroying process.

					if (!GameState.isTakenItemID(entry.hotspotItem))
						hotspot->setActive();
				}
			}
		}
		break;
	}
}

void Neighborhood::startSpotOnceOnly(TimeValue startTime, TimeValue stopTime) {
	_turnPush.hide();
	startMovieSequence(startTime, stopTime, kSpotCompletedFlag, kFilterNoInput, false);
}

void Neighborhood::startMovieSequence(const TimeValue startTime, const TimeValue stopTime, tNotificationFlags flags, bool loopSequence,
		const tInputBits interruptionInput, const TimeValue strideStop) {
	if (!loopSequence && g_AIArea)
		g_AIArea->lockAIOut();
	
	_interruptionFilter = interruptionInput;

	// Stop the movie before doing anything else
	_navMovie.stop();

	Common::Rect pushBounds;
	_turnPush.getBounds(pushBounds);

	_navMovie.moveElementTo(pushBounds.left, pushBounds.top);
	_navMovie.show();
	_navMovie.setFlags(0);
	_navMovie.setSegment(startTime, stopTime);
	_navMovie.setTime(startTime);

	if (loopSequence)
		_navMovie.setFlags(kLoopTimeBase);
	else
		flags |= kNeighborhoodMovieCompletedFlag;

	if (strideStop != 0xffffffff)
		// Subtract a little slop from the striding stop time to keep from "pumping" at the
		// end of a walk.
		// 40 is one frame (scale == 600, 15 fps).
		scheduleStridingCallBack(strideStop - kStridingSlop, flags);
	else
		scheduleNavCallBack(flags);

	_navMovie.start();
}

void Neighborhood::throwAwayInterface() {
	_doorTable.clear();
	_exitTable.clear();
	_extraTable.clear();
	_hotspotInfoTable.clear();
	_spotTable.clear();
	_turnTable.clear();
	_viewTable.clear();
	_zoomTable.clear();

	_navMovie.stopDisplaying();
	_navMovie.releaseMovie();
	_pushIn.deallocateSurface();
	_turnPush.stopDisplaying();
	_turnPush.setInAndOutElements(0, 0);
	_turnPush.disposeAllCallBacks();

	for (HotspotList::iterator it = _neighborhoodHotspots.begin(); it != _neighborhoodHotspots.end(); it++)
		g_allHotspots.remove(*it);

	_neighborhoodHotspots.deleteHotspots();
	_spotSounds.disposeSound();
	_delayTimer.disposeAllCallBacks();

	if (g_AIArea) {
		g_AIArea->saveAIState();
		g_AIArea->removeAllRules();
	}

	// TODO: Interaction

	_croppedMovie.releaseMovie();

	loadLoopSound1("");
	loadLoopSound2("");

	if (g_energyMonitor) {
		g_energyMonitor->stopEnergyDraining();
		g_energyMonitor->saveCurrentEnergyValue();
	}

	delete g_interface;
}

bool Neighborhood::prepareExtraSync(const tExtraID extraID) {
	ExtraTable::Entry extraEntry;
	FaderMoveSpec compassMove;
	
	if (g_compass) {
		getExtraEntry(extraID, extraEntry);
		getExtraCompassMove(extraEntry, compassMove);
	}

	ExtraTable::Entry entry;
	getExtraEntry(extraID, entry);
	bool result;

	if (entry.movieStart != 0xffffffff) {
		_turnPush.hide();

		// Stop the movie before doing anything else
		_navMovie.stop();

		Common::Rect pushBounds;
		_turnPush.getBounds(pushBounds);
		_navMovie.moveElementTo(pushBounds.left, pushBounds.top);

		_navMovie.show();
		_navMovie.setFlags(0);
		_navMovie.setSegment(entry.movieStart, entry.movieEnd);
		_navMovie.setTime(entry.movieStart);
		_navMovie.start();
		result = true;
	} else {
		result = false;
	}

	if (result && g_compass)
		g_compass->startFader(compassMove);

	return result;
}

bool Neighborhood::waitMovieFinish(Movie *movie, const tInputBits interruptionFilter) {
	Input input;
	bool result = true;
	bool saveAllowed = _vm->swapSaveAllowed(false);
	bool openAllowed = _vm->swapLoadAllowed(false);

	while (movie->isRunning()) {
		InputHandler::getCurrentInputDevice()->getInput(input, interruptionFilter);

		if (input.anyInput() || _vm->shouldQuit()) {
			result = false;
			break;
		}

		_vm->refreshDisplay();
		_vm->_system->delayMillis(10);
	}

	movie->stop();
	_vm->swapSaveAllowed(saveAllowed);
	_vm->swapLoadAllowed(openAllowed);

	return result;
}

tInputBits Neighborhood::getInputFilter() {
	return _interruptionFilter & InputHandler::getInputFilter();
}

void Neighborhood::getZoomCompassMove(const ZoomTable::Entry &zoomEntry, FaderMoveSpec &compassMove) {	
	int32 startAngle = getStaticCompassAngle(GameState.getCurrentRoom(), GameState.getCurrentDirection());
	int32 stopAngle = getStaticCompassAngle(zoomEntry.room, zoomEntry.direction);

	if (startAngle > stopAngle) {
		if (stopAngle + 180 < startAngle)
			stopAngle += 360;
	} else {
		if (startAngle + 180 < stopAngle)
			startAngle += 360;
	}

	compassMove.makeTwoKnotFaderSpec(_navMovie.getScale(), zoomEntry.movieStart, startAngle, zoomEntry.movieEnd, stopAngle);
}

void Neighborhood::getExtraCompassMove(const ExtraTable::Entry &, FaderMoveSpec &compassMove) {
	compassMove.makeOneKnotFaderSpec(g_compass->getFaderValue());
}

void Neighborhood::setUpAIRules() {
	// TODO
}

void Neighborhood::newInteraction(const tInteractionID interactionID) {
	// TODO
}

void Neighborhood::bumpIntoWall() {
	// TODO
}

void Neighborhood::zoomUpOrBump() {
	// TODO
}

void Neighborhood::loadLoopSound1(const Common::String &soundName, uint16 volume, TimeValue fadeOut, TimeValue fadeIn, TimeScale fadeScale) {
	// TODO
}

void Neighborhood::loadLoopSound2(const Common::String &soundName, uint16 volume, TimeValue fadeOut, TimeValue fadeIn, TimeScale fadeScale) {
	// TODO
}

void Neighborhood::takeItemFromRoom(Item *item) {
	item->setItemRoom(kNoNeighborhoodID, kNoRoomID, kNoDirection);
	// Also set the taken item flag. Do this before updating the view frame.
	GameState.setTakenItem(item, true);
	updateViewFrame();
}

} // End of namespace Pegasus
