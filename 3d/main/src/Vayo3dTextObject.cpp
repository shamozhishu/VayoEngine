#include "Vayo3dTextObject.h"
#include "Vayo3dRoot.h"
#include "Vayo3dSceneManager.h"
#include "Vayo3dCoreGUI.h"
#include "Vayo3dCollision.h"
#include "Vayo3dSceneNode.h"
#include "VayoUtils.h"

NS_VAYO3D_BEGIN

Reflex<TextObject, const wstring&, SceneManager*> TextObject::_dynReflex;
TextObject::TextObject(const wstring& name, SceneManager* originSceneMgr)
	: MovableObject(name, originSceneMgr)
	, _fontid(0)
{
}

TextObject::~TextObject()
{
}

void TextObject::update(float dt)
{
	Root& root = Root::getSingleton();
	if (root.getUIManager())
		root.getCurSceneMgr()->registerForRendering(this, getQueueID());
}

void TextObject::render()
{
	UIFont* uiFont = Root::getSingleton().getUIManager()->getFont(_fontid);
	SceneManager* sceneMgr = Root::getSingleton().getCurSceneMgr();
	if (uiFont && sceneMgr)
	{
		CollisionDetector* coll = sceneMgr->getCollDetector();
		if (coll)
		{
			Matrix4x4 mat;
			getWorldTransform(mat);
			Position2di pos = coll->getScreenCoordinatesFrom3DPosition(mat.getTranslation(), sceneMgr->getActiveCamera());
			pos -= Position2di(1, 4);
			Recti rc(pos, Dimension2di(2, 4));
			uiFont->renderText(_text.c_str(), rc, _color, EUIA_CENTER, EUIA_CENTER);
		}
	}
}

void TextObject::setText(const wstring& text)
{
	_text = text;
}

void TextObject::setTextColor(Colour color)
{
	_color = color;
}

void TextObject::setFontID(int fontid)
{
	_fontid = fontid;
}

void TextObject::getWorldTransform(Matrix4x4& mat) const
{
	if (_parentNode)
		mat = _parentNode->getAbsTransformation();
	else
		Renderable::getWorldTransform(mat);
}

void TextObject::serialize(XMLElement* outXml)
{
	MovableObject::serialize(outXml);
	outXml->SetAttribute("text", unicodeToUtf8(_text).c_str());
	char szbuf[256];
	sprintf_s(szbuf, sizeof(szbuf), "0x%08x", _color._clr);
	outXml->SetAttribute("color", szbuf);
	outXml->SetAttribute("fontid", _fontid);
}

bool TextObject::deserialize(XMLElement* inXml)
{
	if (!MovableObject::deserialize(inXml))
		return false;

	const char* szTmp = inXml->Attribute("text");
	if (szTmp)
		_text = utf8ToUnicode(szTmp);

	szTmp = inXml->Attribute("color");
	if (szTmp)
		_color.set(strtoul(szTmp, NULL, 16));

	_fontid = inXml->IntAttribute("fontid");
	return true;
}

NS_VAYO3D_END
