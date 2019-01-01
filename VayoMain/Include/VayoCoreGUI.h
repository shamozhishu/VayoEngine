/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* GUI核心
\*************************************************************************/
#ifndef __VAYO_CORE_GUI_H__
#define __VAYO_CORE_GUI_H__

#include "VayoSupport.h"
#include "VayoInput.h"
#include "VayoTexture.h"
#include "VayoSharedPtr.h"
#include "VayoRectangle.h"
#include "VayoColour.h"
#include "VayoBitState.h"
#include "VayoBaseGUI.h"
#include "VayoSigslot.h"

NS_VAYO_BEGIN

// 前置声明
class UIDialog;

/** 字体 */
class _VayoExport UIFont
{
public:
	UIFont(unsigned int fontID)
		: _fontID(fontID)
		, _kerningSize(0, 2) {}
	~UIFont() {}
	void         renderText(const wstring& text, const Recti& rc, Colour clr, EUIAlignment hori, EUIAlignment vert, float scale = 1.0f, const Recti* clip = NULL);
	void         drawTextBegin();
	void         drawText(int x, int y, const wstring& text, float scale = 1.0f, unsigned color = 0xffffffff);
	void         drawTextEnd();
	int          getFontSize();
	Dimension2di getTextSize(const wstring& text);
	Dimension2di getCharSize(wchar_t ch, bool hasKerning = false);

private:
	PROPERTY_RW_REF(Dimension2di, _kerningSize, KerningSize)
private:
	unsigned int _fontID;
	struct tagTexBatchGroup
	{
		vector<Recti>  DestRects;
		vector<Recti>  SourceRects;
		vector<Colour> Colors;
	};
	map<TexturePtr, tagTexBatchGroup> _texBatches;
};

/** 图元集: 通过一张贴图创建,该贴图里面包含了很多小区域 */
class _VayoExport UIImageSet
{
public:
	UIImageSet();
	~UIImageSet();
	static UIImageSet* createImgset(const wstring& filepath);
	bool               init(const wstring& filepath);
	const Recti&       getIconRect(const wstring& iconName);
	Dimension2di       getImageSize() const { return Dimension2di(_imageWidth, _imageHeight); }
	const wstring&     getImagePath() const { return _imagePath; }
	TexturePtr         getBindTexture() const { return _bindTexture; }

private:
	int                 _imageWidth;
	int                 _imageHeight;
	wstring             _imagePath;
	map<wstring, Recti> _icons;
	TexturePtr          _bindTexture;
};

/** 窗口小部件: 所有控件页面都是一个Widget */
class _VayoExport UIWidget : public AttribSerializer
{
public:
	UIWidget();
	virtual ~UIWidget();
	virtual void render();

	virtual bool isVisible()     { return _visible; }
	virtual bool isActive()      { return _active; }
	virtual bool isFocus()       { return _focus; }
	virtual bool isCanBeActive() { return _canBeActive; }

	virtual void showWidget(bool bShow);
	virtual void setActive(bool bActive);
	virtual void setFocus(bool bFocus);
	virtual void setCanBeActive(bool bCanBeActive);
	virtual void moveTo(int x, int y);
	virtual void moveBy(int x, int y);

	virtual void setRectArea(int x, int y, int width, int height);
	virtual void updateAbsoluteRect() = 0;

	virtual void touchBegan(const Touch& touch);
	virtual void touchMoved(const Touch& touch);
	virtual void touchEnded(const Touch& touch);
	virtual bool touchWheel(const Touch& touch, float wheel) { return false; }
	virtual bool keyClicked(const tagKeyInput& keyInput);

	PROPERTY_R_REF(Recti, _relativeRect, RelativeRect)
	PROPERTY_R_REF(Recti, _absoluteRect, AbsoluteRect)
	PROPERTY_R_REF(Recti, _animatedRelativeRect, AnimatedRelativeRect)

	PROPERTY_RW(int,        _imgsetID,     ImgsetID)
	PROPERTY_RW_REF(Colour, _imageColor,   ImageColor)
	PROPERTY_RW_REF(Colour, _outlineColor, OutlineColor)
	DISALLOW_COPY_AND_ASSIGN(UIWidget)

private:
	bool           _visible;
	bool           _active;
	bool           _activeMark;
	bool           _canBeActive;
	bool           _focus;
	BitState       _rcLocation;
};

/** 控件基类 */
class _VayoExport UIControl : public UIWidget
{
public:
	UIControl();
	virtual ~UIControl();
	virtual void render();
	virtual void setActive(bool bActive);
	virtual void updateAbsoluteRect();
	virtual const wstring& getImage(EUIControlStatus status) const;
	virtual void setImage(EUIControlStatus status, const wstring& iconName);
	virtual void serialize(XMLElement* outXml);
	virtual bool deserialize(XMLElement* inXml);

	PROPERTY_RW(int,              _ID,            ID)
	PROPERTY_RW(int,              _fontID,        FontID)
	PROPERTY_RW(UIDialog*,        _parent,        Parent)
	PROPERTY_RW(EUIControlStatus, _status,        Status)
	PROPERTY_RW(EUIAlignment,     _textAlignHori, TextAlignHori)
	PROPERTY_RW(EUIAlignment,     _textAlignVert, TextAlignVert)
	PROPERTY_RW(float,            _textScale,     TextScale)
	PROPERTY_RW_REF(Colour,       _textColor,     TextColor)
	PROPERTY_RW_REF(wstring,      _text,          Text)

protected:
	string  _textID;
	wstring _imageArr[EUICS_COUNT];
};

/** 按钮 */
class _VayoExport UIButton : public UIControl
{
	VAYO_REFLEX_DECLARE(UIButton)
public:
	UIButton();
	virtual ~UIButton();
	virtual void render();
	virtual bool isClicked();
	virtual bool isHover() const;
	virtual void touchBegan(const Touch& touch);
	virtual void touchMoved(const Touch& touch);
	virtual void touchEnded(const Touch& touch);
	virtual void serialize(XMLElement* outXml);
	virtual bool deserialize(XMLElement* inXml);

private:
	bool _click;
	bool _hover;

signals:
	Signal _buttonClicked;
};

/** 复选框 */
class _VayoExport UICheckBox : public UIControl
{
	VAYO_REFLEX_DECLARE(UICheckBox)
public:
	UICheckBox();
	virtual ~UICheckBox();
	virtual void render();
	virtual bool isChecked() const;
	virtual void setChecked(bool checked);
	virtual void touchBegan(const Touch& touch);
	virtual void touchMoved(const Touch& touch);
	virtual void touchEnded(const Touch& touch);
	virtual void serialize(XMLElement* outXml);
	virtual bool deserialize(XMLElement* inXml);

private:
	bool         _checked;
	bool         _pressed;

signals:
	Signal _checkBoxChanged;
};

/** 静态文本 */
class _VayoExport UIStatic : public UIControl
{
	VAYO_REFLEX_DECLARE(UIStatic)
public:
	UIStatic();
	virtual ~UIStatic();
	virtual void render();
	virtual void setText(const wstring& text);
	virtual int  getTextWidth() const;
	virtual int  getTextHeight() const;
	virtual void updateAbsoluteRect();
	virtual void setWordWrap(bool enable);
	virtual void setBgColor(Colour color);
	virtual void serialize(XMLElement* outXml);
	virtual bool deserialize(XMLElement* inXml);

private:
	void breakText();
	void breakLine(vector<wstring>& outLines, const wstring& inLine);

private:
	PROPERTY_R(Colour, _bgColor,        BgColor)
	PROPERTY_R(bool,   _wordWrap,       WordWrap)
	PROPERTY_RW(bool,  _border,         Border)
	PROPERTY_RW(bool,  _drawBg,         DrawBg)
	PROPERTY_RW(bool,  _clipText,       ClipText)
	PROPERTY_RW(bool,  _bgColorEnabled, BgColorEnabled)

private:
	int             _lastFontID;
	vector<wstring> _brokenText;
};

/** 滚动条 */
class _VayoExport UIScrollBar : public UIControl
{
	VAYO_REFLEX_DECLARE(UIScrollBar)
public:
	UIScrollBar();
	virtual ~UIScrollBar();
	virtual void render();
	virtual void updateAbsoluteRect();
	virtual void setPos(int pos);
	virtual void setMin(int minVal);
	virtual void setMax(int maxVal);
	virtual void setSmallStep(int step);
	virtual void setLargeStep(int step);
	virtual void touchBegan(const Touch& touch);
	virtual void touchMoved(const Touch& touch);
	virtual void touchEnded(const Touch& touch);
	virtual bool touchWheel(const Touch& touch, float wheel);
	virtual void serialize(XMLElement* outXml);
	virtual bool deserialize(XMLElement* inXml);

private:
	void  refreshControls();
	int   getPosFromMousePt(const Position2di& pt) const;
	float range() const { return (float)(_max - _min); }

private:
	PROPERTY_R(int,  _pos,        Pos)
	PROPERTY_R(int,  _min,        Min)
	PROPERTY_R(int,  _max,        Max)
	PROPERTY_R(int,  _smallStep,  SmallStep)
	PROPERTY_R(int,  _largeStep,  LargeStep)
	PROPERTY_R(bool, _horizontal, Horizontal)
	PROPERTY_RW_REF(wstring, _sliderIconName, SliderIconName)

private:
	UIButton* _upButton;
	UIButton* _downButton;
	Recti     _sliderRect;
	bool      _dragging;
	bool      _draggedBySlider;
	bool      _trayClick;
	int       _drawPos;
	int       _drawHeight;
	int       _desiredPos;
	unsigned  _timeGap;

signals:
	Signal _scrollBarChanged;
};

/** 列表框 */
class _VayoExport UIListBox : public UIControl
{
	VAYO_REFLEX_DECLARE(UIListBox)
public:
	UIListBox();
	virtual ~UIListBox();
	virtual void render();
	virtual void clear();
	virtual void updateAbsoluteRect();
	virtual void touchBegan(const Touch& touch);
	virtual void touchMoved(const Touch& touch);
	virtual void touchEnded(const Touch& touch);
	virtual bool touchWheel(const Touch& touch, float wheel);
	virtual void serialize(XMLElement* outXml);
	virtual bool deserialize(XMLElement* inXml);
	virtual void setItemHeight(int height);
	virtual void setDrawBg(bool drawBg);
	virtual int  getItemCount() const;
	virtual int  getSelected() const;
	virtual void setSelected(int id);
	virtual void setSelected(const wstring& item);
	virtual int  getIcon(unsigned int id) const;
	virtual void removeItem(unsigned int id);
	virtual int  getItemAt(int xpos, int ypos) const;
	virtual const wchar_t* getListItem(unsigned int id) const;
	virtual unsigned int   addItem(const wstring& text);
	virtual unsigned int   addItem(const wstring& text, int icon);
	virtual void   setItem(unsigned int index, const wstring& text, int icon);
	virtual void   insertItem(unsigned int index, const wstring& text, int icon);
	virtual void   swapItems(unsigned int index1, unsigned int index2);
	virtual void   setItemColor(unsigned int index, Colour color);
	virtual void   setItemColor(unsigned int index, EUIListBoxColor colorType, Colour color);
	virtual void   clearItemColor(unsigned int index);
	virtual void   clearItemColor(unsigned int index, EUIListBoxColor colorType);
	virtual bool   hasItemColor(unsigned int index, EUIListBoxColor colorType) const;
	virtual Colour getItemColor(unsigned int index, EUIListBoxColor colorType) const;
	virtual Colour getItemDefaultColor(EUIListBoxColor colorType) const;

private:
	typedef struct tagListItem
	{
		tagListItem() : Icon(-1) {}
		typedef struct tagListItemColor
		{
			tagListItemColor() : Use(false) {}
			bool                 Use;
			Colour               Color;
		} ListItemColor;
		ListItemColor  Colors[EUILBC_COUNT];
		wstring        Text;
		int            Icon;
	} ListItem;

	void selectNew(int ypos, bool onlyHover = false);
	void recalculateScrollPos();
	void recalculateItemWidth(int icon);
	void recalculateItemHeight();

	vector<ListItem> _items;
	int              _selected;
	int              _itemHeight;
	bool             _itemHeightOverride;
	int              _totalItemHeight;
	int              _itemsIconWidth;
	UIScrollBar*     _scrollBar;
	unsigned int     _selectTime;
	unsigned int     _lastKeyTime;
	wstring          _keyBuffer;
	bool             _selecting;
	bool             _drawBg;
	bool             _highlightWhenNotFocused;

signals:
	Signal _listBoxChanged;
	Signal _listBoxSelectedAgain;
private:
	PROPERTY_RW(bool, _autoScroll,     AutoScroll)
	PROPERTY_RW(bool, _moveOverSelect, MoveOverSelect)
};

/** 组合框 */
class _VayoExport UIComboBox : public UIControl
{
	VAYO_REFLEX_DECLARE(UIComboBox)
public:
	UIComboBox();
	virtual ~UIComboBox();
	virtual void render();

private:
	UIButton* ListButton;
	UIStatic* SelectedText;
	UIListBox* ListBox;
	UIControl* LastFocus;

	struct SComboData
	{
		SComboData(const wchar_t * text, unsigned int data)
			: Name(text), Data(data) {}

		wstring Name;
		unsigned int Data;
	};

	vector<SComboData> Items;
	int Selected;
	EUIAlignment HAlign, VAlign;
	unsigned int MaxSelectionRows;
	bool HasFocus;
};

/** 对话框: 它包含各种类型的控件 */
class _VayoExport UIDialog : public UIWidget
{
	static Reflex<UIDialog, const wstring&> _dynReflex;
public:
	UIDialog(const wstring& fileName);
	virtual ~UIDialog();
	virtual void open(bool bBlockMode=true);
	virtual void close();
	virtual void render();
	virtual void handleInput();
	virtual void center(bool bXPos=true, bool bYPos=true);
	virtual UIControl* getCtrl(int id) const;
	virtual bool removeCtrl(UIControl* pCtrl);
	virtual bool removeCtrl(int id);
	virtual void setFocus(bool bFocus);
	virtual void updateAbsoluteRect();
	virtual void touchBegan(const Touch& touch);
	virtual void touchMoved(const Touch& touch);
	virtual void touchEnded(const Touch& touch);
	virtual bool touchWheel(const Touch& touch, float wheel);
	virtual bool keyClicked(const tagKeyInput& keyInput);
	virtual void serialize(XMLElement* outXml);
	virtual bool deserialize(XMLElement* inXml);

	PROPERTY_R(UIControl*,           _lastSelectedCtrl, LastSelectedCtrl)
	PROPERTY_R_REF(list<UIControl*>, _controls,         CtrlList)
	PROPERTY_R_REF(wstring,          _xmlFilePath,      XMLFilePath)
	PROPERTY_RW_REF(wstring,         _image,            Image)
};

/**
 * UI管理器: 负责整个图形用户界面主循环,包括渲染和事件,以及一些界面的自绘接口
**/
class _VayoExport UIManager : public TouchDelegate, public KeypadDelegate
{
public:
	UIManager(Device* dev);
	virtual ~UIManager();
	bool                init();
	void                render();
	bool                isEditMode() const;
	bool                addDialog(UIDialog* pDlg);
	bool                removeDialog(UIDialog* pDlg);
	void                beginPopup(UIDialog* pDlg);
	void                endPopup(UIDialog* pDlg);
	UIDialog*           getPopupDialog();
	UIFont*             getFont(int fontID);
	UIImageSet*         getImageSet(int imgsetID);
	UISkin*             getSkin() const;
	void                setSkin(UISkin* skin);
	void                showAllDialog(bool bShow);
	void                notifyDlgCancelSelectedExcept(UIDialog* pDlg);
	Recti*              getClipAreaPointer() const { return _clipArea; }
	virtual bool        touchBegan(const Touch& touch, EMouseKeys key);
	virtual void        touchMoved(const Touch& touch, EMouseKeys key);
	virtual void        touchEnded(const Touch& touch, EMouseKeys key);
	virtual bool        touchWheel(const Touch& touch, float wheel);
	virtual bool        keyClicked(const tagKeyInput& keyInput);
	
	PROPERTY_R_REF(list<UIDialog*>, _dialogs, DlgList)
	DISALLOW_COPY_AND_ASSIGN(UIManager)
private:
	void allotTouch(ETouchType touchType, const Touch& touch);
	void confirmWhichSelected(UIDialog* pDialog, const Touch& touch);

private:
	deque<UIDialog*> _popupDlgs;
	BitState         _UIStateMark;
	UIImageSet*      _curDrawImage;
	UIImageSet*      _imageSetArr[VAYO_MAX_IMAGESET_NUM];
	UIFont*          _fontArr[EUIFT_COUNT];
	UISkin*          _currentSkin;
	Recti*           _clipArea;
	vector<Recti>    _destRects;
	vector<Recti>    _sourceRects;
	vector<Colour>   _colors;

	struct tagAnim
	{
		struct tagKeyFrame
		{
			int           SrcImageSetID;
			wstring       IconName;
			float         XOffset;
			float         YOffset;
			float         Scale;
			bool          ScaleByCenter;
			int           Time;
		};

		int AnimID;
		vector<tagKeyFrame> ArrKeyFrame;
	};
	vector<tagAnim> _arrAnimSet;

public:
	struct tagSequecenAnim
	{
		int AnimID;          // 动画ID
		int CurTime;		 // 当前动画时间
		int Times;	         // 当前播放次数
		tagSequecenAnim()
		{
			AnimID = -1;
			CurTime = Times = 0;
		}
	};

	//------------------------------------------------------------------------
	// 绘制相关
	//------------------------------------------------------------------------

	// 绘制图标
	void drawIconBegin(int nImgSetID);
	void drawIcon(const wstring& iconName, const Recti& drawArea, const Colour* color=NULL);
	void drawIconEnd();

	// 裁剪
	void beginClipRender(Recti* clipArea);
	void endClipRender();

	// 绘制序列帧动画
	void drawSequenceAnim(float x, float y, tagSequecenAnim& seqAnim, float scale=1.0f, unsigned color=0xffffffff);
	void drawSequenceAnim(float x, float y, float width, float height, tagSequecenAnim& seqAnim, unsigned color=0xffffffff);
};

NS_VAYO_END

#endif // __VAYO_CORE_GUI_H__
