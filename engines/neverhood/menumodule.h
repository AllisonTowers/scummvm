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

#ifndef NEVERHOOD_MENUMODULE_H
#define NEVERHOOD_MENUMODULE_H

#include "common/str.h"
#include "neverhood/neverhood.h"
#include "neverhood/module.h"
#include "neverhood/scene.h"

namespace Neverhood {

typedef Common::Array<Common::String> StringArray;

class MenuModule : public Module {
public:
	MenuModule(NeverhoodEngine *vm, Module *parentModule, int which);
	virtual ~MenuModule();
protected:
	int _sceneNum;
	Common::String _savegameName;
	byte *_savedPaletteData;
	StringArray *_savegameList;
	void createScene(int sceneNum, int which);
	void updateScene();
	uint32 handleMessage(int messageNum, const MessageParam &param, Entity *sender);
	void createSaveGameMenu();
	void handleSaveGameMenuAction(int action);
};

class MenuButton : public StaticSprite {
public:
	MenuButton(NeverhoodEngine *vm, Scene *parentScene, uint buttonIndex, uint32 fileHash, const NRect &collisionBounds);
protected:
	Scene *_parentScene;
	int _countdown;
	uint _buttonIndex;
	void update();
	uint32 handleMessage(int messageNum, const MessageParam &param, Entity *sender);
};

class MainMenu : public Scene {
public:
	MainMenu(NeverhoodEngine *vm, Module *parentModule);
protected:
	Sprite *_musicOnButton;
	uint32 handleMessage(int messageNum, const MessageParam &param, Entity *sender);
};

class CreditsScene : public Scene {
public:
	CreditsScene(NeverhoodEngine *vm, Module *parentModule, bool canAbort);
	virtual ~CreditsScene();
protected:
	int _screenIndex;
	int _countdown;
	MusicResource *_musicResource;
	uint32 _ticksTime;
	uint32 _ticksDuration;
	bool _canAbort;
	void update();
	uint32 handleMessage(int messageNum, const MessageParam &param, Entity *sender);
};

class Widget;

class WidgetScene : public Scene {
public:
	WidgetScene(NeverhoodEngine *vm, Module *parentModule);
	NPoint getMousePos();
	virtual void setCurrWidget(Widget *newWidget);
	virtual Widget *getCurrWidget() { return _currWidget; }
	virtual void handleEvent(int16 itemID, int eventType);
protected:
	Widget *_currWidget;
};

class Widget : public StaticSprite {
public:
	Widget(NeverhoodEngine *vm, int16 x, int16 y, int16 itemID, WidgetScene *parentScene,
		int baseObjectPriority, int baseSurfacePriority);
	virtual void onClick();
	virtual void setPosition(int16 x, int16 y);
	virtual void refreshPosition();
	virtual void addSprite();
	virtual int16 getWidth();
	virtual int16 getHeight();
	virtual void enterWidget();
	virtual void exitWidget();
protected:
	int16 _itemID;
	WidgetScene *_parentScene;
	int _baseObjectPriority;
	int _baseSurfacePriority;
	void update();
	uint32 handleMessage(int messageNum, const MessageParam &param, Entity *sender);
};

class TextLabelWidget : public Widget {
public:
	TextLabelWidget(NeverhoodEngine *vm, int16 x, int16 y, int16 itemID, WidgetScene *parentScene,
		int baseObjectPriority, int baseSurfacePriority, 
		const byte *string, int stringLen, BaseSurface *drawSurface, int16 tx, int16 ty, TextSurface *textSurface);	
	virtual void onClick();
	virtual void addSprite();
	virtual int16 getWidth();
	virtual int16 getHeight();
	void drawString(int maxStringLength);
	void clear();
	void setString(const byte *string, int stringLen);
	TextSurface *getTextSurface() const { return _textSurface; }
	void setTY(int16 ty);
protected:
	BaseSurface *_drawSurface;
	int16 _tx, _ty;
	TextSurface *_textSurface;
	const byte *_string;
	int _stringLen;
};

class TextEditWidget : public Widget {
public:
	TextEditWidget(NeverhoodEngine *vm, int16 x, int16 y, int16 itemID, WidgetScene *parentScene,
		int baseObjectPriority, int baseSurfacePriority,
		const byte *string, int maxStringLength, TextSurface *textSurface, uint32 fileHash, const NRect &rect);
	~TextEditWidget();
	virtual void onClick();
	virtual void addSprite();
	virtual void enterWidget();
	virtual void exitWidget();
	void setCursor(uint32 cursorFileHash, int16 cursorWidth, int16 cursorHeight);
	void drawCursor();
	void updateString();
	void getString(Common::String &string);
	void setString(const Common::String &string);
	void handleAsciiKey(char ch);
	void handleKeyDown(Common::KeyCode keyCode);
	void refresh();
protected:
	NRect _rect;
	uint32 _fileHash;
	int _maxVisibleChars;
	int _maxStringLength;
	int _cursorPos;
	int _cursorTicks;
	Common::String _entryString;
	TextSurface *_textSurface;
	TextLabelWidget *_textLabelWidget;
	BaseSurface *_cursorSurface;
	uint32 _cursorFileHash;
	int16 _cursorWidth, _cursorHeight;
	void update();
	uint32 handleMessage(int messageNum, const MessageParam &param, Entity *sender);
};

class SavegameListBox : public Widget {
public:
	SavegameListBox(NeverhoodEngine *vm, int16 x, int16 y, int16 itemID, WidgetScene *parentScene,
		int baseObjectPriority, int baseSurfacePriority,
		StringArray *savegameList, TextSurface *textSurface, uint32 bgFileHash, const NRect &rect);
	virtual void onClick();
	virtual void addSprite();
	void buildItems();
	void drawItems();
	void refresh();
	void scrollUp();
	void scrollDown();
	void pageUp();
	void pageDown();
	uint getCurrIndex() const { return _currIndex; }
protected:
	const NRect _rect;
	uint32 _bgFileHash;
	int _maxStringLength;
	Common::Array<TextLabelWidget*> _textLabelItems;
	int _firstVisibleItem;
	int _lastVisibleItem;
	StringArray *_savegameList;
	TextSurface *_textSurface;
	uint _currIndex;
	int _maxVisibleItemsCount;
};

class SaveGameMenu : public WidgetScene {
public:
	SaveGameMenu(NeverhoodEngine *vm, Module *parentModule, StringArray *savegameList);
	~SaveGameMenu();
	virtual void handleEvent(int16 itemID, int eventType);
protected:
	StringArray *_savegameList;
	TextSurface *_textSurface;
	SavegameListBox *_listBox;
	TextEditWidget *_textEditWidget;
	Common::String _savegameName;
	void update();
	uint32 handleMessage(int messageNum, const MessageParam &param, Entity *sender);
};

} // End of namespace Neverhood

#endif /* NEVERHOOD_MENUMODULE_H */
