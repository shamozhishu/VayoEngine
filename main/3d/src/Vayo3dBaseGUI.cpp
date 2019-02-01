#include "Vayo3dBaseGUI.h"
#include "Vayo3dRoot.h"
#include "VayoLog.h"
#include "VayoUtils.h"
#include "Vayo3dRenderSystem.h"

NS_VAYO3D_BEGIN

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
		Log::wprint(ELL_ERROR, L"GUIÆ¤·ôÎÄ¼þÂ·¾¶Îª¿Õ");
		return;
	}

	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(w2a_(skinFilePath).c_str()) != tinyxml2::XML_SUCCESS)
	{
		Log::wprint(ELL_ERROR, L"GUIÆ¤·ô[%s]¼ÓÔØÊ§°Ü", skinFilePath.c_str());
		return;
	}

	XMLElement* pRoot = doc.RootElement();
	if (NULL == pRoot)
	{
		Log::wprint(ELL_ERROR, L"GUIÆ¤·ô[%s]½âÎöÊ§°Ü", skinFilePath.c_str());
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

NS_VAYO3D_END
