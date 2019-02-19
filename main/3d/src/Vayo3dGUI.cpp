#include "Vayo3dGUI.h"
#include "VayoConfigManager.h"
#include "VayoDevice.h"
#include "Vayo3dRenderSystem.h"
#include "VayoLanguage.h"
#include "Vayo3dRoot.h"
#include "VayoLog.h"
#include "Vayo3dTextureManager.h"
#include "Vayo3dFontManager.h"

NS_VAYO3D_BEGIN

static FontManager* s_fontManager = NULL;
const char* const GUISkinColorNames[EUIC_COUNT + 1] =
{
	"DarkShadow3D",
	"Shadow3D",
	"Face3D",
	"Highlight3D",
	"Light3D",
	"ActiveBorder",
	"ActiveCaption",
	"AppWorkspace",
	"ButtonText",
	"GrayText",
	"Highlight",
	"HighlightText",
	"InactiveBorder",
	"InactiveCaption",
	"ToolTip",
	"ToolTipBackground",
	"ScrollBar",
	"Window",
	"WindowSymbol",
	"Icon",
	"IconHighlight",
	"GrayWindowSymbol",
	"Editable",
	"GrayEditable",
	"FocusedEditable",
	NULL
};

const char* const GUISkinSizeNames[EUIS_COUNT + 1] =
{
	"ScrollBarSize",
	"MenuHeight",
	"WindowButtonWidth",
	"CheckBoxWidth",
	"MessageBoxWidth",
	"MessageBoxHeight",
	"ButtonWidth",
	"ButtonHeight",
	"TextDistanceX",
	"TextDistanceY",
	"TitleBarTextX",
	"TitleBarTextY",
	"MessageBoxGapSpace",
	"MessageBoxMinTextWidth",
	"MessageBoxMaxTextWidth",
	"MessageBoxMinTextHeight",
	"MessageBoxMaxTextHeight",
	"ButtonPressedImageOffsetX",
	"ButtonPressedImageOffsetY",
	"ButtonPressedTextOffsetX",
	"ButtonPressedTextOffsetY",
	NULL
};

const char* const GUISkinTextNames[EUIT_COUNT + 1] =
{
	"MessageBoxOkay",
	"MessageBoxCancel",
	"MessageBoxYes",
	"MessageBoxNo",
	"WindowButtonClose",
	"WindowButtonMaximize",
	"WindowButtonMinimize",
	"WindowButtonRestore",
	NULL
};

const char* const GUISkinIconNames[EUII_COUNT + 1] =
{
	"WindowMaximize",
	"WindowRestore",
	"WindowClose",
	"WindowMinimize",
	"WindowResize",
	"CursorUp",
	"CursorDown",
	"CursorLeft",
	"CursorRight",
	"MenuMore",
	"CheckBoxChecked",
	"DropDown",
	"SmallCursorUp",
	"SmallCursorDown",
	"RadioButtonChecked",
	"MoreLeft",
	"MoreRight",
	"MoreUp",
	"MoreDown",
	"Expand",
	"Collapse",
	"File",
	"Directory",
	NULL
};

UISkin::UISkin(const wstring& skinFilePath)
	: _renderer(Root::getSingleton().getActiveRenderer())
	, _useGradient(false)
{
	if (skinFilePath.empty())
	{
		Log::wprint(ELL_ERROR, L"GUI皮肤文件路径为空");
		return;
	}

	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(w2a_(skinFilePath).c_str()) != tinyxml2::XML_SUCCESS)
	{
		Log::wprint(ELL_ERROR, L"GUI皮肤[%s]加载失败", skinFilePath.c_str());
		return;
	}

	XMLElement* pRoot = doc.RootElement();
	if (NULL == pRoot)
	{
		Log::wprint(ELL_ERROR, L"GUI皮肤[%s]解析失败", skinFilePath.c_str());
		return;
	}

	_useGradient = pRoot->BoolAttribute("UseGradient");
	XMLElement* pElem = pRoot->FirstChildElement("colors");
	if (pElem)
	{
		XMLElement* pColor = pElem->FirstChildElement("color");
		int idx = 0;
		while (pColor && idx < EUIC_COUNT)
		{
			_colors[idx].set(strtoul(pColor->Attribute(GUISkinColorNames[idx]), NULL, 16));
			++idx;
			pColor = pColor->NextSiblingElement();
		}
	}

	pElem = pRoot->FirstChildElement("sizes");
	if (pElem)
	{
		XMLElement* pSize = pElem->FirstChildElement("size");
		int idx = 0;
		while (pSize && idx < EUIS_COUNT)
		{
			_sizes[idx] = pSize->IntAttribute(GUISkinSizeNames[idx]);
			++idx;
			pSize = pSize->NextSiblingElement();
		}
	}

	pElem = pRoot->FirstChildElement("texts");
	if (pElem)
	{
		XMLElement* pText = pElem->FirstChildElement("text");
		int idx = 0;
		while (pText && idx < EUIT_COUNT)
		{
			_texts[idx] = utf8ToUnicode(pText->Attribute(GUISkinTextNames[idx]));
			++idx;
			pText = pText->NextSiblingElement();
		}
	}

	pElem = pRoot->FirstChildElement("icons");
	if (pElem)
	{
		XMLElement* pIcon = pElem->FirstChildElement("icon");
		int idx = 0;
		while (pIcon && idx < EUII_COUNT)
		{
			_icons[idx] = pIcon->IntAttribute(GUISkinIconNames[idx]);
			++idx;
			pIcon = pIcon->NextSiblingElement();
		}
	}
}

UISkin::~UISkin()
{
}

Colour UISkin::getColor(EUIColor color) const
{
	if ((unsigned int)color < EUIC_COUNT)
		return _colors[color];
	else
		return Colour();
}

void UISkin::setColor(EUIColor which, Colour newColor)
{
	if ((unsigned int)which < EUIC_COUNT)
		_colors[which] = newColor;
}

int UISkin::getSize(EUISize size) const
{
	if ((unsigned int)size < EUIS_COUNT)
		return _sizes[size];
	else
		return 0;
}

void UISkin::setSize(EUISize which, int size)
{
	if ((unsigned int)which < EUIS_COUNT)
		_sizes[which] = size;
}

unsigned int UISkin::getIcon(EUIIcon icon) const
{
	if ((unsigned int)icon < EUII_COUNT)
		return _icons[icon];
	else
		return 0;
}

void UISkin::setIcon(EUIIcon icon, unsigned int index)
{
	if ((unsigned int)icon < EUII_COUNT)
		_icons[icon] = index;
}

const wchar_t* UISkin::getText(EUIText text) const
{
	if ((unsigned int)text < EUIT_COUNT)
		return _texts[text].c_str();
	else
		return _texts[0].c_str();
}

void UISkin::setText(EUIText which, const wchar_t* newText)
{
	if ((unsigned int)which < EUIT_COUNT)
		_texts[which] = newText;
}

void UISkin::draw2DRect(const Colour &color, const Recti& pos, const Recti* clip /*= NULL*/)
{
	_renderer->draw2DRect(color, pos, clip);
}

void UISkin::draw3DButtonPaneStandard(const Recti& rect, const Recti* clip /*= NULL*/)
{
	if (!_renderer)
		return;

	Recti rc = rect;
	rc._upperLeftCorner._x -= 1;
	rc._upperLeftCorner._y -= 1;
	rc._lowerRightCorner._x += 1;
	rc._lowerRightCorner._y += 1;
	draw3DSunkenPane(getColor(EUIC_WINDOW).getInterpolated(0xFFFFFFFF, 0.9f), false, true, rc, clip);
}

void UISkin::draw3DButtonPanePressed(const Recti& rect, const Recti* clip /*= NULL*/)
{
	if (!_renderer)
		return;

	Recti rc = rect;
	_renderer->draw2DRect(getColor(EUIC_HIGH_LIGHT_3D), rc, clip);

	rc._lowerRightCorner._x -= 1;
	rc._lowerRightCorner._y -= 1;
	_renderer->draw2DRect(getColor(EUIC_DARK_SHADOW_3D), rc, clip);

	rc._upperLeftCorner._x += 1;
	rc._upperLeftCorner._y += 1;
	_renderer->draw2DRect(getColor(EUIC_SHADOW_3D), rc, clip);

	rc._upperLeftCorner._x += 1;
	rc._upperLeftCorner._y += 1;

	if (!_useGradient)
	{
		_renderer->draw2DRect(getColor(EUIC_FACE_3D), rc, clip);
	}
	else
	{
		const Colour c1 = getColor(EUIC_FACE_3D);
		const Colour c2 = c1.getInterpolated(getColor(EUIC_DARK_SHADOW_3D), 0.4f);
		_renderer->draw2DRect(rc, c1, c1, c2, c2, clip);
	}
}

void UISkin::draw3DSunkenPane(Colour bgcolor, bool flat, bool fillBackGround, const Recti& rect, const Recti* clip /*= NULL*/)
{
	if (!_renderer)
		return;

	Recti rc = rect;

	if (fillBackGround)
		_renderer->draw2DRect(bgcolor, rc, clip);

	if (flat)
	{
		// draw flat sunken pane
		rc._lowerRightCorner._y = rc._upperLeftCorner._y + 1;
		_renderer->draw2DRect(getColor(EUIC_SHADOW_3D), rc, clip); // top

		++rc._upperLeftCorner._y;
		rc._lowerRightCorner._y = rect._lowerRightCorner._y;
		rc._lowerRightCorner._x = rc._upperLeftCorner._x + 1;
		_renderer->draw2DRect(getColor(EUIC_SHADOW_3D), rc, clip); // left

		rc = rect;
		++rc._upperLeftCorner._y;
		rc._upperLeftCorner._x = rc._lowerRightCorner._x - 1;
		_renderer->draw2DRect(getColor(EUIC_HIGH_LIGHT_3D), rc, clip); // right

		rc = rect;
		++rc._upperLeftCorner._x;
		rc._upperLeftCorner._y = rect._lowerRightCorner._y - 1;
		--rc._lowerRightCorner._x;
		_renderer->draw2DRect(getColor(EUIC_HIGH_LIGHT_3D), rc, clip); // bottom
	}
	else
	{
		// draw deep sunken pane
		rc._lowerRightCorner._y = rc._upperLeftCorner._y + 1;
		_renderer->draw2DRect(getColor(EUIC_SHADOW_3D), rc, clip); // top
		++rc._upperLeftCorner._x;
		++rc._upperLeftCorner._y;
		--rc._lowerRightCorner._x;
		++rc._lowerRightCorner._y;
		_renderer->draw2DRect(getColor(EUIC_DARK_SHADOW_3D), rc, clip);

		rc._upperLeftCorner._x = rect._upperLeftCorner._x;
		rc._upperLeftCorner._y = rect._upperLeftCorner._y + 1;
		rc._lowerRightCorner._x = rc._upperLeftCorner._x + 1;
		rc._lowerRightCorner._y = rect._lowerRightCorner._y;
		_renderer->draw2DRect(getColor(EUIC_SHADOW_3D), rc, clip); // left
		++rc._upperLeftCorner._x;
		++rc._upperLeftCorner._y;
		++rc._lowerRightCorner._x;
		--rc._lowerRightCorner._y;
		_renderer->draw2DRect(getColor(EUIC_DARK_SHADOW_3D), rc, clip);

		rc = rect;
		rc._upperLeftCorner._x = rc._lowerRightCorner._x - 1;
		++rc._upperLeftCorner._y;
		_renderer->draw2DRect(getColor(EUIC_HIGH_LIGHT_3D), rc, clip); // right
		--rc._upperLeftCorner._x;
		++rc._upperLeftCorner._y;
		--rc._lowerRightCorner._x;
		--rc._lowerRightCorner._y;
		_renderer->draw2DRect(getColor(EUIC_LIGHT_3D), rc, clip);

		rc = rect;
		++rc._upperLeftCorner._x;
		rc._upperLeftCorner._y = rect._lowerRightCorner._y - 1;
		--rc._lowerRightCorner._x;
		_renderer->draw2DRect(getColor(EUIC_HIGH_LIGHT_3D), rc, clip); // bottom
		++rc._upperLeftCorner._x;
		--rc._upperLeftCorner._y;
		--rc._lowerRightCorner._x;
		--rc._lowerRightCorner._y;
		_renderer->draw2DRect(getColor(EUIC_LIGHT_3D), rc, clip);
	}
}

Recti UISkin::draw3DWindowBackground(bool drawTitleBar, Colour titleBarColor, const Recti& rect, const Recti* clip, Recti* checkClientArea)
{
	if (!_renderer)
	{
		if (checkClientArea)
			*checkClientArea = rect;
		return rect;
	}

	Recti rc = rect;

	// top border
	rc._lowerRightCorner._y = rc._upperLeftCorner._y + 1;
	if (!checkClientArea)
	{
		_renderer->draw2DRect(getColor(EUIC_HIGH_LIGHT_3D), rc, clip);
	}

	// left border
	rc._lowerRightCorner._y = rect._lowerRightCorner._y;
	rc._lowerRightCorner._x = rc._upperLeftCorner._x + 1;
	if (!checkClientArea)
	{
		_renderer->draw2DRect(getColor(EUIC_HIGH_LIGHT_3D), rc, clip);
	}

	// right border dark outer line
	rc._upperLeftCorner._x = rect._lowerRightCorner._x - 1;
	rc._lowerRightCorner._x = rect._lowerRightCorner._x;
	rc._upperLeftCorner._y = rect._upperLeftCorner._y;
	rc._lowerRightCorner._y = rect._lowerRightCorner._y;
	if (!checkClientArea)
	{
		_renderer->draw2DRect(getColor(EUIC_DARK_SHADOW_3D), rc, clip);
	}

	// right border bright innner line
	rc._upperLeftCorner._x -= 1;
	rc._lowerRightCorner._x -= 1;
	rc._upperLeftCorner._y += 1;
	rc._lowerRightCorner._y -= 1;
	if (!checkClientArea)
	{
		_renderer->draw2DRect(getColor(EUIC_SHADOW_3D), rc, clip);
	}

	// bottom border dark outer line
	rc._upperLeftCorner._x = rect._upperLeftCorner._x;
	rc._upperLeftCorner._y = rect._lowerRightCorner._y - 1;
	rc._lowerRightCorner._y = rect._lowerRightCorner._y;
	rc._lowerRightCorner._x = rect._lowerRightCorner._x;
	if (!checkClientArea)
	{
		_renderer->draw2DRect(getColor(EUIC_DARK_SHADOW_3D), rc, clip);
	}

	// bottom border bright inner line
	rc._upperLeftCorner._x += 1;
	rc._lowerRightCorner._x -= 1;
	rc._upperLeftCorner._y -= 1;
	rc._lowerRightCorner._y -= 1;
	if (!checkClientArea)
	{
		_renderer->draw2DRect(getColor(EUIC_SHADOW_3D), rc, clip);
	}

	// client area for background
	rc = rect;
	rc._upperLeftCorner._x += 1;
	rc._upperLeftCorner._y += 1;
	rc._lowerRightCorner._x -= 2;
	rc._lowerRightCorner._y -= 2;
	if (checkClientArea)
	{
		*checkClientArea = rc;
	}

	if (!checkClientArea)
	{
		if (!_useGradient)
		{
			_renderer->draw2DRect(getColor(EUIC_FACE_3D), rc, clip);
		}
		else
		{
			const Colour c1 = getColor(EUIC_WINDOW).getInterpolated(0xFFFFFFFF, 0.9f);
			const Colour c2 = getColor(EUIC_WINDOW).getInterpolated(0xFFFFFFFF, 0.8f);

			_renderer->draw2DRect(rc, c1, c1, c2, c2, clip);
		}
	}

	// title bar
	rc = rect;
	rc._upperLeftCorner._x += 2;
	rc._upperLeftCorner._y += 2;
	rc._lowerRightCorner._x -= 2;
	rc._lowerRightCorner._y = rc._upperLeftCorner._y + getSize(EUIS_WINDOW_BUTTON_WIDTH) + 2;

	if (drawTitleBar)
	{
		if (checkClientArea)
		{
			(*checkClientArea)._upperLeftCorner._y = rc._lowerRightCorner._y;
		}
		else
		{
			// draw title bar
			const Colour c = titleBarColor.getInterpolated(Colour(titleBarColor.getAlpha(), 255, 255, 255), 0.8f);
			_renderer->draw2DRect(rc, titleBarColor, titleBarColor, c, c, clip);
		}
	}

	return rc;
}

void UISkin::draw3DMenuPane(const Recti& rect, const Recti* clip /*= NULL*/)
{
	if (!_renderer)
		return;

	Recti rc = rect;
	rc._upperLeftCorner._y -= 3;
	draw3DButtonPaneStandard(rc, clip);
}

void UISkin::draw3DToolBar(const Recti& rect, const Recti* clip /*= NULL*/)
{
	if (!_renderer)
		return;

	Recti rc = rect;

	rc._upperLeftCorner._x = rect._upperLeftCorner._x;
	rc._upperLeftCorner._y = rect._lowerRightCorner._y - 1;
	rc._lowerRightCorner._y = rect._lowerRightCorner._y;
	rc._lowerRightCorner._x = rect._lowerRightCorner._x;
	_renderer->draw2DRect(getColor(EUIC_SHADOW_3D), rc, clip);

	rc = rect;
	rc._lowerRightCorner._y -= 1;

	if (!_useGradient)
	{
		_renderer->draw2DRect(getColor(EUIC_FACE_3D), rc, clip);
	}
	else
	{
		const Colour c1 = 0xF0000000 | getColor(EUIC_FACE_3D)._clr;
		const Colour c2 = 0xF0000000 | getColor(EUIC_SHADOW_3D)._clr;
		rc._lowerRightCorner._y += 1;
		_renderer->draw2DRect(rc, c1, c2, c1, c2, clip);
	}
}

void UISkin::draw3DTabButton(bool active, const Recti& rect, const Recti* clip /*= NULL*/, EUIAlignment alignment /*= EUIA_LEFT_OR_UP*/)
{
	if (!_renderer)
		return;

	Recti rc = rect;

	if (alignment == EUIA_LEFT_OR_UP)
	{
		rc._lowerRightCorner._x -= 2;
		rc._lowerRightCorner._y = rc._upperLeftCorner._y + 1;
		rc._upperLeftCorner._x += 1;
		_renderer->draw2DRect(getColor(EUIC_HIGH_LIGHT_3D), rc, clip);

		// draw left highlight
		rc = rect;
		rc._lowerRightCorner._x = rc._upperLeftCorner._x + 1;
		rc._upperLeftCorner._y += 1;
		_renderer->draw2DRect(getColor(EUIC_HIGH_LIGHT_3D), rc, clip);

		// draw grey background
		rc = rect;
		rc._upperLeftCorner._x += 1;
		rc._upperLeftCorner._y += 1;
		rc._lowerRightCorner._x -= 2;
		_renderer->draw2DRect(getColor(EUIC_FACE_3D), rc, clip);

		// draw right middle gray shadow
		rc._lowerRightCorner._x += 1;
		rc._upperLeftCorner._x = rc._lowerRightCorner._x - 1;
		_renderer->draw2DRect(getColor(EUIC_SHADOW_3D), rc, clip);

		rc._lowerRightCorner._x += 1;
		rc._upperLeftCorner._x += 1;
		rc._upperLeftCorner._y += 1;
		_renderer->draw2DRect(getColor(EUIC_DARK_SHADOW_3D), rc, clip);
	}
	else
	{
		rc._lowerRightCorner._x -= 2;
		rc._upperLeftCorner._y = rc._lowerRightCorner._y - 1;
		rc._upperLeftCorner._x += 1;
		_renderer->draw2DRect(getColor(EUIC_HIGH_LIGHT_3D), rc, clip);

		// draw left highlight
		rc = rect;
		rc._lowerRightCorner._x = rc._upperLeftCorner._x + 1;
		rc._lowerRightCorner._y -= 1;
		_renderer->draw2DRect(getColor(EUIC_HIGH_LIGHT_3D), rc, clip);

		// draw grey background
		rc = rect;
		rc._upperLeftCorner._x += 1;
		rc._upperLeftCorner._y -= 1;
		rc._lowerRightCorner._x -= 2;
		rc._lowerRightCorner._y -= 1;
		_renderer->draw2DRect(getColor(EUIC_FACE_3D), rc, clip);

		// draw right middle gray shadow
		rc._lowerRightCorner._x += 1;
		rc._upperLeftCorner._x = rc._lowerRightCorner._x - 1;
		_renderer->draw2DRect(getColor(EUIC_SHADOW_3D), rc, clip);

		rc._lowerRightCorner._x += 1;
		rc._upperLeftCorner._x += 1;
		rc._lowerRightCorner._y -= 1;
		_renderer->draw2DRect(getColor(EUIC_DARK_SHADOW_3D), rc, clip);
	}
}

void UISkin::draw3DTabBody(bool border, bool background, const Recti& rect, const Recti* clip /*= 0*/, int tabHeight /*= -1*/, EUIAlignment alignment /*= EUIA_LEFT_OR_UP*/)
{
	if (!_renderer)
		return;

	Recti rc = rect;

	if (tabHeight == -1)
		tabHeight = getSize(EUIS_BUTTON_HEIGHT);

	// draw border.
	if (border)
	{
		if (alignment == EUIA_LEFT_OR_UP)
		{
			// draw left hightlight
			rc._upperLeftCorner._y += tabHeight + 2;
			rc._lowerRightCorner._x = rc._upperLeftCorner._x + 1;
			_renderer->draw2DRect(getColor(EUIC_HIGH_LIGHT_3D), rc, clip);

			// draw right shadow
			rc._upperLeftCorner._x = rect._lowerRightCorner._x - 1;
			rc._lowerRightCorner._x = rc._upperLeftCorner._x + 1;
			_renderer->draw2DRect(getColor(EUIC_SHADOW_3D), rc, clip);

			// draw lower shadow
			rc = rect;
			rc._upperLeftCorner._y = rc._lowerRightCorner._y - 1;
			_renderer->draw2DRect(getColor(EUIC_SHADOW_3D), rc, clip);
		}
		else
		{
			// draw left hightlight
			rc._lowerRightCorner._y -= tabHeight + 2;
			rc._lowerRightCorner._x = rc._upperLeftCorner._x + 1;
			_renderer->draw2DRect(getColor(EUIC_HIGH_LIGHT_3D), rc, clip);

			// draw right shadow
			rc._upperLeftCorner._x = rect._lowerRightCorner._x - 1;
			rc._lowerRightCorner._x = rc._upperLeftCorner._x + 1;
			_renderer->draw2DRect(getColor(EUIC_SHADOW_3D), rc, clip);

			// draw lower shadow
			rc = rect;
			rc._lowerRightCorner._y = rc._upperLeftCorner._y + 1;
			_renderer->draw2DRect(getColor(EUIC_HIGH_LIGHT_3D), rc, clip);
		}
	}

	if (background)
	{
		if (alignment == EUIA_LEFT_OR_UP)
		{
			rc = rect;
			rc._upperLeftCorner._y += tabHeight + 2;
			rc._lowerRightCorner._x -= 1;
			rc._upperLeftCorner._x += 1;
			rc._lowerRightCorner._y -= 1;
		}
		else
		{
			rc = rect;
			rc._upperLeftCorner._x += 1;
			rc._upperLeftCorner._y -= 1;
			rc._lowerRightCorner._x -= 1;
			rc._lowerRightCorner._y -= tabHeight + 2;
		}

		if (!_useGradient)
			_renderer->draw2DRect(getColor(EUIC_FACE_3D), rc, clip);
		else
		{
			Colour c1 = getColor(EUIC_FACE_3D);
			Colour c2 = getColor(EUIC_SHADOW_3D);
			_renderer->draw2DRect(rc, c1, c1, c2, c2, clip);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void UIFont::renderText(const wstring& text, const Recti& rc, Colour clr, EUIAlignment hori, EUIAlignment vert, float scale /*= 1.0f*/, const Recti* clip /*= NULL*/)
{
	if (text.empty())
		return;

	UIManager* pUIMgr = Root::getSingleton().getUIManager();
	UIFont* pFont = pUIMgr->getFont(_fontID);
	int x = rc._upperLeftCorner._x;
	int y = rc._upperLeftCorner._y;
	int w = rc.getWidth();
	int h = rc.getHeight();

	Dimension2di textSize = pFont->getTextSize(text);
	textSize._width *= scale;
	textSize._height *= scale;

	switch (hori)
	{
	case EUIA_LEFT_OR_UP: break;
	case EUIA_CENTER: x += (w - textSize._width) / 2; break;
	case EUIA_RIGHT_OR_DOWN: x += (w - textSize._width); break;
	}

	const int vertOffset = 3;
	switch (vert)
	{
	case EUIA_LEFT_OR_UP: break;
	case EUIA_CENTER: y += (h - textSize._height) / 2; break;
	case EUIA_RIGHT_OR_DOWN: y += (h - textSize._height - vertOffset); break;
	}

	pUIMgr->beginClipRender(const_cast<Recti*>(clip));
	pFont->drawTextBegin();
	pFont->drawText(x, y, text, scale, clr._clr);
	pFont->drawTextEnd();
	pUIMgr->endClipRender();
}

void UIFont::drawTextBegin()
{
	_texBatches.clear();
}

void UIFont::drawText(int x, int y, const wstring& text, float scale /*= 1.0f*/, unsigned color /*= 0xffffffff*/)
{
	if (_fontID < 0)
		return;

	TexturePtr selTex;
	int tall = s_fontManager->getFontTall(_fontID);
	int dx = x;
	int dy = y + tall * scale;
	bool lineBreak = false;

	const wchar_t* szText = text.c_str();
	while (*szText)
	{
		if (*szText == L'\r' || *szText == L'\n')
		{
			++szText;

			if (!lineBreak)
			{
				lineBreak = true;
				dx = x;
				dy += tall * scale + _kerningSize._height; // row spacing
			}
			
			continue;
		}

		lineBreak = false;
		int cw, ct, bx, by, av;
		TexturePtr curTex;
		float crd[4];

		if (!s_fontManager->getCharInfo(_fontID, *szText, &cw, &ct, &bx, &by, &av, curTex, crd))
		{
			++szText;
			continue;
		}

		int px = dx + bx * scale;
		int py = dy - by * scale;
		Recti dstRect(px, py, px + cw * scale, py + ct * scale);
		Recti srcRect(crd[0] * TRUE_TYPE_FONT_TEXTURE_SIZE, crd[1] * TRUE_TYPE_FONT_TEXTURE_SIZE, crd[2] * TRUE_TYPE_FONT_TEXTURE_SIZE, crd[3] * TRUE_TYPE_FONT_TEXTURE_SIZE);

		// 大多数情况下多个字符都在同一个纹理中，避免频繁绑定纹理，可以提高效率
		tagTexBatchGroup batch;
		if (curTex != selTex)
		{
			selTex = curTex;
			_texBatches.insert(make_pair(selTex, batch));
		}

		map<TexturePtr, tagTexBatchGroup>::iterator it = _texBatches.find(selTex);
		if (it != _texBatches.end())
		{
			tagTexBatchGroup& batch = it->second;
			batch.DestRects.push_back(dstRect);
			batch.SourceRects.push_back(srcRect);
			batch.Colors.push_back(color);
		}

		dx += av * scale + _kerningSize._width;
		++szText;
	}
}

void UIFont::drawTextEnd()
{
	UIManager* uiMgr = Root::getSingleton().getUIManager();
	RenderSystem* pRenderer = Root::getSingleton().getActiveRenderer();
	map<TexturePtr, tagTexBatchGroup>::iterator it = _texBatches.begin();
	for (; it != _texBatches.end(); ++it)
	{
		tagTexBatchGroup& texBatch = it->second;
		pRenderer->draw2DImageBatch(it->first, texBatch.DestRects, texBatch.SourceRects,
			uiMgr->getClipAreaPointer(), &texBatch.Colors, true);
	}

	_texBatches.clear();
}

int UIFont::getFontSize()
{
	return s_fontManager->getFontTall(_fontID);
}

Dimension2di UIFont::getTextSize(const wstring& text)
{
	if (_fontID < 0)
		return Dimension2di();

	int tall = s_fontManager->getFontTall(_fontID);
	int totalWidth = 0;
	int totalHeight = tall;
	int keepWidth = 0;
	const wchar_t* szText = text.c_str();
	bool lineBreak = false;

	while (*szText)
	{
		if (*szText == L'\r' || *szText == L'\n')
		{
			++szText;

			if (!lineBreak)
			{
				lineBreak = true;
				totalHeight += tall + _kerningSize._height;
				keepWidth -= _kerningSize._width;
				if (keepWidth > totalWidth)
					totalWidth = keepWidth;
				keepWidth = 0;
			}

			continue;
		}

		lineBreak = false;
		int cw, ct, bx, by, av;
		TexturePtr curTex;
		float crd[4];

		if (!s_fontManager->getCharInfo(_fontID, *szText, &cw, &ct, &bx, &by, &av, curTex, crd))
		{
			++szText;
			continue;
		}

		keepWidth += av + _kerningSize._width;
		++szText;
	}

	keepWidth -= _kerningSize._width;
	if (keepWidth > totalWidth)
		totalWidth = keepWidth;

	return Dimension2di(totalWidth, totalHeight);
}

Dimension2di UIFont::getCharSize(wchar_t ch, bool hasKerning /*= false*/)
{
	if (_fontID < 0)
		return Dimension2di();

	int height = s_fontManager->getFontTall(_fontID);
	int cw, ct, bx, by, av;
	TexturePtr curTex;
	float crd[4];

	if (!s_fontManager->getCharInfo(_fontID, ch, &cw, &ct, &bx, &by, &av, curTex, crd))
		return Dimension2di(0, height);

	int width = av;
	if (hasKerning)
		width += _kerningSize._width;
	return Dimension2di(width, height);
}

//////////////////////////////////////////////////////////////////////////
UIImageSet::UIImageSet()
	: _imageWidth(0)
	, _imageHeight(0)
	, _bindTexture(NULL)
{
}

UIImageSet::~UIImageSet()
{
}

UIImageSet* UIImageSet::createImgset(const wstring& filepath)
{
	UIImageSet* pRet = new UIImageSet();
	if (NULL != pRet && pRet->init(filepath))
		return pRet;

	SAFE_DELETE(pRet);
	return NULL;
}

bool UIImageSet::init(const wstring& filepath)
{
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(w2a_(filepath).c_str()) != tinyxml2::XML_SUCCESS)
	{
		Log::wprint(ELL_ERROR, L"图集[%s]加载失败", filepath.c_str());
		return false;
	}

	XMLElement* pRoot = doc.RootElement();
	if (NULL == pRoot)
	{
		Log::wprint(ELL_ERROR, L"图集[%s]解析失败", filepath.c_str());
		return false;
	}

	_icons.clear();
	_imagePath = ConfigManager::getSingleton().getConfig()._3d.imgsetPath;
	_imagePath += utf8ToUnicode(pRoot->Attribute("image"));
	_imageWidth = pRoot->IntAttribute("width");
	_imageHeight = pRoot->IntAttribute("height");

	XMLElement* pElem = pRoot->FirstChildElement("icon");
	while (pElem)
	{
		int x = pElem->IntAttribute("x");
		int y = pElem->IntAttribute("y");
		int w = pElem->IntAttribute("w");
		int h = pElem->IntAttribute("h");
		_icons.insert(make_pair(utf8ToUnicode(pElem->Attribute("name")), Recti(x,y,x+w,y+h)));
		pElem = pElem->NextSiblingElement();
	}

	_bindTexture = Root::getSingleton().getTextureManager()->getTexture(_imagePath);
	if (!_bindTexture)
	{
		Log::wprint(ELL_ERROR, L"图集[%s]获取纹理[%s]失败", filepath.c_str(), _imagePath.c_str());
		return false;
	}

	Log::wprint(ELL_INFORMATION, L"图集[%s]创建成功", filepath.c_str());
	return true;
}

const Recti& UIImageSet::getIconRect(const wstring& iconName)
{
	static Recti emptyRect;
	map<wstring, Recti>::iterator it = _icons.find(iconName);
	if (it == _icons.end())
		return emptyRect;
	return it->second;
}

//////////////////////////////////////////////////////////////////////////
UIWidget::UIWidget()
	: _visible(true)
	, _active(true)
	, _activeMark(true)
	, _canBeActive(true)
	, _focus(false)
	, _imgsetID(-1)
	, _imageColor(0xffffffff)
	, _outlineColor(255,0,240,0) // 默认绿色
{
}

UIWidget::~UIWidget()
{
}

void UIWidget::render()
{
	if (!Root::getSingleton().getUIManager()->isEditMode())
		return;

	Colour lineClr[4];
	const Colour selClr = Colour(255, 255, 0, 128);
	lineClr[0] = lineClr[1] = lineClr[2] = lineClr[3] = _outlineColor;

	if (_rcLocation.checkState(ERECT_LT))
	{
		lineClr[0] = selClr;
		lineClr[3] = selClr;
	}
	else if (_rcLocation.checkState(ERECT_RT))
	{
		lineClr[0] = selClr;
		lineClr[1] = selClr;
	}
	else if (_rcLocation.checkState(ERECT_RB))
	{
		lineClr[1] = selClr;
		lineClr[2] = selClr;
	}
	else if (_rcLocation.checkState(ERECT_LB))
	{
		lineClr[2] = selClr;
		lineClr[3] = selClr;
	}
	else if (_rcLocation.checkState(ERECT_T))
	{
		lineClr[0] = selClr;
	}
	else if (_rcLocation.checkState(ERECT_R))
	{
		lineClr[1] = selClr;
	}
	else if (_rcLocation.checkState(ERECT_B))
	{
		lineClr[2] = selClr;
	}
	else if (_rcLocation.checkState(ERECT_L))
	{
		lineClr[3] = selClr;
	}

	Root::getSingleton().getActiveRenderer()->draw2DLine(_absoluteRect._upperLeftCorner, Position2di(_absoluteRect._lowerRightCorner._x, _absoluteRect._upperLeftCorner._y), lineClr[0]);
	Root::getSingleton().getActiveRenderer()->draw2DLine(Position2di(_absoluteRect._lowerRightCorner._x, _absoluteRect._upperLeftCorner._y), _absoluteRect._lowerRightCorner, lineClr[1]);
	Root::getSingleton().getActiveRenderer()->draw2DLine(_absoluteRect._lowerRightCorner, Position2di(_absoluteRect._upperLeftCorner._x, _absoluteRect._lowerRightCorner._y), lineClr[2]);
	Root::getSingleton().getActiveRenderer()->draw2DLine(Position2di(_absoluteRect._upperLeftCorner._x, _absoluteRect._lowerRightCorner._y), _absoluteRect._upperLeftCorner, lineClr[3]);
}

void UIWidget::showWidget(bool bShow)
{
	if (_visible == bShow)
		return;
	_visible = bShow;
	if (bShow)
	{
		if (_activeMark)
			_active = true;
	}
	else
	{
		_active = false;
	}
}

void UIWidget::setActive(bool bActive)
{
	_active = bActive;
	_activeMark = bActive;
}

void UIWidget::setFocus(bool bFocus)
{
	if (isFocus() == bFocus || !isActive() || !isVisible())
		return;
	_focus = bFocus;
	bFocus ? setOutlineColor(Colour(255,240,0,0)) : setOutlineColor(Colour(255,0,240,0));
}

void UIWidget::setCanBeActive(bool bCanBeActive)
{
	_canBeActive = bCanBeActive;
}

void UIWidget::moveTo(int x, int y)
{
	int w = _relativeRect.getWidth();
	int h = _relativeRect.getHeight();
	_animatedRelativeRect._upperLeftCorner = Position2di(x, y);
	_animatedRelativeRect._lowerRightCorner = Position2di(x + w, y + h);
	updateAbsoluteRect();
}

void UIWidget::moveBy(int x, int y)
{
	Position2di delta(x, y);
	_animatedRelativeRect += delta;
	updateAbsoluteRect();
}

void UIWidget::setRectArea(int x, int y, int width, int height)
{
	_relativeRect = Recti(x, y, x + width, y + height);
	_relativeRect.repair();
	_animatedRelativeRect = _relativeRect;
	updateAbsoluteRect();
}

void UIWidget::touchBegan(const Touch& touch)
{
	if (!Root::getSingleton().getUIManager()->isEditMode())
		return;

	Recti rc[8];
	const int nHalfSize = 4;

	Position2di posLT = _absoluteRect._upperLeftCorner;
	Position2di posRB = _absoluteRect._lowerRightCorner;

	rc[0]._upperLeftCorner = Position2di(posLT._x-nHalfSize, posLT._y-nHalfSize);
	rc[0]._lowerRightCorner = Position2di(posLT._x+nHalfSize, posLT._y+nHalfSize);
	rc[1]._upperLeftCorner = Position2di(posLT._x+nHalfSize, posLT._y-nHalfSize);
	rc[1]._lowerRightCorner = Position2di(posRB._x-nHalfSize, posLT._y+nHalfSize);
	rc[2]._upperLeftCorner = Position2di(posRB._x-nHalfSize, posLT._y-nHalfSize);
	rc[2]._lowerRightCorner = Position2di(posRB._x+nHalfSize, posLT._y+nHalfSize);
	rc[3]._upperLeftCorner = Position2di(posRB._x-nHalfSize, posLT._y+nHalfSize);
	rc[3]._lowerRightCorner = Position2di(posRB._x+nHalfSize, posRB._y-nHalfSize);
	rc[4]._upperLeftCorner = Position2di(posRB._x-nHalfSize, posRB._y-nHalfSize);
	rc[4]._lowerRightCorner = Position2di(posRB._x+nHalfSize, posRB._y+nHalfSize);
	rc[5]._upperLeftCorner = Position2di(posLT._x+nHalfSize, posRB._y-nHalfSize);
	rc[5]._lowerRightCorner = Position2di(posRB._x-nHalfSize, posRB._y+nHalfSize);
	rc[6]._upperLeftCorner = Position2di(posLT._x-nHalfSize, posRB._y-nHalfSize);
	rc[6]._lowerRightCorner = Position2di(posLT._x+nHalfSize, posRB._y+nHalfSize);
	rc[7]._upperLeftCorner = Position2di(posLT._x-nHalfSize, posLT._y+nHalfSize);
	rc[7]._lowerRightCorner = Position2di(posLT._x+nHalfSize, posRB._y-nHalfSize);

	_rcLocation.clearState();
	if (rc[0].isPointInside(touch.getCurPos()))
		_rcLocation.addState(ERECT_LT);
	else if (rc[2].isPointInside(touch.getCurPos()))
		_rcLocation.addState(ERECT_RT);
	else if (rc[4].isPointInside(touch.getCurPos()))
		_rcLocation.addState(ERECT_RB);
	else if (rc[6].isPointInside(touch.getCurPos()))
		_rcLocation.addState(ERECT_LB);
	else if (rc[1].isPointInside(touch.getCurPos()))
		_rcLocation.addState(ERECT_T);
	else if (rc[3].isPointInside(touch.getCurPos()))
		_rcLocation.addState(ERECT_R);
	else if (rc[5].isPointInside(touch.getCurPos()))
		_rcLocation.addState(ERECT_B);
	else if (rc[7].isPointInside(touch.getCurPos()))
		_rcLocation.addState(ERECT_L);
}

void UIWidget::touchMoved(const Touch& touch)
{
	if (!Root::getSingleton().getUIManager()->isEditMode())
		return;

	if (_rcLocation.checkState(ERECT_LT))
	{
		_animatedRelativeRect._upperLeftCorner += touch.getDelta();
	}
	else if (_rcLocation.checkState(ERECT_RT))
	{
		_animatedRelativeRect._upperLeftCorner._y += touch.getDelta()._y;
		_animatedRelativeRect._lowerRightCorner._x += touch.getDelta()._x;
	}
	else if (_rcLocation.checkState(ERECT_RB))
	{
		_animatedRelativeRect._lowerRightCorner += touch.getDelta();
	}
	else if (_rcLocation.checkState(ERECT_LB))
	{
		_animatedRelativeRect._upperLeftCorner._x += touch.getDelta()._x;
		_animatedRelativeRect._lowerRightCorner._y += touch.getDelta()._y;
	}
	else if (_rcLocation.checkState(ERECT_T))
	{
		_animatedRelativeRect._upperLeftCorner._y += touch.getDelta()._y;
	}
	else if (_rcLocation.checkState(ERECT_R))
	{
		_animatedRelativeRect._lowerRightCorner._x += touch.getDelta()._x;
	}
	else if (_rcLocation.checkState(ERECT_B))
	{
		_animatedRelativeRect._lowerRightCorner._y += touch.getDelta()._y;
	}
	else if (_rcLocation.checkState(ERECT_L))
	{
		_animatedRelativeRect._upperLeftCorner._x += touch.getDelta()._x;
	}
	
	if (_rcLocation.isEmptyState())
	{
		moveBy(touch.getDelta()._x, touch.getDelta()._y);
	}
	else
	{
		_animatedRelativeRect.repair();
		updateAbsoluteRect();
	}
}

void UIWidget::touchEnded(const Touch& touch)
{
	if (!Root::getSingleton().getUIManager()->isEditMode())
		return;
	
	if (_rcLocation.isEmptyState())
		moveBy(touch.getDelta()._x, touch.getDelta()._y);
	else
		_rcLocation.clearState();
}

bool UIWidget::keyClicked(const tagKeyInput& keyInput)
{
	if (!Root::getSingleton().getUIManager()->isEditMode())
		return false;

	if (!keyInput.PressedDown)
		return false;

	int speed = 1;
	if (keyInput.Control)
		speed = 5;

	switch (keyInput.Key)
	{
	case KEY_UP:moveBy(0, -speed);return true;
	case KEY_DOWN:moveBy(0, +speed);return true;
	case KEY_LEFT:moveBy(-speed, 0);return true;
	case KEY_RIGHT:moveBy(+speed, 0);return true;
	}

	Position2di pos;
	if (keyInput.Shift)
		pos = _animatedRelativeRect._lowerRightCorner;
	else
		pos = _animatedRelativeRect._upperLeftCorner;

	bool isChangePos = false;
	switch (keyInput.Key)
	{
	case KEY_KEY_W: pos._y -= speed;isChangePos=true;break;
	case KEY_KEY_S: pos._y += speed;isChangePos=true;break;
	case KEY_KEY_A: pos._x -= speed;isChangePos=true;break;
	case KEY_KEY_D: pos._x += speed;isChangePos=true;break;
	}

	if (keyInput.Shift)
		_animatedRelativeRect._lowerRightCorner = pos;
	else
		_animatedRelativeRect._upperLeftCorner = pos;

	if (isChangePos)
	{
		_animatedRelativeRect.repair();
		updateAbsoluteRect();
	}

	return isChangePos;
}

//////////////////////////////////////////////////////////////////////////
UIControl::UIControl()
	: _ID(-1)
	, _parent(NULL)
	, _status(EUICS_NORMAL)
	, _textScale(1.0f)
	, _textColor(0xffffffff)
	, _textAlignHori(EUIA_LEFT_OR_UP)
	, _textAlignVert(EUIA_LEFT_OR_UP)
	, _fontID(0)
{
}

UIControl::~UIControl()
{
}

void UIControl::render()
{
	if (Root::getSingleton().getUIManager()->isEditMode())
	{
		if (isFocus())
		{
			UIManager* pUIMgr = Root::getSingleton().getUIManager();
			wchar_t szTemp[256] = {};
			UIFont* pFont = pUIMgr->getFont(EUIFT_TOOLTIP);
			swprintf(szTemp, 256, L"%s,ID[%d]Rect[%d %d %d %d]",
				a2w_(getType()).c_str(),
				getID(),
				getAnimatedRelativeRect()._upperLeftCorner._x,
				getAnimatedRelativeRect()._upperLeftCorner._y,
				getAnimatedRelativeRect().getWidth(),
				getAnimatedRelativeRect().getHeight());

			Vector2di tipFontDrawArea = getAbsoluteRect()._upperLeftCorner;
			pFont->drawTextBegin();
			pFont->drawText(tipFontDrawArea._x, tipFontDrawArea._y, szTemp, 1.0f, 0xff008000);
			pFont->drawTextEnd();
		}
		UIWidget::render();
	}
}

void UIControl::setActive(bool bActive)
{
	UIWidget::setActive(bActive);
	if (bActive)
		_status = EUICS_NORMAL;
	else
		_status = EUICS_DISABLE;
}

void UIControl::updateAbsoluteRect()
{
	_absoluteRect = _animatedRelativeRect;
	if (NULL != _parent)
		_absoluteRect += _parent->getAbsoluteRect()._upperLeftCorner;
}

const std::wstring& UIControl::getImage(EUIControlStatus status) const
{
	return _imageArr[status];
}

void UIControl::setImage(EUIControlStatus status, const wstring& iconName)
{
	_imageArr[status] = iconName;
}

void UIControl::serialize(XMLElement* outXml)
{
	char buffer[256] = {};
	// 1, id
	sprintf_s(buffer, sizeof(buffer), "%d", _ID);
	outXml->SetAttribute("id", buffer);
	// 2, rect
	Recti animRc = getAnimatedRelativeRect();
	sprintf_s(buffer, sizeof(buffer), "%d,%d,%d,%d", animRc._upperLeftCorner._x, animRc._upperLeftCorner._y, animRc.getWidth(), animRc.getHeight());
	outXml->SetAttribute("rect", buffer);
	// 3, imageset
	sprintf_s(buffer, sizeof(buffer), "%d", _imgsetID);
	outXml->SetAttribute("imageset", buffer);
	// 4, image
	sprintf_s(buffer, sizeof(buffer), "%s", w2a_(_imageArr[EUICS_NORMAL]).c_str());
	outXml->SetAttribute("image", buffer);
	// 5, text
	outXml->SetAttribute("textid", _textID.c_str());
	// 6, textscale
	sprintf_s(buffer, sizeof(buffer), "%.1f", _textScale);
	outXml->SetAttribute("textscale", buffer);
	// 7, textcolor
	sprintf_s(buffer, sizeof(buffer), "0x%08x", _textColor._clr);
	outXml->SetAttribute("textcolor", buffer);
	// 8, textalignhori
	sprintf_s(buffer, sizeof(buffer), "%d", _textAlignHori);
	outXml->SetAttribute("textalignhori", buffer);
	// 9, textalignvert
	sprintf_s(buffer, sizeof(buffer), "%d", _textAlignVert);
	outXml->SetAttribute("textalignvert", buffer);
	// 10, fontID
	sprintf_s(buffer, sizeof(buffer), "%d", _fontID);
	outXml->SetAttribute("fontid", buffer);
}

bool UIControl::deserialize(XMLElement* inXml)
{
	if (NULL == inXml)
		return false;

	setID(inXml->IntAttribute("id"));
	vector<string> container;
	string strTemp = inXml->Attribute("rect");
	stringtok(container, strTemp, ",");
	if (container.size() >= 4)
	{
		int x = atoi(container[0].c_str());
		int y = atoi(container[1].c_str());
		int width = atoi(container[2].c_str());
		int height = atoi(container[3].c_str());
		setRectArea(x, y, width, height);
	}

	setImgsetID(inXml->IntAttribute("imageset"));

	strTemp = inXml->Attribute("image");
	setImage(EUICS_NORMAL, utf8ToUnicode(strTemp));

	_textID = inXml->Attribute("textid");
	container.clear();
	stringtok(container, _textID, ",");
	unsigned int ids = container.size();
	wstring textContent;
	for (unsigned int i = 0; i < ids; ++i)
	{
		textContent += Language::getSingleton().getLanguageByID(atoi(container[i].c_str()));
		textContent += L"\r\n";
	}
	setText(trim(textContent));
	setTextScale(inXml->FloatAttribute("textscale"));
	strTemp = inXml->Attribute("textcolor");
	setTextColor(strtoul(strTemp.c_str(), NULL, 16));
	setTextAlignHori((EUIAlignment)inXml->IntAttribute("textalignhori"));
	setTextAlignVert((EUIAlignment)inXml->IntAttribute("textalignvert"));
	setFontID(inXml->IntAttribute("fontid"));

	return true;
}

//////////////////////////////////////////////////////////////////////////
VAYO_REFLEX_IMPLEMENT(UIButton)
UIButton::UIButton()
	: _click(false)
	, _hover(false)
{
}

UIButton::~UIButton()
{
}

void UIButton::render()
{
	RenderSystem* renderer = Root::getSingleton().getActiveRenderer();
	UIManager* uiMgr = Root::getSingleton().getUIManager();
	UISkin* skin = uiMgr->getSkin();

	if (_imageArr[EUICS_NORMAL].empty() && _imageArr[EUICS_PUSH].empty() && _imageArr[EUICS_DISABLE].empty())
	{
		if (_hover)
			skin->draw3DButtonPanePressed(_absoluteRect, NULL);
		else
			skin->draw3DButtonPaneStandard(_absoluteRect, NULL);
	}

	UIManager* pUIMgr = Root::getSingleton().getUIManager();
	if (!_imageArr[_status].empty())
	{
		pUIMgr->drawIconBegin(_imgsetID);
		pUIMgr->drawIcon(_imageArr[_status], _absoluteRect, &_imageColor);
		pUIMgr->drawIconEnd();
	}

	if (!_text.empty())
	{
		UIFont* pFont = pUIMgr->getFont(_fontID);
		int x = _absoluteRect._upperLeftCorner._x;
		int y = _absoluteRect._upperLeftCorner._y;
		int w = _absoluteRect.getWidth();
		int h = _absoluteRect.getHeight();

		if (_hover)
		{
			x += skin->getSize(EUIS_BUTTON_PRESSED_TEXT_OFFSET_X);
			y += skin->getSize(EUIS_BUTTON_PRESSED_TEXT_OFFSET_Y);
		}

		pFont->renderText(_text, Recti(x, y, x + w, y + h), _textColor, _textAlignHori, _textAlignVert, _textScale, &_absoluteRect);
	}

	UIControl::render();
}

bool UIButton::isClicked()
{
	bool isClick = _click;
	_click = false;
	return isClick;
}

bool UIButton::isHover() const
{
	return _hover;
}

void UIButton::touchBegan(const Touch& touch)
{
	UIControl::touchBegan(touch);
	if (Root::getSingleton().getUIManager()->isEditMode())
		return;

	_status = EUICS_PUSH;
	_click = false;
	if (_absoluteRect.isPointInside(touch.getCurPos()))
		_hover = true;
}

void UIButton::touchMoved(const Touch& touch)
{
	UIControl::touchMoved(touch);
	if (Root::getSingleton().getUIManager()->isEditMode())
		return;

	if (_hover && !_absoluteRect.isPointInside(touch.getCurPos()))
	{
		_hover = false;
	}
}

void UIButton::touchEnded(const Touch& touch)
{
	UIControl::touchEnded(touch);
	if (Root::getSingleton().getUIManager()->isEditMode())
		return;

	_status = EUICS_NORMAL;
	if (_hover)
	{
		_click = true;
		_hover = false;
		SignalTrigger::emit(_buttonClicked);
	}
}

void UIButton::serialize(XMLElement* outXml)
{
	UIControl::serialize(outXml);
	char buffer[256] = {};
	sprintf_s(buffer, sizeof(buffer), "%s", w2a_(_imageArr[EUICS_PUSH]).c_str());
	outXml->SetAttribute("pushimage", buffer);
	sprintf_s(buffer, sizeof(buffer), "%s", w2a_(_imageArr[EUICS_DISABLE]).c_str());
	outXml->SetAttribute("disableimage", buffer);
}

bool UIButton::deserialize(XMLElement* inXml)
{
	if (!UIControl::deserialize(inXml))
		return false;

	setImage(EUICS_PUSH, utf8ToUnicode(inXml->Attribute("pushimage")));
	setImage(EUICS_DISABLE, utf8ToUnicode(inXml->Attribute("disableimage")));
	return true;
}

//////////////////////////////////////////////////////////////////////////
VAYO_REFLEX_IMPLEMENT(UICheckBox)
UICheckBox::UICheckBox()
	: _checked(false)
	, _pressed(false)
{
}

UICheckBox::~UICheckBox()
{
}

void UICheckBox::render()
{
	UIManager* uiMgr = Root::getSingleton().getUIManager();
	UISkin* skin = uiMgr->getSkin();
	if (skin)
	{
		const int height = skin->getSize(EUIS_CHECK_BOX_WIDTH);
		Recti checkRect(_absoluteRect._upperLeftCorner._x,
			((_absoluteRect.getHeight() - height) / 2) + _absoluteRect._upperLeftCorner._y,
			0, 0);

		checkRect._lowerRightCorner._x = checkRect._upperLeftCorner._x + height;
		checkRect._lowerRightCorner._y = checkRect._upperLeftCorner._y + height;

		EUIColor col = EUIC_GRAY_EDITABLE;
		if (isActive())
			col = _pressed ? EUIC_FOCUSED_EDITABLE : EUIC_EDITABLE;
		skin->draw3DSunkenPane(skin->getColor(col), false, true, checkRect, NULL);

		if (_checked)
		{
			wchar_t ch = (wchar_t)skin->getIcon(EUII_CHECK_BOX_CHECKED);
			UIFont* pFont = uiMgr->getFont(EUIFT_TOOLTIP);
			Recti adjustRc = checkRect;
			adjustRc += Vector2di(0, 1);
			pFont->renderText(wstring(1, ch), adjustRc,
				skin->getColor(isActive() ? EUIC_BUTTON_TEXT : EUIC_GRAY_TEXT),
				EUIA_CENTER, EUIA_LEFT_OR_UP, 1.0f, &adjustRc);
		}

		if (_text.size())
		{
			checkRect = _absoluteRect;
			checkRect._upperLeftCorner._x += height + 5;

			Colour textClr = isActive() ? _textColor : skin->getColor(EUIC_GRAY_TEXT);
			UIFont* pFont = uiMgr->getFont(_fontID);
			pFont->renderText(_text, checkRect, textClr,
				_textAlignHori, _textAlignVert, _textScale, &checkRect);
		}
	}
	
	UIControl::render();
}

bool UICheckBox::isChecked() const
{
	return _checked;
}

void UICheckBox::setChecked(bool checked)
{
	_checked = checked;
}

void UICheckBox::touchBegan(const Touch& touch)
{
	UIControl::touchBegan(touch);
	if (Root::getSingleton().getUIManager()->isEditMode())
		return;

	if (_absoluteRect.isPointInside(touch.getCurPos()))
		_pressed = true;
}

void UICheckBox::touchMoved(const Touch& touch)
{
	UIControl::touchMoved(touch);
	if (Root::getSingleton().getUIManager()->isEditMode())
		return;

	if (_pressed && !_absoluteRect.isPointInside(touch.getCurPos()))
	{
		_pressed = false;
	}
}

void UICheckBox::touchEnded(const Touch& touch)
{
	UIControl::touchEnded(touch);
	if (Root::getSingleton().getUIManager()->isEditMode())
		return;

	if (_pressed)
	{
		_checked = !_checked;
		_pressed = false;
		SignalTrigger::emit(_checkBoxChanged);
	}
}

void UICheckBox::serialize(XMLElement* outXml)
{
	UIControl::serialize(outXml);
	char buffer[256] = {};
	int check = _checked ? 1 : 0;
	sprintf_s(buffer, sizeof(buffer), "%d", check);
	outXml->SetAttribute("checked", buffer);
}

bool UICheckBox::deserialize(XMLElement* inXml)
{
	if (!UIControl::deserialize(inXml))
		return false;

	setChecked(inXml->BoolAttribute("checked"));
	return true;
}

//////////////////////////////////////////////////////////////////////////
VAYO_REFLEX_IMPLEMENT(UIStatic)
UIStatic::UIStatic()
	: _lastFontID(_fontID)
	, _border(true)
	, _wordWrap(true)
	, _drawBg(true)
	, _clipText(true)
	, _bgColorEnabled(false)
	, _bgColor(Root::getSingleton().getUIManager()->getSkin()->getColor(EUIC_FACE_3D))
{
}

UIStatic::~UIStatic()
{
}

void UIStatic::render()
{
	UIManager* uiMgr = Root::getSingleton().getUIManager();
	UISkin* skin = uiMgr->getSkin();
	if (!skin)
		return;

	RenderSystem* renderer = Root::getSingleton().getActiveRenderer();
	Recti frameRect(_absoluteRect);

	// draw background
	if (_drawBg)
	{
		if (!_bgColorEnabled) // skin-colors can change
			_bgColor = skin->getColor(EUIC_FACE_3D);
		renderer->draw2DRect(_bgColor, frameRect, NULL);
	}

	// draw the border
	if (_border)
	{
		skin->draw3DSunkenPane(0, true, false, frameRect, NULL);
	}

	Vector2di offset = Vector2di(skin->getSize(EUIS_TEXT_DISTANCE_X), skin->getSize(EUIS_TEXT_DISTANCE_Y));
	frameRect._upperLeftCorner += offset;
	frameRect._lowerRightCorner -= offset;

	// draw the text
	if (_text.size())
	{
		UIFont* font = uiMgr->getFont(_fontID);
		if (font)
		{
			if (_fontID != _lastFontID)
				breakText();

			Recti r = frameRect;
			int height = font->getFontSize();
			int kerningH = font->getKerningSize()._height;
			int totalHeight = (height + kerningH) * _brokenText.size();
			if (_textAlignVert == EUIA_CENTER)
			{
				r._upperLeftCorner._y = r.getCenter()._y - (totalHeight / 2);
			}
			else if (_textAlignVert == EUIA_RIGHT_OR_DOWN)
			{
				r._upperLeftCorner._y = r._lowerRightCorner._y - totalHeight;
			}

			for (unsigned i = 0; i < _brokenText.size(); ++i)
			{
				if (_textAlignHori == EUIA_RIGHT_OR_DOWN)
				{
					r._upperLeftCorner._x = frameRect._lowerRightCorner._x - font->getTextSize(_brokenText[i])._width;
				}

				font->renderText(_brokenText[i], r, isActive() ? _textColor : skin->getColor(EUIC_GRAY_TEXT),
					(_textAlignHori != EUIA_CENTER) ? EUIA_LEFT_OR_UP : EUIA_CENTER, EUIA_LEFT_OR_UP,
					_textScale, (_clipText ? &frameRect : NULL));

				r._lowerRightCorner._y += height + kerningH;
				r._upperLeftCorner._y += height + kerningH;
			}
		}
	}

	UIControl::render();
}

void UIStatic::setText(const wstring& text)
{
	UIControl::setText(text);
	breakText();
}

int UIStatic::getTextWidth() const
{
	UIFont* font = Root::getSingleton().getUIManager()->getFont(_fontID);
	if (!font)
		return 0;

	int widest = 0;

	for (unsigned int line = 0; line < _brokenText.size(); ++line)
	{
		int width = font->getTextSize(_brokenText[line])._width;
		if (width > widest)
			widest = width;
	}

	return widest;
}

int UIStatic::getTextHeight() const
{
	UIFont* font = Root::getSingleton().getUIManager()->getFont(_fontID);
	if (!font)
		return 0;

	int kerningH = font->getKerningSize()._height;
	return (font->getFontSize() + kerningH) * _brokenText.size();
}

void UIStatic::updateAbsoluteRect()
{
	UIControl::updateAbsoluteRect();
	breakText();
}

void UIStatic::setWordWrap(bool enable)
{
	if (_wordWrap != enable)
	{
		_wordWrap = enable;
		breakText();
	}
}

void UIStatic::setBgColor(Colour color)
{
	_bgColor = color;
	_bgColorEnabled = true;
	_drawBg = true;
}

void UIStatic::serialize(XMLElement* outXml)
{
	UIControl::serialize(outXml);
	char buffer[256] = {};
	int val = _wordWrap ? 1 : 0;
	sprintf_s(buffer, sizeof(buffer), "%d", val);
	outXml->SetAttribute("wordwrap", buffer);
	val = _border ? 1 : 0;
	sprintf_s(buffer, sizeof(buffer), "%d", val);
	outXml->SetAttribute("border", buffer);
	val = _drawBg ? 1 : 0;
	sprintf_s(buffer, sizeof(buffer), "%d", val);
	outXml->SetAttribute("drawbg", buffer);
	val = _clipText ? 1 : 0;
	sprintf_s(buffer, sizeof(buffer), "%d", val);
	outXml->SetAttribute("cliptext", buffer);
	sprintf_s(buffer, sizeof(buffer), "0x%08x", _bgColor._clr);
	outXml->SetAttribute("bgcolor", buffer);
	val = _bgColorEnabled ? 1 : 0;
	sprintf_s(buffer, sizeof(buffer), "%d", val);
	outXml->SetAttribute("bgcolorenabled", buffer);
}

bool UIStatic::deserialize(XMLElement* inXml)
{
	if (!UIControl::deserialize(inXml))
		return false;

	setWordWrap(inXml->BoolAttribute("wordwrap"));
	setBorder(inXml->BoolAttribute("border"));
	setDrawBg(inXml->BoolAttribute("drawbg"));
	setClipText(inXml->BoolAttribute("cliptext"));
	string strTemp = inXml->Attribute("bgcolor");
	setBgColor(strtoul(strTemp.c_str(), NULL, 16));
	setBgColorEnabled(inXml->BoolAttribute("bgcolorenabled"));
	return true;
}

void UIStatic::breakText()
{
	if (_text.empty())
		return;

	UIManager* uiMgr = Root::getSingleton().getUIManager();
	UISkin* skin = uiMgr->getSkin();
	UIFont* font = uiMgr->getFont(_fontID);
	if (!skin || !font)
		return;

	_brokenText.clear();
	_lastFontID = _fontID;

	wstring line;
	int size = _text.size();
	wchar_t c;

	for (int i = 0; i < size; ++i)
	{
		c = _text[i];
		bool lineBreak = false;

		if (c == L'\r') // Mac or Windows breaks
		{
			lineBreak = true;
			if (_text[i + 1] == L'\n') // Windows breaks
				++i;
		}
		else if (c == L'\n') // Unix breaks
		{
			lineBreak = true;
		}

		if (lineBreak)
		{
			_brokenText.push_back(line);
			line = L"";
		}
		else
			line += c;
	}

	if (!line.empty())
		_brokenText.push_back(line);

	if (_wordWrap)
	{
		vector<wstring> allLines = _brokenText;
		_brokenText.clear();
		unsigned int oriLineCnt = allLines.size();
		for (unsigned int i = 0; i < oriLineCnt; ++i)
		{
			breakLine(_brokenText, allLines[i]);
		}
	}
}

void UIStatic::breakLine(vector<wstring>& outLines, const wstring& inLine)
{
	if (inLine.empty())
		return;

	UIManager* uiMgr = Root::getSingleton().getUIManager();
	UISkin* skin = uiMgr->getSkin();
	UIFont* font = uiMgr->getFont(_fontID);
	if (!skin || !font)
		return;

	wstring line;
	wstring word;
	wstring whitespace;
	int size = inLine.size();
	int length = 0;
	int elWidth = _absoluteRect.getWidth();

	if (_border)
		elWidth -= 2 * skin->getSize(EUIS_TEXT_DISTANCE_X);

	wchar_t c;

	for (int i = 0; i < size; ++i)
	{
		c = inLine[i];

		bool isWhitespace = (c == L' ' || c == L'\t');
		if (!isWhitespace)
			word += c;

		if (isWhitespace || c > 255 || i == (size - 1))
		{
			if (word.size())
			{
				const int whitelgth = font->getTextSize(whitespace.c_str())._width;
				const int wordlgth = font->getTextSize(word.c_str())._width;

				if (wordlgth > elWidth)
				{
					int where = word.find_first_of(wchar_t(0x00AD));
					if (where != -1)
					{
						wstring first = word.substr(0, where);
						wstring second = word.substr(where, word.size() - where);
						outLines.push_back(line + first + L"-");
						const int secondLength = font->getTextSize(second.c_str())._width;

						length = secondLength;
						line = second;
					}
					else
					{
						if (length && line != L"")
							outLines.push_back(line);
						length = wordlgth;
						line = word;
					}
				}
				else if (length && (length + wordlgth + whitelgth > elWidth))
				{
					if (line != L"")
						outLines.push_back(line);
					length = wordlgth;
					line = word;
				}
				else
				{
					line += whitespace;
					line += word;
					length += whitelgth + wordlgth;
				}

				word = L"";
				whitespace = L"";
			}

			if (isWhitespace)
			{
				whitespace += c;
			}
		}
	}

	line += whitespace;
	line += word;
	if (line != L"")
		outLines.push_back(line);
}

//////////////////////////////////////////////////////////////////////////
VAYO_REFLEX_IMPLEMENT(UIScrollBar)
UIScrollBar::UIScrollBar()
	: _upButton(NULL)
	, _downButton(NULL)
	, _dragging(false)
	, _horizontal(false)
	, _draggedBySlider(false)
	, _trayClick(false)
	, _pos(0)
	, _drawPos(0)
	, _drawHeight(0)
	, _min(0)
	, _max(100)
	, _smallStep(1)
	, _largeStep(10)
	, _desiredPos(0)
	, _timeGap(0)
{
	refreshControls();
	setPos(0);
}

UIScrollBar::~UIScrollBar()
{
	SAFE_DELETE(_upButton);
	SAFE_DELETE(_downButton);
}

void UIScrollBar::render()
{
	UIManager* uiMgr = Root::getSingleton().getUIManager();
	UISkin* skin = uiMgr->getSkin();
	if (!skin)
		return;

	_sliderRect = _absoluteRect;

	// draws the background
	skin->draw2DRect(skin->getColor(EUIC_SCROLLBAR), _sliderRect, &_absoluteRect);

	if (!isZero(range()))
	{
		// recalculate slider rectangle
		if (_horizontal)
		{
			_sliderRect._upperLeftCorner._x = _absoluteRect._upperLeftCorner._x + _drawPos + _absoluteRect.getHeight() - _drawHeight / 2;
			_sliderRect._lowerRightCorner._x = _sliderRect._upperLeftCorner._x + _drawHeight;
		}
		else
		{
			_sliderRect._upperLeftCorner._y = _absoluteRect._upperLeftCorner._y + _drawPos + _absoluteRect.getWidth() - _drawHeight / 2;
			_sliderRect._lowerRightCorner._y = _sliderRect._upperLeftCorner._y + _drawHeight;
		}

		skin->draw3DButtonPaneStandard(_sliderRect, &_absoluteRect);
	}

	_upButton->render();
	_downButton->render();
	UIControl::render();

	_timeGap += Root::getSingleton().getTimer().deltaMilliTime();
	if (_timeGap < 200)
		return;

	_timeGap = 0;

	const int oldPos = _pos;
	bool btnHover = false;
	if (_upButton->isHover())
	{
		btnHover = true;
		setPos(_pos - _smallStep);
	}
	else if (_downButton->isHover())
	{
		btnHover = true;
		setPos(_pos + _smallStep);
	}

	if (btnHover)
	{
		if (oldPos != _pos)
			SignalTrigger::emit(_scrollBarChanged);
	}
	else if (_dragging && !_draggedBySlider && _trayClick)
	{
		if (_desiredPos >= _pos + _largeStep)
			setPos(_pos + _largeStep);
		else if (_desiredPos <= _pos - _largeStep)
			setPos(_pos - _largeStep);
		else if (_desiredPos >= _pos - _largeStep && _desiredPos <= _pos + _largeStep)
			setPos(_desiredPos);

		if (oldPos != _pos)
			SignalTrigger::emit(_scrollBarChanged);
	}
}

void UIScrollBar::updateAbsoluteRect()
{
	UIControl::updateAbsoluteRect();
	refreshControls();
	setPos(_pos);
}

void UIScrollBar::setPos(int pos)
{
	_pos = clamp_(pos, _min, _max);

	if (_horizontal)
	{
		float f = (_absoluteRect.getWidth() - ((float)_absoluteRect.getHeight()*3.0f)) / range();
		_drawPos = (int)(((_pos - _min) * f) + ((float)_absoluteRect.getHeight() * 0.5f));
		_drawHeight = _absoluteRect.getHeight();
	}
	else
	{
		float f = (_absoluteRect.getHeight() - ((float)_absoluteRect.getWidth()*3.0f)) / range();
		_drawPos = (int)(((_pos - _min) * f) + ((float)_absoluteRect.getWidth() * 0.5f));
		_drawHeight = _absoluteRect.getWidth();
	}
}

void UIScrollBar::setMin(int minVal)
{
	_min = minVal;
	if (_max < _min)
		_max = _min;

	bool enable = !isZero(range());
	_upButton->setActive(enable);
	_downButton->setActive(enable);
	setPos(_pos);
}

void UIScrollBar::setMax(int maxVal)
{
	_max = maxVal;
	if (_min > _max)
		_min = _max;

	bool enable = !isZero(range());
	_upButton->setActive(enable);
	_downButton->setActive(enable);
	setPos(_pos);
}

void UIScrollBar::setSmallStep(int step)
{
	if (step > 0)
		_smallStep = step;
	else
		_smallStep = 1;
}

void UIScrollBar::setLargeStep(int step)
{
	if (step > 0)
		_largeStep = step;
	else
		_largeStep = 10;
}

void UIScrollBar::touchBegan(const Touch& touch)
{
	if (!Root::getSingleton().getUIManager()->isEditMode())
	{
		if (_upButton->isVisible() && _upButton->isActive())
			_upButton->touchBegan(touch);
		if (_downButton->isVisible() && _downButton->isActive())
			_downButton->touchBegan(touch);
	}

	UIControl::touchBegan(touch);
	if (Root::getSingleton().getUIManager()->isEditMode())
		return;

	Position2di pos = touch.getCurPos();
	if (_absoluteRect.isPointInside(pos))
	{
		_dragging = true;
		_draggedBySlider = _sliderRect.isPointInside(pos);
		_trayClick = !_draggedBySlider && !_upButton->getAbsoluteRect().isPointInside(pos) && !_downButton->getAbsoluteRect().isPointInside(pos);
		_desiredPos = getPosFromMousePt(touch.getCurPos());
	}
}

void UIScrollBar::touchMoved(const Touch& touch)
{
	if (!Root::getSingleton().getUIManager()->isEditMode())
	{
		if (_upButton->isVisible() && _upButton->isActive())
			_upButton->touchMoved(touch);
		if (_downButton->isVisible() && _downButton->isActive())
			_downButton->touchMoved(touch);
	}

	UIControl::touchMoved(touch);
	if (Root::getSingleton().getUIManager()->isEditMode())
		return;

	if (!_dragging)
		return;

	Position2di pos = touch.getCurPos();
	const int newPos = getPosFromMousePt(pos);
	const int oldPos = _pos;

	if (!_draggedBySlider)
	{
		if (_absoluteRect.isPointInside(pos))
		{
			_draggedBySlider = _sliderRect.isPointInside(pos);
			_trayClick = !_draggedBySlider && !_upButton->getAbsoluteRect().isPointInside(pos) && !_downButton->getAbsoluteRect().isPointInside(pos);
		}
		else
			_trayClick = false;
	}
	
	if (_draggedBySlider)
		setPos(newPos);
	else
		_desiredPos = newPos;

	if (oldPos != _pos)
		SignalTrigger::emit(_scrollBarChanged);
}

void UIScrollBar::touchEnded(const Touch& touch)
{
	if (!Root::getSingleton().getUIManager()->isEditMode())
	{
		if (_upButton->isVisible() && _upButton->isActive())
			_upButton->touchEnded(touch);
		if (_downButton->isVisible() && _downButton->isActive())
			_downButton->touchEnded(touch);
	}

	UIControl::touchEnded(touch);
	if (Root::getSingleton().getUIManager()->isEditMode())
		return;

	if (!_dragging)
		return;

	_dragging = false;
	Position2di pos = touch.getCurPos();
	const int newPos = getPosFromMousePt(pos);
	const int oldPos = _pos;

	if (!_draggedBySlider)
	{
		if (_absoluteRect.isPointInside(pos))
		{
			_draggedBySlider = _sliderRect.isPointInside(pos);
			_trayClick = !_draggedBySlider && !_upButton->getAbsoluteRect().isPointInside(pos) && !_downButton->getAbsoluteRect().isPointInside(pos);
		}
		else
			_trayClick = false;
	}

	if (_draggedBySlider)
		setPos(newPos);
	else
		_desiredPos = newPos;

	if (oldPos != _pos)
		SignalTrigger::emit(_scrollBarChanged);
}

bool UIScrollBar::touchWheel(const Touch& touch, float wheel)
{
	if (Root::getSingleton().getUIManager()->isEditMode())
		return false;

	const int oldPos = _pos;
	setPos(getPos() + ((wheel < 0 ? 1 : -1) * _smallStep));
	if (oldPos != _pos)
		SignalTrigger::emit(_scrollBarChanged);

	return true;
}

void UIScrollBar::serialize(XMLElement* outXml)
{
	UIControl::serialize(outXml);
	char buffer[256] = {};
	sprintf_s(buffer, sizeof(buffer), "%d", _min);
	outXml->SetAttribute("min", buffer);
	sprintf_s(buffer, sizeof(buffer), "%d", _max);
	outXml->SetAttribute("max", buffer);
	sprintf_s(buffer, sizeof(buffer), "%d", _smallStep);
	outXml->SetAttribute("smallstep", buffer);
	sprintf_s(buffer, sizeof(buffer), "%d", _largeStep);
	outXml->SetAttribute("largestep", buffer);
	sprintf_s(buffer, sizeof(buffer), "%s", w2a_(_upButton->getImage(EUICS_NORMAL)).c_str());
	outXml->SetAttribute("btnnormalimage", buffer);
	sprintf_s(buffer, sizeof(buffer), "%s", w2a_(_upButton->getImage(EUICS_PUSH)).c_str());
	outXml->SetAttribute("btnpushimage", buffer);
	sprintf_s(buffer, sizeof(buffer), "%s", w2a_(_upButton->getImage(EUICS_DISABLE)).c_str());
	outXml->SetAttribute("btndisableimage", buffer);
	sprintf_s(buffer, sizeof(buffer), "%s", w2a_(_sliderIconName).c_str());
	outXml->SetAttribute("sliderimage", buffer);
}

bool UIScrollBar::deserialize(XMLElement* inXml)
{
	if (!UIControl::deserialize(inXml))
		return false;

	setMin(inXml->IntAttribute("min"));
	setMax(inXml->IntAttribute("max"));
	setSmallStep(inXml->IntAttribute("smallstep"));
	setLargeStep(inXml->IntAttribute("largestep"));
	string strTemp = inXml->Attribute("sliderimage");
	setSliderIconName(utf8ToUnicode(strTemp));
	strTemp = inXml->Attribute("btnnormalimage");
	_upButton->setImage(EUICS_NORMAL, utf8ToUnicode(strTemp));
	_downButton->setImage(EUICS_NORMAL, utf8ToUnicode(strTemp));
	strTemp = inXml->Attribute("btnpushimage");
	_upButton->setImage(EUICS_PUSH, utf8ToUnicode(strTemp));
	_downButton->setImage(EUICS_PUSH, utf8ToUnicode(strTemp));
	strTemp = inXml->Attribute("btndisableimage");
	_upButton->setImage(EUICS_DISABLE, utf8ToUnicode(strTemp));
	_downButton->setImage(EUICS_DISABLE, utf8ToUnicode(strTemp));
	return true;
}

void UIScrollBar::refreshControls()
{
	_horizontal = _absoluteRect.getWidth() > _absoluteRect.getHeight();
	if (_horizontal)
	{
		int h = _absoluteRect.getHeight();
		if (!_upButton)
			_upButton = new UIButton();

		Recti rc(0, 0, h, h);
		if (rc.getWidth() > 0 && rc.getHeight() > 0)
		{
			rc += _absoluteRect._upperLeftCorner;
			_upButton->setRectArea(rc._upperLeftCorner._x, rc._upperLeftCorner._y, rc.getWidth(), rc.getHeight());
			_upButton->updateAbsoluteRect();
		}

		if (!_downButton)
			_downButton = new UIButton();

		rc._upperLeftCorner.set(_absoluteRect.getWidth() - h, 0);
		rc._lowerRightCorner.set(_absoluteRect.getWidth(), h);
		if (rc.getWidth() > 0 && rc.getHeight() > 0)
		{
			rc += _absoluteRect._upperLeftCorner;
			_downButton->setRectArea(rc._upperLeftCorner._x, rc._upperLeftCorner._y, rc.getWidth(), rc.getHeight());
			_downButton->updateAbsoluteRect();
		}
	}
	else
	{
		int w = _absoluteRect.getWidth();
		if (!_upButton)
			_upButton = new UIButton();

		Recti rc(0, 0, w, w);
		if (rc.getWidth() > 0 && rc.getHeight() > 0)
		{
			rc += _absoluteRect._upperLeftCorner;
			_upButton->setRectArea(rc._upperLeftCorner._x, rc._upperLeftCorner._y, rc.getWidth(), rc.getHeight());
			_upButton->updateAbsoluteRect();
		}

		if (!_downButton)
			_downButton = new UIButton();

		rc._upperLeftCorner.set(0, _absoluteRect.getHeight() - w);
		rc._lowerRightCorner.set(w, _absoluteRect.getHeight());
		if (rc.getWidth() > 0 && rc.getHeight() > 0)
		{
			rc += _absoluteRect._upperLeftCorner;
			_downButton->setRectArea(rc._upperLeftCorner._x, rc._upperLeftCorner._y, rc.getWidth(), rc.getHeight());
			_downButton->updateAbsoluteRect();
		}
	}
}

int UIScrollBar::getPosFromMousePt(const Position2di& pt) const
{
	float w, p;
	if (_horizontal)
	{
		w = _absoluteRect.getWidth() - float(_absoluteRect.getHeight())*3.0f;
		p = pt._x - _absoluteRect._upperLeftCorner._x - _absoluteRect.getHeight()*1.5f;
	}
	else
	{
		w = _absoluteRect.getHeight() - float(_absoluteRect.getWidth())*3.0f;
		p = pt._y - _absoluteRect._upperLeftCorner._y - _absoluteRect.getWidth()*1.5f;
	}
	return (int)(p / w * range()) + _min;
}

//////////////////////////////////////////////////////////////////////////
VAYO_REFLEX_IMPLEMENT(UIListBox)
UIListBox::UIListBox()
	: _selected(-1)
	, _itemHeight(0)
	, _itemHeightOverride(0)
	, _totalItemHeight(0)
	, _itemsIconWidth(0)
	, _scrollBar(0)
	, _selectTime(0)
	, _lastKeyTime(0)
	, _selecting(false)
	, _drawBg(true)
	, _moveOverSelect(false)
	, _autoScroll(true)
	, _highlightWhenNotFocused(true)
{
	_scrollBar = new UIScrollBar();
	updateAbsoluteRect();
}

UIListBox::~UIListBox()
{
	SAFE_DELETE(_scrollBar);
}

void UIListBox::render()
{
	recalculateItemHeight();

	UISkin* skin = Root::getSingleton().getUIManager()->getSkin();
	UIFont* font = Root::getSingleton().getUIManager()->getFont(EUIFT_DEFAULT);

	Recti* clipRect = 0;

	Recti frameRect(_absoluteRect);

	Recti clientClip(_absoluteRect);
	clientClip._upperLeftCorner._y += 1;
	clientClip._upperLeftCorner._x += 1;
	if (_scrollBar->isVisible())
		clientClip._lowerRightCorner._x = _absoluteRect._lowerRightCorner._x - skin->getSize(EUIS_SCROLLBAR_SIZE);
	clientClip._lowerRightCorner._y -= 1;
	clientClip.clipAgainst(_absoluteRect);

	skin->draw3DSunkenPane(skin->getColor(EUIC_HIGH_LIGHT_3D), true, _drawBg, frameRect, &_absoluteRect);

	if (clipRect)
		clientClip.clipAgainst(*clipRect);

	frameRect = _absoluteRect;
	frameRect._upperLeftCorner._x += 1;
	if (_scrollBar->isVisible())
		frameRect._lowerRightCorner._x = _absoluteRect._lowerRightCorner._x - skin->getSize(EUIS_SCROLLBAR_SIZE);

	frameRect._lowerRightCorner._y = _absoluteRect._upperLeftCorner._y + _itemHeight;

	frameRect._upperLeftCorner._y -= _scrollBar->getPos();
	frameRect._lowerRightCorner._y -= _scrollBar->getPos();

	bool hl = (_highlightWhenNotFocused || isFocus() || _scrollBar->isFocus());

	for (int i = 0; i < (int)_items.size(); ++i)
	{
		if (frameRect._lowerRightCorner._y >= _absoluteRect._upperLeftCorner._y &&
			frameRect._upperLeftCorner._y <= _absoluteRect._lowerRightCorner._y)
		{
			if (i == _selected && hl)
				skin->draw2DRect(skin->getColor(EUIC_HIGH_LIGHT), frameRect, &clientClip);

			Recti textRect = frameRect;
			textRect._upperLeftCorner._x += 3;

			if (font)
			{
				textRect._upperLeftCorner._x += _itemsIconWidth + 3;

				if (i == _selected && hl)
				{
					font->renderText(_items[i].Text, textRect,
						hasItemColor(i, EUILBC_TEXT_HIGHLIGHT) ?
						getItemColor(i, EUILBC_TEXT_HIGHLIGHT) : getItemDefaultColor(EUILBC_TEXT_HIGHLIGHT),
						EUIA_LEFT_OR_UP, EUIA_CENTER, _textScale, &clientClip);
				}
				else
				{
					font->renderText(_items[i].Text, textRect,
						hasItemColor(i, EUILBC_TEXT) ?
						getItemColor(i, EUILBC_TEXT) : getItemDefaultColor(EUILBC_TEXT),
						EUIA_LEFT_OR_UP, EUIA_CENTER, _textScale, &clientClip);
				}

				textRect._upperLeftCorner._x -= _itemsIconWidth + 3;
			}
		}

		frameRect._upperLeftCorner._y += _itemHeight;
		frameRect._lowerRightCorner._y += _itemHeight;
	}

	if (_scrollBar->isVisible())
		_scrollBar->render();
	UIControl::render();
}

void UIListBox::clear()
{
	_items.clear();
	_itemsIconWidth = 0;
	_selected = -1;

	if (_scrollBar)
		_scrollBar->setPos(0);

	recalculateItemHeight();
}

void UIListBox::updateAbsoluteRect()
{
	UIControl::updateAbsoluteRect();
	UISkin* skin = Root::getSingleton().getUIManager()->getSkin();
	const int s = skin->getSize(EUIS_SCROLLBAR_SIZE);
	Recti rc(_absoluteRect.getWidth() - s, 0, _absoluteRect.getWidth(), _absoluteRect.getHeight());
	rc += _absoluteRect._upperLeftCorner;
	_scrollBar->setRectArea(rc._upperLeftCorner._x, rc._upperLeftCorner._y, rc.getWidth(), rc.getHeight());
	_scrollBar->showWidget(false);
	_scrollBar->setPos(0);
	_scrollBar->updateAbsoluteRect();
	recalculateItemHeight();
}

void UIListBox::touchBegan(const Touch& touch)
{
	if (!Root::getSingleton().getUIManager()->isEditMode())
		_scrollBar->touchBegan(touch);

	UIControl::touchBegan(touch);
	if (Root::getSingleton().getUIManager()->isEditMode())
		return;

	_selecting = true;
}

void UIListBox::touchMoved(const Touch& touch)
{
	if (!Root::getSingleton().getUIManager()->isEditMode())
		_scrollBar->touchMoved(touch);

	UIControl::touchMoved(touch);
	if (Root::getSingleton().getUIManager()->isEditMode())
		return;

	if (_selecting || _moveOverSelect)
	{
		Position2di p = touch.getCurPos();
		if (_absoluteRect.isPointInside(p))
			selectNew(p._y, true);
	}
}

void UIListBox::touchEnded(const Touch& touch)
{
	if (!Root::getSingleton().getUIManager()->isEditMode())
		_scrollBar->touchEnded(touch);

	UIControl::touchEnded(touch);
	if (Root::getSingleton().getUIManager()->isEditMode())
		return;

	_selecting = false;
	Position2di p = touch.getCurPos();
	if (_absoluteRect.isPointInside(p))
		selectNew(p._y);
}

bool UIListBox::touchWheel(const Touch& touch, float wheel)
{
	if (Root::getSingleton().getUIManager()->isEditMode())
		return false;

	Position2di p = touch.getCurPos();
	_scrollBar->setPos(_scrollBar->getPos() + (wheel < 0 ? -1 : 1) * -_itemHeight / 2);
	return true;
}

void UIListBox::serialize(XMLElement* outXml)
{
	UIControl::serialize(outXml);
}

bool UIListBox::deserialize(XMLElement* inXml)
{
	if (!UIControl::deserialize(inXml))
		return false;

	return true;
}

void UIListBox::setItemHeight(int height)
{
	_itemHeight = height;
	_itemHeightOverride = true;
}

void UIListBox::setDrawBg(bool drawBg)
{
	_drawBg = drawBg;
}

int UIListBox::getItemCount() const
{
	return (int)_items.size();
}

int UIListBox::getSelected() const
{
	return _selected;
}

void UIListBox::setSelected(int id)
{
	if ((unsigned int)id >= _items.size())
		_selected = -1;
	else
		_selected = id;

	_selectTime = Root::getSingleton().getTimer().totalMilliTime();
	recalculateScrollPos();
}

void UIListBox::setSelected(const wstring& item)
{
	int index = -1;
	if (item != L"")
	{
		for (index = 0; index < (int)_items.size(); ++index)
		{
			if (_items[index].Text == item)
				break;
		}
	}
	setSelected(index);
}

int UIListBox::getIcon(unsigned int id) const
{
	if (id >= _items.size())
		return -1;

	return _items[id].Icon;
}

void UIListBox::removeItem(unsigned int id)
{
	if (id >= _items.size())
		return;

	if ((unsigned int)_selected == id)
		_selected = -1;
	else if ((unsigned int)_selected > id)
	{
		_selected -= 1;
		_selectTime = Root::getSingleton().getTimer().totalMilliTime();
	}

	vector<ListItem>::iterator it = _items.begin() + id;
	_items.erase(it);
	recalculateItemHeight();
}

int UIListBox::getItemAt(int xpos, int ypos) const
{
	if (xpos < _absoluteRect._upperLeftCorner._x || xpos >= _absoluteRect._lowerRightCorner._x
		|| ypos < _absoluteRect._upperLeftCorner._y || ypos >= _absoluteRect._lowerRightCorner._y)
		return -1;

	if (_itemHeight == 0)
		return -1;

	int item = ((ypos - _absoluteRect._upperLeftCorner._y - 1) + _scrollBar->getPos()) / _itemHeight;
	if (item < 0 || item >= (int)_items.size())
		return -1;

	return item;
}

const wchar_t* UIListBox::getListItem(unsigned int id) const
{
	if (id >= _items.size())
		return L"";

	return _items[id].Text.c_str();
}

unsigned int UIListBox::addItem(const wstring& text)
{
	return addItem(text, -1);
}

unsigned int UIListBox::addItem(const wstring& text, int icon)
{
	ListItem i;
	i.Text = text;
	i.Icon = icon;
	_items.push_back(i);
	recalculateItemHeight();
	recalculateItemWidth(icon);
	return _items.size() - 1;
}

void UIListBox::setItem(unsigned int index, const wstring& text, int icon)
{
	if (index >= _items.size())
		return;

	_items[index].Text = text;
	_items[index].Icon = icon;

	recalculateItemHeight();
	recalculateItemWidth(icon);
}

void UIListBox::insertItem(unsigned int index, const wstring& text, int icon)
{
	ListItem i;
	i.Text = text;
	i.Icon = icon;

	vector<ListItem>::iterator it = _items.begin() + index;
	_items.insert(it, i);
	recalculateItemHeight();
	recalculateItemWidth(icon);
}

void UIListBox::swapItems(unsigned int index1, unsigned int index2)
{
	if (index1 >= _items.size() || index2 >= _items.size())
		return;

	ListItem dummy = _items[index1];
	_items[index1] = _items[index2];
	_items[index2] = dummy;
}

void UIListBox::setItemColor(unsigned int index, Colour color)
{
	for (unsigned int c = 0; c < EUILBC_COUNT; ++c)
	{
		_items[index].Colors[c].Use = true;
		_items[index].Colors[c].Color = color;
	}
}

void UIListBox::setItemColor(unsigned int index, EUIListBoxColor colorType, Colour color)
{
	if (index >= _items.size() || colorType < EUILBC_TEXT || colorType >= EUILBC_COUNT)
		return;

	_items[index].Colors[colorType].Use = true;
	_items[index].Colors[colorType].Color = color;
}

void UIListBox::clearItemColor(unsigned int index)
{
	for (unsigned int c = 0; c < (unsigned int)EUILBC_COUNT; ++c)
		_items[index].Colors[c].Use = false;
}

void UIListBox::clearItemColor(unsigned int index, EUIListBoxColor colorType)
{
	if (index >= _items.size() || colorType < EUILBC_TEXT || colorType >= EUILBC_COUNT)
		return;

	_items[index].Colors[colorType].Use = false;
}

bool UIListBox::hasItemColor(unsigned int index, EUIListBoxColor colorType) const
{
	if (index >= _items.size() || colorType < EUILBC_TEXT || colorType >= EUILBC_COUNT)
		return false;

	return _items[index].Colors[colorType].Use;
}

Colour UIListBox::getItemColor(unsigned int index, EUIListBoxColor colorType) const
{
	if ((unsigned int)index >= _items.size() || colorType < EUILBC_TEXT || colorType >= EUILBC_COUNT)
		return Colour();

	return _items[index].Colors[colorType].Color;
}

Colour UIListBox::getItemDefaultColor(EUIListBoxColor colorType) const
{
	UISkin* skin = Root::getSingleton().getUIManager()->getSkin();
	if (!skin)
		return Colour();

	switch (colorType)
	{
	case EUILBC_TEXT: return skin->getColor(EUIC_BUTTON_TEXT);
	case EUILBC_TEXT_HIGHLIGHT: return skin->getColor(EUIC_HIGH_LIGHT_TEXT);
	case EUILBC_ICON: return skin->getColor(EUIC_ICON);
	case EUILBC_ICON_HIGHLIGHT: return skin->getColor(EUIC_ICON_HIGH_LIGHT);
	}

	return Colour();
}

void UIListBox::selectNew(int ypos, bool onlyHover /*= false*/)
{
	unsigned int now = Root::getSingleton().getTimer().totalMilliTime();
	int oldSelected = _selected;

	_selected = getItemAt(_absoluteRect._upperLeftCorner._x, ypos);
	if (_selected < 0 && !_items.empty())
		_selected = 0;

	recalculateScrollPos();

	bool selAgain = (_selected == oldSelected && now < _selectTime + 500);
	_selectTime = now;
	if (!onlyHover)
	{
		if (selAgain)
			SignalTrigger::emit(_listBoxSelectedAgain);
		else
			SignalTrigger::emit(_listBoxChanged);
	}
}

void UIListBox::recalculateScrollPos()
{
	if (!_autoScroll)
		return;

	const int selPos = (_selected == -1 ? _totalItemHeight : _selected * _itemHeight) - _scrollBar->getPos();

	if (selPos < 0)
		_scrollBar->setPos(_scrollBar->getPos() + selPos);
	else if (selPos > _absoluteRect.getHeight() - _itemHeight)
		_scrollBar->setPos(_scrollBar->getPos() + selPos - _absoluteRect.getHeight() + _itemHeight);
}

void UIListBox::recalculateItemWidth(int icon)
{
	// do nothing.
}

void UIListBox::recalculateItemHeight()
{
	UIManager* uiMgr = Root::getSingleton().getUIManager();
	UIFont* font = uiMgr->getFont(EUIFT_DEFAULT);

	if (!_itemHeightOverride)
		_itemHeight = 0;

	if (font && !_itemHeightOverride)
		_itemHeight = font->getFontSize() + font->getKerningSize()._height * 5;

	_totalItemHeight = _itemHeight * _items.size();
	_scrollBar->setMax(max_(0, _totalItemHeight - _absoluteRect.getHeight()));
	int minItemHeight = _itemHeight > 0 ? _itemHeight : 1;
	_scrollBar->setSmallStep(minItemHeight);
	_scrollBar->setLargeStep(2 * minItemHeight);

	if (_totalItemHeight <= _absoluteRect.getHeight())
		_scrollBar->showWidget(false);
	else
		_scrollBar->showWidget(true);
}

//////////////////////////////////////////////////////////////////////////
VAYO_REFLEX_IMPLEMENT(UIComboBox)
UIComboBox::UIComboBox()
{
}

UIComboBox::~UIComboBox()
{
}

void UIComboBox::render()
{
}

//////////////////////////////////////////////////////////////////////////
Reflex<UIDialog, const wstring&> UIDialog::_dynReflex;
UIDialog::UIDialog(const wstring& fileName)
	: _lastSelectedCtrl(NULL)
{
	wstring fullPath = ConfigManager::getSingleton().getConfig()._3d.dlgXmlPath + fileName;
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(w2a_(fullPath).c_str()) != tinyxml2::XML_SUCCESS)
		return;

	XMLElement* pRoot = doc.RootElement();
	if (NULL == pRoot)
		return;

	if (deserialize(pRoot))
	{
		_xmlFilePath = fullPath;
		showWidget(false);
		setOutlineColor(Colour(255, 0, 100, 255));
		Root::getSingleton().getUIManager()->addDialog(this);
	}
}

UIDialog::~UIDialog()
{
	list<UIControl*>::iterator itor = _controls.begin();
	for (; itor != _controls.end(); ++itor)
	{
		SAFE_DELETE(*itor);
	}
	_controls.clear();
	Root::getSingleton().getUIManager()->removeDialog(this);
}

void UIDialog::open(bool bBlockMode/*=true*/)
{
	showWidget(true);
	if (bBlockMode)
		Root::getSingleton().getUIManager()->beginPopup(this);
}

void UIDialog::close()
{
	showWidget(false);
	Root::getSingleton().getUIManager()->endPopup(this);
}

void UIDialog::render()
{
	list<UIControl*>::iterator itor = _controls.begin();
	for (; itor != _controls.end(); ++itor)
	{
		if ((*itor)->isVisible())
		{
			(*itor)->render();
		}
	}
	
	if (Root::getSingleton().getUIManager()->isEditMode())
	{
		if (isFocus())
		{
			wstring xmlPath = getXMLFilePath();
			xmlPath = xmlPath.substr(xmlPath.rfind(L'\\') + 1);

			wchar_t szTemp[256] = {};
			UIFont* pFont = Root::getSingleton().getUIManager()->getFont(EUIFT_TOOLTIP);
			swprintf(szTemp, 256, L"%s,Rect[%d %d %d %d]Path[%s]",
				a2w_(getType()).c_str(),
				getAnimatedRelativeRect()._upperLeftCorner._x,
				getAnimatedRelativeRect()._upperLeftCorner._y,
				getAnimatedRelativeRect().getWidth(),
				getAnimatedRelativeRect().getHeight(),
				xmlPath.c_str());

			Vector2di tipFontDrawArea = getAbsoluteRect()._upperLeftCorner;
			pFont->drawTextBegin();
			pFont->drawText(tipFontDrawArea._x, tipFontDrawArea._y, szTemp, 1.0f, 0xffc8c8c8);
			pFont->drawTextEnd();
		}
		UIWidget::render();
	}
}

void UIDialog::handleInput()
{
	// do nothing.
}

void UIDialog::center(bool bXPos/*=true*/, bool bYPos/*=true*/)
{
	const Dimension2di& screenSize = Root::getSingleton().getActiveDevice()->getScreenSize();
	int dialogW = getRelativeRect().getWidth();
	int dialogH = getRelativeRect().getHeight();
	int xpos = bXPos ? ((int)screenSize._width - dialogW) / 2 : getRelativeRect()._upperLeftCorner._x;
	int yPos = bYPos ? ((int)screenSize._height - dialogH) / 2 : getRelativeRect()._upperLeftCorner._y;
	moveTo(xpos, yPos);
}

UIControl* UIDialog::getCtrl(int id) const
{
	list<UIControl*>::const_iterator itor = _controls.cbegin();
	for (; itor != _controls.cend(); ++itor)
	{
		if (id == (*itor)->getID())
		{
			return *itor;
		}
	}
	return NULL;
}

bool UIDialog::removeCtrl(UIControl* pCtrl)
{
	if (NULL == pCtrl)
		return false;
	list<UIControl*>::iterator itor = std::find(_controls.begin(), _controls.end(), pCtrl);
	if (itor != _controls.end())
	{
		SAFE_DELETE(*itor);
		_controls.erase(itor);
		return true;
	}
	return false;
}

bool UIDialog::removeCtrl(int id)
{
	list<UIControl*>::iterator itor = _controls.begin();
	for (; itor != _controls.end(); ++itor)
	{
		if (id == (*itor)->getID())
		{
			SAFE_DELETE(*itor);
			_controls.erase(itor);
			return true;
		}
	}
	return false;
}

void UIDialog::setFocus(bool bFocus)
{
	if (isFocus() == bFocus || !isActive() || !isVisible())
		return;

	UIWidget::setFocus(bFocus);

	if (bFocus)
	{
		setOutlineColor(Colour(255, 240, 100, 0));
		Root::getSingleton().getUIManager()->notifyDlgCancelSelectedExcept(this);
	}
	else
	{
		setOutlineColor(Colour(255, 0, 100, 255));
		list<UIControl*>::reverse_iterator itor = _controls.rbegin();
		for (; itor != _controls.rend(); ++itor)
		{
			if ((*itor)->isFocus())
				_lastSelectedCtrl = *itor;
			(*itor)->setFocus(false);
		}
	}
}

void UIDialog::updateAbsoluteRect()
{
	_absoluteRect = _animatedRelativeRect;
	list<UIControl*>::iterator itor = _controls.begin();
	for (; itor != _controls.end(); ++itor)
	{
		(*itor)->updateAbsoluteRect();
	}
}

void UIDialog::touchBegan(const Touch& touch)
{
	list<UIControl*>::reverse_iterator itor = _controls.rbegin();
	for (; itor != _controls.rend(); ++itor)
	{
		if ((*itor)->isFocus())
		{
			(*itor)->touchBegan(touch);
			return;
		}
	}
	UIWidget::touchBegan(touch);
}

void UIDialog::touchMoved(const Touch& touch)
{
	list<UIControl*>::reverse_iterator itor = _controls.rbegin();
	for (; itor != _controls.rend(); ++itor)
	{
		if ((*itor)->isFocus())
		{
			(*itor)->touchMoved(touch);
			return;
		}
	}
	UIWidget::touchMoved(touch);
}

void UIDialog::touchEnded(const Touch& touch)
{
	list<UIControl*>::reverse_iterator itor = _controls.rbegin();
	for (; itor != _controls.rend(); ++itor)
	{
		if ((*itor)->isFocus())
		{
			(*itor)->touchEnded(touch);
			return;
		}
	}
	UIWidget::touchEnded(touch);
}

bool UIDialog::touchWheel(const Touch& touch, float wheel)
{
	list<UIControl*>::reverse_iterator itor = _controls.rbegin();
	for (; itor != _controls.rend(); ++itor)
	{
		if ((*itor)->isFocus())
		{
			return (*itor)->touchWheel(touch, wheel);
		}
	}
	return UIWidget::touchWheel(touch, wheel);
}

bool UIDialog::keyClicked(const tagKeyInput& keyInput)
{
	list<UIControl*>::reverse_iterator itor = _controls.rbegin();
	for (; itor != _controls.rend(); ++itor)
	{
		if ((*itor)->isFocus())
		{
			return (*itor)->keyClicked(keyInput);
		}
	}
	return UIWidget::keyClicked(keyInput);
}

void UIDialog::serialize(XMLElement* outXml)
{
	char buffer[256] = {};
	Recti animRc = getAnimatedRelativeRect();
	sprintf_s(buffer, sizeof(buffer), "%d,%d,%d,%d", animRc._upperLeftCorner._x,
		animRc._upperLeftCorner._y,
		animRc.getWidth(),
		animRc.getHeight());
	outXml->SetAttribute("rect", buffer);
	sprintf_s(buffer, sizeof(buffer), "%d", _imgsetID);
	outXml->SetAttribute("imageset", buffer);
	outXml->SetAttribute("image", w2a_(_image).c_str());
}

bool UIDialog::deserialize(XMLElement* inXml)
{
	vector<string> container;
	string strArea = inXml->Attribute("rect");
	stringtok(container, strArea, ",");
	if (container.size() >= 4)
	{
		int x = atoi(container[0].c_str());
		int y = atoi(container[1].c_str());
		int width = atoi(container[2].c_str());
		int height = atoi(container[3].c_str());
		setRectArea(x, y, width, height);
	}

	setImgsetID(atoi(inXml->Attribute("imageset")));
	setImage(utf8ToUnicode(inXml->Attribute("image")));

	XMLElement* pElem = inXml->FirstChildElement();
	while (pElem)
	{
		UIControl* pCtrl = ReflexFactory<>::getInstance().create<UIControl>(pElem->Name());
		if (NULL != pCtrl)
		{
			pCtrl->setParent(this);
			if (pCtrl->deserialize(pElem))
			{
				_controls.push_back(pCtrl);
			}
			else
			{
				SAFE_DELETE(pCtrl);
			}
		}

		pElem = pElem->NextSiblingElement();
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
UIManager::UIManager(Device* dev)
	: TouchDelegate(dev->getDeviceCode())
	, KeypadDelegate(dev->getDeviceCode())
	, _curDrawImage(NULL)
	, _currentSkin(NULL)
	, _clipArea(NULL)
{
	memset(_fontArr, 0, sizeof(_fontArr));
	memset(_imageSetArr, 0, sizeof(_imageSetArr));
	enableTouch(true, 0);
	enableKeypad(true, 0);
}

UIManager::~UIManager()
{
	setSkin(NULL);
	SAFE_DELETE(s_fontManager);

	for (int i = 0; i < EUIFT_COUNT; ++i)
	{
		SAFE_DELETE(_fontArr[i]);
	}

	for (int i = 0; i < VAYO_MAX_IMAGESET_NUM; i++)
	{
		SAFE_DELETE(_imageSetArr[i]);
	}
}

bool UIManager::init()
{
	const Config& config = ConfigManager::getSingleton().getConfig();
	do
	{
		// loading sequence frame animation.
		tinyxml2::XMLDocument doc;
		if (doc.LoadFile(w2a_(config._3d.seqAnimPath).c_str()) != tinyxml2::XML_SUCCESS)
		{
			Log::wprint(ELL_ERROR, L"序列帧动画[%s]加载失败", config._3d.seqAnimPath.c_str());
			break;
		}

		XMLElement* pRoot = doc.RootElement();
		if (NULL == pRoot)
		{
			Log::wprint(ELL_ERROR, L"序列帧动画[%s]解析失败", config._3d.seqAnimPath.c_str());
			break;
		}

		_arrAnimSet.clear();

		XMLElement* pAnim = pRoot->FirstChildElement("anim");
		while (pAnim)
		{
			tagAnim curAnimFrame;
			curAnimFrame.AnimID = pAnim->IntAttribute("ID");
			_arrAnimSet.push_back(curAnimFrame);
			XMLElement* pKeyFrame = pAnim->FirstChildElement("keyframe");
			while (pKeyFrame)
			{
				tagAnim::tagKeyFrame keyFrame;
				keyFrame.SrcImageSetID = pKeyFrame->IntAttribute("srcImageSetID");
				keyFrame.IconName = utf8ToUnicode(pKeyFrame->Attribute("iconName"));
				keyFrame.Time = pKeyFrame->IntAttribute("time");
				keyFrame.Scale = pKeyFrame->FloatAttribute("scale");
				keyFrame.XOffset = pKeyFrame->FloatAttribute("xoffset");
				keyFrame.YOffset = pKeyFrame->FloatAttribute("yoffset");
				keyFrame.ScaleByCenter = (0 == pKeyFrame->IntAttribute("scaleByCenter") ? false : true);

				tagAnim& curAnimFrame = _arrAnimSet.back();
				curAnimFrame.ArrKeyFrame.push_back(keyFrame);

				pKeyFrame = pKeyFrame->NextSiblingElement();
			}
			pAnim = pAnim->NextSiblingElement();
		}
	} while (0);

	// loading imageset.
	unordered_map<int, wstring>::const_iterator itor = config._3d.imgsets.cbegin();
	for (; itor != config._3d.imgsets.cend(); ++itor)
	{
		int idx = itor->first;
		if (idx < 0 || idx >= VAYO_MAX_IMAGESET_NUM)
			continue;

		UIImageSet* pImgSet = UIImageSet::createImgset(itor->second);
		if (NULL == pImgSet)
			continue;

		_imageSetArr[idx] = pImgSet;
	}

	// loading font.
	unsigned int fontIdx = 0;
	if (NULL == s_fontManager)
		s_fontManager = new FontManager();
	s_fontManager->initialize();
	const vector<Config::Font>& fontsSet = config._3d.fontset;
	vector<Config::Font>::const_iterator cit = fontsSet.cbegin();
	for (; cit != fontsSet.cend(); ++cit)
	{
		if (fontIdx >= EUIFT_COUNT)
			break;

		const Config::Font& curFont = *cit;
		string fontPath = w2a_(curFont.filePath);
		int fontID = s_fontManager->createFont(fontPath.c_str(), 0, curFont.size, curFont.bold, curFont.italic, true);
		if (fontID == -1)
		{
			Log::print(ELL_ERROR, "createFont %s failed", fontPath.c_str());
			return false;
		}

		_fontArr[fontIdx] = new UIFont(fontID);
		++fontIdx;
	}

	// loading skin.
	setSkin(new UISkin(config._3d.skinFilePath));

	return true;
}

void UIManager::render()
{
	UIDialog* pDlg = NULL;
	list<UIDialog*>::iterator itor = _dialogs.begin();
	for (; itor != _dialogs.end(); ++itor)
	{
		pDlg = (*itor);
		if (pDlg->isVisible())
		{
			pDlg->render();
		}
	}
}

bool UIManager::isEditMode() const
{
	return _UIStateMark.checkState(EUI_EDIT_MODE_OPEN);
}

bool UIManager::addDialog(UIDialog* pDlg)
{
	if (NULL == pDlg)
		return false;

	list<UIDialog*>::iterator itor = std::find(_dialogs.begin(), _dialogs.end(), pDlg);
	if (itor != _dialogs.end())
		return true;

	_dialogs.push_back(pDlg);
	return true;
}

bool UIManager::removeDialog(UIDialog* pDlg)
{
	if (NULL == pDlg)
		return false;

	list<UIDialog*>::iterator itList = std::find(_dialogs.begin(), _dialogs.end(), pDlg);
	if (itList != _dialogs.end())
	{
		_dialogs.erase(itList);

		deque<UIDialog*>::iterator itDeque = std::find(_popupDlgs.begin(), _popupDlgs.end(), pDlg);
		if (itDeque != _popupDlgs.end())
			_popupDlgs.erase(itDeque);

		return true;
	}

	return false;
}

void UIManager::beginPopup(UIDialog* pDlg)
{
	if (NULL == pDlg)
		return;

	list<UIDialog*>::iterator itList = std::find(_dialogs.begin(), _dialogs.end(), pDlg);
	if (itList != _dialogs.end())
	{
		_dialogs.erase(itList);
		_dialogs.push_back(pDlg);
		deque<UIDialog*>::iterator itDeque = std::find(_popupDlgs.begin(), _popupDlgs.end(), pDlg);
		if (itDeque != _popupDlgs.end())
			_popupDlgs.erase(itDeque);
		_popupDlgs.push_back(pDlg);
	}
}

void UIManager::endPopup(UIDialog* pDlg)
{
	if (_popupDlgs.empty() || _popupDlgs.back() != pDlg)
		return;
	_popupDlgs.pop_back();
}

UIDialog* UIManager::getPopupDialog()
{
	if (_popupDlgs.empty())
		return NULL;
	return _popupDlgs.back();
}

UIFont* UIManager::getFont(int fontID)
{
	if (fontID < 0 || fontID >= EUIFT_COUNT)
		return NULL;
	return _fontArr[fontID];
}

UIImageSet* UIManager::getImageSet(int imgsetID)
{
	if (imgsetID < 0 || imgsetID >= VAYO_MAX_IMAGESET_NUM)
		return NULL;
	return _imageSetArr[imgsetID];
}

UISkin* UIManager::getSkin() const
{
	return _currentSkin;
}

void UIManager::setSkin(UISkin* skin)
{
	SAFE_DELETE(_currentSkin);
	_currentSkin = skin;
}

void UIManager::showAllDialog(bool bShow)
{
	list<UIDialog*>::iterator itor = _dialogs.begin();
	for (; itor != _dialogs.end(); itor++)
	{
		(*itor)->showWidget(bShow);
	}
}

void UIManager::notifyDlgCancelSelectedExcept(UIDialog* pDlg)
{
	list<UIDialog*>::reverse_iterator itor = _dialogs.rbegin();
	for (; itor != _dialogs.rend(); ++itor)
	{
		if (pDlg == (*itor))
			continue;
		(*itor)->setFocus(false);
	}
}

bool UIManager::touchBegan(const Touch& touch, EMouseKeys key)
{
	if (key == EMK_LEFT)
	{
		allotTouch(ETT_BEGAN, touch);
		return isEditMode();
	}
	return false;
}

void UIManager::touchMoved(const Touch& touch, EMouseKeys key)
{
	if (key == EMK_LEFT)
		allotTouch(ETT_MOVED, touch);
}

void UIManager::touchEnded(const Touch& touch, EMouseKeys key)
{
	if (key == EMK_LEFT)
		allotTouch(ETT_ENDED, touch);
}

bool UIManager::touchWheel(const Touch& touch, float wheel)
{
	if (isEditMode())
		return true;
	UIDialog* pCurSelectedDlg = NULL;
	list<UIDialog*>::iterator itor = _dialogs.begin();
	for (; itor != _dialogs.end(); ++itor)
	{
		UIDialog* pDlg = (*itor);
		if (pDlg->isFocus() && pDlg->isVisible())
		{
			pCurSelectedDlg = pDlg;
			break;
		}
	}
	if (pCurSelectedDlg)
		return pCurSelectedDlg->touchWheel(touch, wheel);
	return false;
}

bool UIManager::keyClicked(const tagKeyInput& keyInput)
{
	// Ctrl+Shift+O: 编辑模式切换
	do 
	{
		if (KEY_KEY_O != keyInput.Key)
			break;

		if (!keyInput.PressedDown)
		{
			_UIStateMark.eraseState(EUI_EDIT_MODE_SWITCH);
			break;
		}

		if (keyInput.Control && keyInput.Shift && !_UIStateMark.checkState(EUI_EDIT_MODE_SWITCH))
		{
			_UIStateMark.addState(EUI_EDIT_MODE_SWITCH);
			if (_UIStateMark.checkState(EUI_EDIT_MODE_OPEN))
			{
				_UIStateMark.eraseState(EUI_EDIT_MODE_OPEN);
				Log::print(ELL_INFORMATION, "编辑模式关闭");
			}
			else
			{
				_UIStateMark.addState(EUI_EDIT_MODE_OPEN);
				Log::print(ELL_INFORMATION, "编辑模式开启");
			}

			return true; // 返回true.
		}

	} while (0);

	UIDialog* pCurSelectedDlg = NULL;
	list<UIDialog*>::iterator itor = _dialogs.begin();
	for (; itor != _dialogs.end(); ++itor)
	{
		UIDialog* pDlg = (*itor);
		if (pDlg->isFocus() && pDlg->isVisible())
		{
			pCurSelectedDlg = pDlg;
			break;
		}
	}

	// Ctrl+Shift+P: 保存对话框的XML配置文件
	do 
	{
		if (!_UIStateMark.checkState(EUI_EDIT_MODE_OPEN))
			break;

		if (KEY_KEY_P != keyInput.Key)
			break;

		if (!keyInput.PressedDown)
		{
			_UIStateMark.eraseState(EUI_EDIT_MODE_SAVE);
			break;
		}

		if (keyInput.Control && keyInput.Shift && !_UIStateMark.checkState(EUI_EDIT_MODE_SAVE))
		{
			_UIStateMark.addState(EUI_EDIT_MODE_SAVE);

			if (NULL == pCurSelectedDlg)
			{
				Log::print(ELL_WARNING, "No selected dialog!");
				break;
			}

			// save widget rect area.
			tinyxml2::XMLDocument doc;
			doc.InsertEndChild(doc.NewDeclaration());
			XMLElement* root = doc.NewElement(pCurSelectedDlg->getType().c_str());
			doc.InsertEndChild(root);
			pCurSelectedDlg->serialize(root);

			UIControl* pCtrl = NULL;
			list<UIControl*>::const_iterator constItor = pCurSelectedDlg->getCtrlList().cbegin();
			list<UIControl*>::const_iterator constItorEnd = pCurSelectedDlg->getCtrlList().cend();
			for (; constItor != constItorEnd; ++constItor)
			{
				pCtrl = (*constItor);
				XMLElement* pCtrlElem = doc.NewElement(pCtrl->getType().c_str());
				root->InsertEndChild(pCtrlElem);
				pCtrl->serialize(pCtrlElem);
			}

			if (tinyxml2::XML_SUCCESS != doc.SaveFile(w2a_(pCurSelectedDlg->getXMLFilePath()).c_str()))
			{
				Log::wprint(ELL_ERROR, L"保存%s失败", pCurSelectedDlg->getXMLFilePath().c_str());
				return false;
			}

			Log::wprint(ELL_INFORMATION, L"保存%s成功", pCurSelectedDlg->getXMLFilePath().c_str());
			return true; // 返回true.
		}

	} while (0);

	if (pCurSelectedDlg)
		return pCurSelectedDlg->keyClicked(keyInput);
	return false;
}

void UIManager::allotTouch(ETouchType touchType, const Touch& touch)
{
	UIDialog* pCurDialog = NULL;
	bool isHasPopupDialogMark = (NULL != getPopupDialog());

	list<UIDialog*>::reverse_iterator itor = _dialogs.rbegin();
	for (; itor != _dialogs.rend(); ++itor)
	{
		pCurDialog = *itor;
		if (isHasPopupDialogMark)
		{
			pCurDialog = getPopupDialog();
		}

		if (pCurDialog->isActive() || isEditMode())
		{
			if (ETT_BEGAN == touchType)
			{
				confirmWhichSelected(pCurDialog, touch);
			}

			if (pCurDialog->isFocus())
			{
				switch (touchType)
				{
				case ETT_BEGAN:pCurDialog->touchBegan(touch);break;
				case ETT_MOVED:pCurDialog->touchMoved(touch);break;
				case ETT_ENDED:
					{
						pCurDialog->touchEnded(touch);
						if (!isEditMode())
							pCurDialog->handleInput();
					}
					break;
				default:break;
				}
				break;
			}
		}

		if (isHasPopupDialogMark)
		{
			break;
		}
	}
}

void UIManager::confirmWhichSelected(UIDialog* pDialog, const Touch& touch)
{
	if (NULL == pDialog)
		return; // 返回

	pDialog->setFocus(false);
	UIControl* pLastSelectedCtrl = pDialog->getLastSelectedCtrl();
	bool bIsInLastSelectedCtrl = false;
	vector<UIControl*> selectedCtrlQueue;
	UIControl* pCurCtrl = NULL;

	const list<UIControl*>& ctrlList = pDialog->getCtrlList();
	list<UIControl*>::const_reverse_iterator itor = ctrlList.crbegin();
	for (; itor != ctrlList.crend(); ++itor)
	{
		pCurCtrl = *itor;
		if (!pCurCtrl->isActive() || !pCurCtrl->isVisible())
			continue;

		do
		{
			if (!pCurCtrl->getAbsoluteRect().isPointInside(touch.getCurPos()))
				break;

			if (isEditMode())
			{
				selectedCtrlQueue.push_back(pCurCtrl);
				if (pCurCtrl == pLastSelectedCtrl)
					bIsInLastSelectedCtrl = true; // 置true说明既选中了该控件(上一个选中控件),又说明了该控件在选中控件队列里面
				break;
			}

			if (pCurCtrl->isCanBeActive())
			{
				pCurCtrl->setFocus(true);
				pDialog->setFocus(true);
				return; // 返回
			}

		} while (0);
	}

	unsigned len = selectedCtrlQueue.size();
	if (len > 0)
	{
		if (!bIsInLastSelectedCtrl)
		{
			selectedCtrlQueue[0]->setFocus(true);
			pDialog->setFocus(true);
			return; // 返回
		}

		// 上一个选中控件在队列里面,所以要循环查找下一个要点击的控件
		for (unsigned i = 0u; i < len; i++)
		{
			if (selectedCtrlQueue[i] == pLastSelectedCtrl)
			{
				int curSelectedCtrlIdx = ((i + 1 >= len) ? (0) : (i + 1));
				selectedCtrlQueue[curSelectedCtrlIdx]->setFocus(true);
				pDialog->setFocus(true);
				return; // 返回
			}
		}
	}

	if (pDialog->getAbsoluteRect().isPointInside(touch.getCurPos()))
		pDialog->setFocus(true);
}

//////////////////////////////////////////////////////////////////////////
// 绘制相关
//////////////////////////////////////////////////////////////////////////
void UIManager::drawIconBegin(int nImgSetID)
{
	_curDrawImage = getImageSet(nImgSetID);
}

void UIManager::drawIcon(const wstring& iconName, const Recti& drawArea, const Colour* color/*=NULL*/)
{
	if (NULL == _curDrawImage)
		return;
	_destRects.push_back(drawArea);
	_sourceRects.push_back(_curDrawImage->getIconRect(iconName));
	_colors.push_back(NULL != color ? *color : 0xFFFFFFFF);
}

void UIManager::drawIconEnd()
{
	if (NULL == _curDrawImage)
		return;
	Root::getSingleton().getActiveRenderer()->draw2DImageBatch(_curDrawImage->getBindTexture(),
		_destRects, _sourceRects, _clipArea, &_colors, true);
	_destRects.clear();
	_sourceRects.clear();
	_colors.clear();
	_curDrawImage = NULL;
}

void UIManager::beginClipRender(Recti* clipArea)
{
	_clipArea = clipArea;
}

void UIManager::endClipRender()
{
	_clipArea = NULL;
}

void UIManager::drawSequenceAnim(float x, float y, tagSequecenAnim& seqAnim, float scale/*=1.0f*/, unsigned color/*=0xffffffff*/)
{
	const tagAnim* pCurAnim = NULL;
	unsigned len = _arrAnimSet.size();
	for (unsigned i = 0; i < len; ++i)
	{
		if (seqAnim.AnimID != _arrAnimSet[i].AnimID)
			continue;

		pCurAnim = &_arrAnimSet[i];
		break;
	}

	if (NULL == pCurAnim || 0 == pCurAnim->ArrKeyFrame.size())
		return;

	len = pCurAnim->ArrKeyFrame.size();
	seqAnim.CurTime += (int)Root::getSingleton().getTimer().deltaMilliTime();

	int nCycleTime = pCurAnim->ArrKeyFrame.back().Time;
	int nIntervalTime = ( 1 == len ? 0 : (nCycleTime) / ((int)len-1) );
	int tempTime = seqAnim.CurTime - (seqAnim.Times * nCycleTime);
	unsigned idx = 0;

	unsigned i;
	for (i = 0; i < len; ++i)
	{
		if (tempTime < pCurAnim->ArrKeyFrame[i].Time)
		{
			idx = (i > 0 ? i - 1 : 0);
			break;
		}
	}

	if (0 == idx && i == len)
	{
		if (tempTime < nCycleTime + nIntervalTime)
		{
			idx = len - 1;
		}
		else
		{
			idx = 0;
			++seqAnim.Times;
		}
	}

	const tagAnim::tagKeyFrame curKeyFrame = pCurAnim->ArrKeyFrame[idx];
	UIImageSet* pCurDrawImage = getImageSet(curKeyFrame.SrcImageSetID);
	if (NULL == pCurDrawImage)
		return;

	Recti srcRect(pCurDrawImage->getIconRect(curKeyFrame.IconName));

	float xpos = x + curKeyFrame.XOffset;
	float ypos = y + curKeyFrame.YOffset;
	float oriWidth = (float)srcRect.getWidth();
	float oriHeight = (float)srcRect.getHeight();
	float width = oriWidth * curKeyFrame.Scale;
	float height = oriHeight * curKeyFrame.Scale;

	if (curKeyFrame.ScaleByCenter)
	{
		xpos += (oriWidth - width) / 2.0f;
		ypos += (oriHeight - height) / 2.0f;
	}

	Recti dstRect((int)xpos, (int)ypos, (int)(xpos + width), (int)(ypos + height));
	const Colour tempColors[4] = {color, color, color, color};

	Root::getSingleton().getActiveRenderer()->draw2DImage(pCurDrawImage->getBindTexture(), dstRect, srcRect, _clipArea, tempColors, true);
}

void UIManager::drawSequenceAnim(float x, float y, float width, float height, tagSequecenAnim& seqAnim, unsigned color/*=0xffffffff*/)
{
	const tagAnim* pCurAnim = NULL;
	unsigned len = _arrAnimSet.size();
	for (unsigned i = 0; i < len; ++i)
	{
		if (seqAnim.AnimID != _arrAnimSet[i].AnimID)
			continue;

		pCurAnim = &_arrAnimSet[i];
		break;
	}

	if (NULL == pCurAnim || 0 == pCurAnim->ArrKeyFrame.size())
		return;

	len = pCurAnim->ArrKeyFrame.size();
	seqAnim.CurTime += (int)Root::getSingleton().getTimer().deltaMilliTime();

	int nCycleTime = pCurAnim->ArrKeyFrame.back().Time;
	int nIntervalTime = ( 1 == len ? 0 : (nCycleTime) / ((int)len-1) );
	int tempTime = seqAnim.CurTime - (seqAnim.Times * nCycleTime);
	unsigned idx = 0;

	unsigned i;
	for (i = 0; i < len; ++i)
	{
		if (tempTime < pCurAnim->ArrKeyFrame[i].Time)
		{
			idx = (i > 0 ? i - 1 : 0);
			break;
		}
	}

	if (0 == idx && i == len)
	{
		if (tempTime < nCycleTime + nIntervalTime)
		{
			idx = len - 1;
		}
		else
		{
			idx = 0;
			++seqAnim.Times;
		}
	}

	const tagAnim::tagKeyFrame curKeyFrame = pCurAnim->ArrKeyFrame[idx];
	UIImageSet* pCurDrawImage = getImageSet(curKeyFrame.SrcImageSetID);
	if (NULL == pCurDrawImage)
		return;

	Recti srcRect(pCurDrawImage->getIconRect(curKeyFrame.IconName.c_str()));

	float xpos = x + curKeyFrame.XOffset;
	float ypos = y + curKeyFrame.YOffset;
	Recti dstRect((int)xpos, (int)ypos, (int)(xpos + width), (int)(ypos + height));
	const Colour tempColors[4] = {color, color, color, color};

	Root::getSingleton().getActiveRenderer()->draw2DImage(pCurDrawImage->getBindTexture(), dstRect, srcRect, _clipArea, tempColors, true);
}

NS_VAYO3D_END
