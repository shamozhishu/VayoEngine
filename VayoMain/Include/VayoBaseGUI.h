/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 authored by 朱加浩
* GUI基础
\*************************************************************************/
#ifndef __VAYO_BASE_GUI_H__
#define __VAYO_BASE_GUI_H__

#include "VayoSupport.h"
#include "VayoColour.h"
#include "VayoRectangle.h"
#include "VayoAttribSerializer.h"

#define VAYO_MAX_IMAGESET_NUM (500)

NS_VAYO_BEGIN

enum EUIControlStatus
{
	EUICS_NORMAL,
	EUICS_PUSH,
	EUICS_DISABLE,
	EUICS_COUNT
};

enum EUIAlignment
{
	EUIA_LEFT_OR_UP,
	EUIA_CENTER,
	EUIA_RIGHT_OR_DOWN
};

enum EUIFontType
{
	EUIFT_DEFAULT = 0,
	EUIFT_TOOLTIP,
	EUIFT_BUTTON,
	EUIFT_WINDOW,
	EUIFT_MENU,
	EUIFT_COUNT
};

enum EUIListBoxColor
{
	EUILBC_TEXT = 0,
	EUILBC_TEXT_HIGHLIGHT,
	EUILBC_ICON,
	EUILBC_ICON_HIGHLIGHT,
	EUILBC_COUNT
};

enum EUIColor
{
	EUIC_DARK_SHADOW_3D = 0,
	EUIC_SHADOW_3D,
	EUIC_FACE_3D,
	EUIC_HIGH_LIGHT_3D,
	EUIC_LIGHT_3D,
	EUIC_ACTIVE_BORDER,
	EUIC_ACTIVE_CAPTION,
	EUIC_APP_WORKSPACE,
	EUIC_BUTTON_TEXT,
	EUIC_GRAY_TEXT,
	EUIC_HIGH_LIGHT,
	EUIC_HIGH_LIGHT_TEXT,
	EUIC_INACTIVE_BORDER,
	EUIC_INACTIVE_CAPTION,
	EUIC_TOOLTIP,
	EUIC_TOOLTIP_BACKGROUND,
	EUIC_SCROLLBAR,
	EUIC_WINDOW,
	EUIC_WINDOW_SYMBOL,
	EUIC_ICON,
	EUIC_ICON_HIGH_LIGHT,
	EUIC_GRAY_WINDOW_SYMBOL,
	EUIC_EDITABLE,
	EUIC_GRAY_EDITABLE,
	EUIC_FOCUSED_EDITABLE,
	EUIC_COUNT
};

enum EUISize
{
	EUIS_SCROLLBAR_SIZE = 0,
	EUIS_MENU_HEIGHT,
	EUIS_WINDOW_BUTTON_WIDTH,
	EUIS_CHECK_BOX_WIDTH,
	EUIS_MESSAGE_BOX_WIDTH,
	EUIS_MESSAGE_BOX_HEIGHT,
	EUIS_BUTTON_WIDTH,
	EUIS_BUTTON_HEIGHT,
	EUIS_TEXT_DISTANCE_X,
	EUIS_TEXT_DISTANCE_Y,
	EUIS_TITLEBARTEXT_DISTANCE_X,
	EUIS_TITLEBARTEXT_DISTANCE_Y,
	EUIS_MESSAGE_BOX_GAP_SPACE,
	EUIS_MESSAGE_BOX_MIN_TEXT_WIDTH,
	EUIS_MESSAGE_BOX_MAX_TEXT_WIDTH,
	EUIS_MESSAGE_BOX_MIN_TEXT_HEIGHT,
	EUIS_MESSAGE_BOX_MAX_TEXT_HEIGHT,
	EUIS_BUTTON_PRESSED_IMAGE_OFFSET_X,
	EUIS_BUTTON_PRESSED_IMAGE_OFFSET_Y,
	EUIS_BUTTON_PRESSED_TEXT_OFFSET_X,
	EUIS_BUTTON_PRESSED_TEXT_OFFSET_Y,
	EUIS_COUNT
};

enum EUIText
{
	EUIT_MSG_BOX_OK = 0,
	EUIT_MSG_BOX_CANCEL,
	EUIT_MSG_BOX_YES,
	EUIT_MSG_BOX_NO,
	EUIT_WINDOW_CLOSE,
	EUIT_WINDOW_MAXIMIZE,
	EUIT_WINDOW_MINIMIZE,
	EUIT_WINDOW_RESTORE,
	EUIT_COUNT
};

enum EUIIcon
{
	EUII_WINDOW_MAXIMIZE = 0,
	EUII_WINDOW_RESTORE,
	EUII_WINDOW_CLOSE,
	EUII_WINDOW_MINIMIZE,
	EUII_WINDOW_RESIZE,
	EUII_CURSOR_UP,
	EUII_CURSOR_DOWN,
	EUII_CURSOR_LEFT,
	EUII_CURSOR_RIGHT,
	EUII_MENU_MORE,
	EUII_CHECK_BOX_CHECKED,
	EUII_DROP_DOWN,
	EUII_SMALL_CURSOR_UP,
	EUII_SMALL_CURSOR_DOWN,
	EUII_RADIO_BUTTON_CHECKED,
	EUII_MORE_LEFT,
	EUII_MORE_RIGHT,
	EUII_MORE_UP,
	EUII_MORE_DOWN,
	EUII_EXPAND,
	EUII_COLLAPSE,
	EUII_FILE,
	EUII_DIRECTORY,
	EUII_COUNT
};

class _VayoExport UISkin
{
public:
	UISkin(const wstring& skinFilePath);
	virtual ~UISkin();
	virtual Colour         getColor(EUIColor color) const;
	virtual void           setColor(EUIColor which, Colour newColor);
	virtual int            getSize(EUISize size) const;
	virtual void           setSize(EUISize which, int size);
	virtual unsigned int   getIcon(EUIIcon icon) const;
	virtual void           setIcon(EUIIcon icon, unsigned int index);
	virtual const wchar_t* getText(EUIText text) const;
	virtual void           setText(EUIText which, const wchar_t* newText);

	virtual void  draw2DRect(const Colour &color, const Recti& pos, const Recti* clip = NULL);
	virtual void  draw3DButtonPaneStandard(const Recti& rect, const Recti* clip = NULL);
	virtual void  draw3DButtonPanePressed(const Recti& rect, const Recti* clip = NULL);
	virtual void  draw3DSunkenPane(Colour bgcolor, bool flat, bool fillBackGround, const Recti& rect, const Recti* clip = NULL);
	virtual Recti draw3DWindowBackground(bool drawTitleBar, Colour titleBarColor, const Recti& rect, const Recti* clip, Recti* checkClientArea);
	virtual void  draw3DMenuPane(const Recti& rect, const Recti* clip = NULL);
	virtual void  draw3DToolBar(const Recti& rect, const Recti* clip = NULL);
	virtual void  draw3DTabButton(bool active, const Recti& rect, const Recti* clip = NULL, EUIAlignment alignment = EUIA_LEFT_OR_UP);
	virtual void  draw3DTabBody(bool border, bool background, const Recti& rect, const Recti* clip = 0, int tabHeight = -1, EUIAlignment alignment = EUIA_LEFT_OR_UP);

private:
	RenderSystem* _renderer;
	bool          _useGradient;
	Colour        _colors[EUIC_COUNT];
	int           _sizes[EUIS_COUNT];
	wstring       _texts[EUIT_COUNT];
	unsigned int  _icons[EUII_COUNT];
};

NS_VAYO_END

#endif // __VAYO_BASE_GUI_H__
