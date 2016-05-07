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

#ifndef TITANIC_MESSAGES_H
#define TITANIC_MESSAGES_H

#include "common/keyboard.h"
#include "titanic/core/saveable_object.h"
#include "titanic/core/tree_item.h"

namespace Titanic {

enum MessageFlag { 
	MSGFLAG_SCAN = 1,
	MSGFLAG_BREAK_IF_HANDLED = 2,
	MSGFLAG_CLASS_DEF = 4
};

#define MESSAGE0(NAME) \
	class NAME: public CMessage { \
	public: NAME() : CMessage() {} \
	CLASSDEF \
	static bool isSupportedBy(const CTreeItem *item) { \
		return supports(item, _type); } \
}
#define MESSAGE1(NAME, F1, N1, V1) \
	class NAME: public CMessage { \
	public: F1 _##N1; \
	NAME() : CMessage(), _##N1(V1) {} \
	NAME(F1 N1) : CMessage(), _##N1(N1) {} \
	CLASSDEF \
	static bool isSupportedBy(const CTreeItem *item) { \
		return supports(item, _type); } \
}
#define MESSAGE2(NAME, F1, N1, V1, F2, N2, V2) \
	class NAME: public CMessage { \
	public: F1 _##N1; F2 _##N2; \
	NAME() : CMessage(), _##N1(V1), _##N2(V2) {} \
	NAME(F1 N1, F2 N2) : CMessage(), _##N1(N1), _##N2(N2) {} \
	CLASSDEF \
	static bool isSupportedBy(const CTreeItem *item) { \
		return supports(item, _type); } \
}
#define MESSAGE3(NAME, F1, N1, V1, F2, N2, V2, F3, N3, V3) \
	class NAME: public CMessage { \
	public: F1 _##N1; F2 _##N2; F3 _##N3; \
	NAME() : CMessage(), _##N1(V1), _##N2(V2), _##N3(V3) {} \
	NAME(F1 N1, F2 N2, F3 N3) : CMessage(), _##N1(N1), _##N2(N2), _##N3(N3) {} \
	CLASSDEF \
	static bool isSupportedBy(const CTreeItem *item) { \
		return supports(item, _type); } \
}
#define MESSAGE4(NAME, F1, N1, V1, F2, N2, V2, F3, N3, V3, F4, N4, V4) \
	class NAME: public CMessage { \
	public: F1 _##N1; F2 _##N2; F3 _##N3; F4 _##N4; \
	NAME() : CMessage(), _##N1(V1), _##N2(V2), _##N3(V3), _##N4(V4) {} \
	NAME(F1 N1, F2 N2, F3 N3, F4 N4) : CMessage(), _##N1(N1), _##N2(N2), _##N3(N3), _##N4(N4) {} \
	CLASSDEF \
	static bool isSupportedBy(const CTreeItem *item) { \
		return supports(item, _type); } \
}

class CCarry;
class CCharacter;
class CGameObject;
class CRoomItem;
class CNodeItem;
class CViewItem;

class CMessage : public CSaveableObject {
private:
	/**
	 * Find a map entry that supports the given class
	 */
	static const MSGMAP_ENTRY *findMapEntry(const CTreeItem *treeItem, const ClassDef *classDef);
public:
	CLASSDEF
	CMessage();

	/**
	 * Executes the message, passing it on to the designated target,
	 * and optionally it's children
	 */
	bool execute(CTreeItem *target, const ClassDef *classDef = nullptr,
		int flags = MSGFLAG_SCAN | MSGFLAG_BREAK_IF_HANDLED);

	/**
	 * Executes the message, passing it on to the designated target,
	 * and optionally it's children
	 */
	bool execute(const CString &target, const ClassDef *classDef = nullptr,
		int flags = MSGFLAG_SCAN | MSGFLAG_BREAK_IF_HANDLED);

	/**
	 * Makes the passed item execute the message
	 */
	virtual bool perform(CTreeItem *treeItem);

	/**
	 * Returns true if the passed item supports the specified message class
	 */
	static bool supports(const CTreeItem *treeItem, ClassDef *classDef);

	/**
	 * Save the data for the class to file
	 */
	virtual void save(SimpleFile *file, int indent) const;

	/**
	 * Load the data for the class from file
	 */
	virtual void load(SimpleFile *file);

	virtual bool isMouseMsg() const;
	virtual bool isButtonDownMsg() const;
	virtual bool isButtonUpMsg() const;
	virtual bool isMouseMoveMsg() const;
	virtual bool isDoubleClickMsg() const;
	virtual bool isEnterRoomMsg() const;
	virtual bool isPreEnterRoomMsg() const;
	virtual bool isleaveRoomMsg() const;
	virtual bool isEnterNodeMsg() const;
	virtual bool isPreEnterNodeMsg() const;
	virtual bool isLeaveNodeMsg() const;
	virtual bool isEnterViewMsg() const;
	virtual bool isPreEnterViewMsg() const;
	virtual bool isLeaveViewMsg() const;
};

class CEditControlMsg : public CMessage {
public:
	int _field4;
	int _field8;
	CString _string1;
	int _field18;
	int _field1C;
	int _field20;
public:
	CLASSDEF
	CEditControlMsg() : _field4(0), _field8(0), _field18(0),
		_field1C(0), _field20(0) {}

	static bool isSupportedBy(const CTreeItem *item) {
		return CMessage::supports(item, _type);
	}
};

class CLightsMsg : public CMessage {
public:
	int _field4;
	int _field8;
	int _fieldC;
	int _field10;
public:
	CLASSDEF
	CLightsMsg() : CMessage(), _field4(0), _field8(0),
		_fieldC(0), _field10(0) {}

	static bool isSupportedBy(const CTreeItem *item) {
		return supports(item, _type);
	}
};

MESSAGE1(CTimeMsg, uint, _ticks, 0);

class CTimerMsg : public CTimeMsg {
public:
	uint _timerCtr;
	int _val3;
	CString _action;
public:
	CLASSDEF
	CTimerMsg() : CTimeMsg(), _timerCtr(0), _val3(0) {}
	CTimerMsg(uint ticks, uint timerCtr, int val2, const CString &action) :
		CTimeMsg(ticks), _timerCtr(timerCtr), _val3(val2), _action(action) {}

	static bool isSupportedBy(const CTreeItem *item) {
		return supports(item, _type);
	}
};

MESSAGE1(CActMsg, CString, action, "");
MESSAGE1(CActivationmsg, CString, value, "");
MESSAGE1(CAddHeadPieceMsg, CString, value, "NULL");
MESSAGE1(CAnimateMaitreDMsg, int, value, 0);
MESSAGE1(CArboretumGateMsg, int, value, 0);
MESSAGE0(CArmPickedUpFromTableMsg);
MESSAGE0(CBodyInBilgeRoomMsg);
MESSAGE1(CBowlStateChange, int, value, 0);
MESSAGE2(CCarryObjectArrivedMsg, CString, strValue, "", int, numValue, 0);
MESSAGE2(CChangeMusicMsg, CString, strValue, "", int, numValue, 0);
MESSAGE1(CChangeSeasonMsg, CString, season, "Summer");
MESSAGE0(CCheckAllPossibleCodes);
MESSAGE2(CCheckChevCode, int, value1, 0, int, value2, 0);
MESSAGE1(CChildDragEndMsg, int, value, 0);
MESSAGE2(CChildDragMoveMsg, int, value1, 0, int, value2, 0);
MESSAGE2(CChildDragStartMsg, int, value1, 0, int, value2, 0);
MESSAGE0(CClearChevPanelBits);
MESSAGE0(CCorrectMusicPlayedMsg);
MESSAGE0(CCreateMusicPlayerMsg);
MESSAGE0(CCylinderHolderReadyMsg);
MESSAGE0(CDeactivationMsg);
MESSAGE1(CDeliverCCarryMsg, CString, value, "");
MESSAGE0(CDisableMaitreDProdReceptor);
MESSAGE0(CDismissBotMsg);
MESSAGE0(CDoffNavHelmet);
MESSAGE0(CDonNavHelmet);
MESSAGE1(CDoorbotNeededInElevatorMsg, int, value, 0);
MESSAGE0(CDoorbotNeededInHomeMsg);
MESSAGE1(CDropObjectMsg, CCarry *, item, nullptr);
MESSAGE1(CDropZoneGotObjectMsg, int, value, 0);
MESSAGE1(CDropZoneLostObjectMsg, int, value, 0);
MESSAGE1(CEjectCylinderMsg, int, value, 0);
MESSAGE2(CPreEnterNodeMsg, CNodeItem *, oldNode, nullptr, CNodeItem *, newNode, nullptr);
MESSAGE2(CPreEnterRoomMsg, CRoomItem *, oldRoom, nullptr, CRoomItem *, newRoom, nullptr);
MESSAGE2(CPreEnterViewMsg, CViewItem *, oldView, nullptr, CViewItem *, newView, nullptr);
MESSAGE2(CEnterNodeMsg, CNodeItem *, oldNode, nullptr, CNodeItem *, newNode, nullptr);
MESSAGE2(CEnterRoomMsg, CRoomItem *, oldRoom, nullptr, CRoomItem *, newRoom, nullptr);
MESSAGE2(CEnterViewMsg, CViewItem *, oldView, nullptr, CViewItem *, newView, nullptr);
MESSAGE0(CErasePhonographCylinderMsg);
MESSAGE1(CFrameMsg, uint, ticks, 0);
MESSAGE2(CFreshenCookieMsg, int, value1, 0, int, value2, 0);
MESSAGE1(CGetChevClassBits, int, value, 0);
MESSAGE1(CGetChevClassNum, int, value, 0);
MESSAGE2(CGetChevCodeFromRoomNameMsg, CString, strValue, "", int, numValue, 0);
MESSAGE1(CGetChevFloorBits, int, value, 0);
MESSAGE1(CGetChevFloorNum, int, value, 0);
MESSAGE1(CGetChevLiftBits, int, value, 0);
MESSAGE1(CGetChevLiftNum, int, value, 0);
MESSAGE1(CGetChevRoomBits, int, value, 0);
MESSAGE1(CGetChevRoomNum, int, value, 0);
MESSAGE2(CHoseConnectedMsg, int, value1, 1, int, value2, 0);
MESSAGE0(CInitializeAnimMsg);
MESSAGE1(CIsEarBowlPuzzleDone, int, value, 0);
MESSAGE3(CIsHookedOnMsg, Rect, rect, Rect(), bool, result, false, CString, string1, "");
MESSAGE1(CIsParrotPresentMsg, bool, value, false);
MESSAGE1(CKeyCharMsg, int, key, 32);
MESSAGE2(CLeaveNodeMsg, CNodeItem *, oldNode, nullptr, CNodeItem *, newNode, nullptr);
MESSAGE2(CLeaveRoomMsg, CRoomItem *, oldRoom, nullptr, CRoomItem *, newRoom, nullptr);
MESSAGE2(CLeaveViewMsg, CViewItem *, oldView, nullptr, CViewItem *, newView, nullptr);
MESSAGE2(CLemonFallsFromTreeMsg, int, value1, 0, int, value2, 0);
MESSAGE1(CLoadSuccessMsg, int, ticks, 0);
MESSAGE1(CLockPhonographMsg, int, value, 0);
MESSAGE0(CMaitreDDefeatedMsg);
MESSAGE0(CMaitreDHappyMsg);
MESSAGE1(CMissiveOMatActionMsg, int, value, 0);
MESSAGE0(CMoveToStartPosMsg);
MESSAGE2(CMovieEndMsg, int, value1, 0, int, value2, 0);
MESSAGE2(CMovieFrameMsg, int, value1, 0, int, value2, 0);
MESSAGE0(CMusicHasStartedMsg);
MESSAGE0(CMusicHasStoppedMsg);
MESSAGE0(CMusicSettingChangedMsg);
MESSAGE2(CNPCPlayAnimationMsg, void *, data, nullptr, int, value2, 0);
MESSAGE1(CNPCPlayIdleAnimationMsg, void *, value, 0);
MESSAGE3(CNPCPlayTalkingAnimationMsg, int, value1, 0, int, value2, 0, void *, value3, nullptr);
MESSAGE0(CNPCQueueIdleAnimMsg);
MESSAGE1(CNutPuzzleMsg, CString, value, "");
MESSAGE1(COnSummonBotMsg, int, value, 0);
MESSAGE0(COpeningCreditsMsg);
MESSAGE1(CPanningAwayFromParrotMsg, int, value, 0);
MESSAGE2(CParrotSpeakMsg, CString, value1, "", CString, value2, "");
MESSAGE2(CParrotTriesChickenMsg, int, value1, 0, int, value2, 0);
MESSAGE1(CPhonographPlayMsg, int, value, 0);
MESSAGE0(CPhonographReadyToPlayMsg);
MESSAGE1(CPhonographRecordMsg, int, value, 0);
MESSAGE3(CPhonographStopMsg, int, value1, 0, int, value2, 0, int, value3, 0);
MESSAGE2(CPlayRangeMsg, int, value1, 0, int, value2, 0);
MESSAGE2(CPlayerTriesRestaurantTableMsg, int, value1, 0, int, value2, 0);
MESSAGE1(CPreSaveMsg, int, value, 0);
MESSAGE1(CProdMaitreDMsg, int, value, 0);
MESSAGE2(CPumpingMsg, int, value1, 0, int, value2, 0);
MESSAGE1(CPutBotBackInHisBoxMsg, int, value, 0);
MESSAGE1(CPutParrotBackMsg, int, value, 0);
MESSAGE0(CPuzzleSolvedMsg);
MESSAGE3(CQueryCylinderHolderMsg, int, value1, 0, int, value2, 0, int, value3, 0);
MESSAGE1(CQueryCylinderMsg, CString, name, "");
MESSAGE1(CQueryCylinderNameMsg, CString, name, "");
MESSAGE3(CQueryCylinderTypeMsg, int, value1, 0, int, value2, 0, int, value3, 0);
MESSAGE1(CQueryMusicControlSettingMsg, int, value, 0);
MESSAGE1(CQueryPhonographState, int, value, 0);
MESSAGE0(CRecordOntoCylinderMsg);
MESSAGE0(CRemoveFromGameMsg);
MESSAGE0(CReplaceBowlAndNutsMsg);
MESSAGE1(CRestaurantMusicChanged, CString, value, "");
MESSAGE2(CSendCCarryMsg, CString, strValue, "", int, numValue, 0);
MESSAGE1(CSenseWorkingMsg, CString, value, "Not Working");
MESSAGE2(CServiceElevatorFloorChangeMsg, int, value1, 0, int, value2, 0);
MESSAGE0(CServiceElevatorFloorRequestMsg);
MESSAGE1(CServiceElevatorMsg, int, value, 4);
MESSAGE2(CSetChevButtonImageMsg, int, value1, 0, int, value2, 0);
MESSAGE1(CSetChevClassBits, int, value, 0);
MESSAGE1(CSetChevFloorBits, int, value, 0);
MESSAGE1(CSetChevLiftBits, int, value, 0);
MESSAGE2(CSetChevPanelBitMsg, int, value1, 0, int, value2, 0);
MESSAGE1(CSetChevPanelButtonsMsg, int, value, 0);
MESSAGE1(CSetChevRoomBits, int, value, 0);
MESSAGE1(CSetFrameMsg, int, frameNumber, 0);
MESSAGE0(CSetMusicControlsMsg);
MESSAGE2(CSetVarMsg, CString, varName, "", int, value, 0);
MESSAGE2(CSetVolumeMsg, int, value1, 70, int, value2, 0);
MESSAGE2(CShipSettingMsg, int, value, 0, CString, name, "");
MESSAGE1(CShowTextMsg, CString, value, "NO TEXT INCLUDED!!!");
MESSAGE2(CSignalObject, CString, strValue, "", int, numValue, 0);
MESSAGE2(CSpeechFallsFromTreeMsg, int, value1, 0, int, value2, 0);
MESSAGE1(CStartMusicMsg, int, value, 0);
MESSAGE3(CStatusChangeMsg, int, oldStatus, 0, int, newStatus, 0, bool, success, false);
MESSAGE1(CStopMusicMsg, int, value, 0);
MESSAGE4(CSubAcceptCCarryMsg, CString, string1, "", int, value1, 0, int, value2, 0, CCarry *, item, nullptr);
MESSAGE0(CSubDeliverCCarryMsg);
MESSAGE0(CSubSendCCarryMsg);
MESSAGE0(CSUBTransition);
MESSAGE0(CSubTurnOffMsg);
MESSAGE0(CSubTurnOnMsg);
MESSAGE2(CSummonBotMsg, CString, strValue, "", int, numValue, 0);
MESSAGE1(CSummonBotQueryMsg, CString, npcName, "");
MESSAGE1(CTakeHeadPieceMsg, CString, value, "NULL");
MESSAGE2(CTextInputMsg, CString, input, "", CString, response, "");
MESSAGE1(CTimeDilationMsg, int, value, 0);
MESSAGE0(CTitleSequenceEndedMsg);
MESSAGE0(CTransitMsg);
MESSAGE1(CTranslateObjectMsg, Point, delta, Point());
MESSAGE3(CTransportMsg, CString, roomName, "", int, value1, 0, int, value2, 0);
MESSAGE1(CTriggerAutoMusicPlayerMsg, int, value, 0);
MESSAGE1(CTriggerNPCEvent, int, value, 0);
MESSAGE4(CTrueTalkGetAnimSetMsg, int, value1, 0, int, value2, 0, int, value3, 0, int, value4, 0);
MESSAGE2(CTrueTalkGetAssetDetailsMsg, CString, filename, "", int, numValue, 0);
MESSAGE2(CTrueTalkGetStateValueMsg, int, value1, 0, int, value2, -1000);
MESSAGE2(CTrueTalkNotifySpeechEndedMsg, int, value1, 0, int, value2, 0);
MESSAGE3(CTrueTalkNotifySpeechStartedMsg, int, value1, 0, int, value2, 0, int, value, 0);
MESSAGE1(CTrueTalkQueueUpAnimSetMsg, int, value, 0);
MESSAGE0(CTrueTalkSelfQueueAnimSetMsg);
MESSAGE3(CTrueTalkTriggerActionMsg, int, value1, 0, int, value2, 0, int, value3, 0);
MESSAGE0(CTurnOff);
MESSAGE0(CTurnOn);
MESSAGE1(CUse, CCarry *, item, nullptr);
MESSAGE1(CUseWithCharMsg, CCharacter *, character, nullptr);
MESSAGE1(CUseWithOtherMsg, CGameObject *, other, 0);
MESSAGE1(CVirtualKeyCharMsg, Common::KeyState, keyState, Common::KeyState());
MESSAGE1(CVisibleMsg, bool, visible, true);

} // End of namespace Titanic

#endif /* TITANIC_MESSAGE_H */
