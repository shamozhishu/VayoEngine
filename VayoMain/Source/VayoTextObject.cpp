#include "VayoTextObject.h"
#include "VayoRoot.h"
#include "VayoSceneManager.h"
#include "VayoCoreGUI.h"
#include "VayoCollision.h"
#include "VayoSceneNode.h"

NS_VAYO_BEGIN

void TextObject::setFontID(int fontid)
{
	_fontid = fontid;
}

TextObject::TextObject(const wstring& name)
	: MovableObject(name)
	, _fontid(0)
{
}

TextObject::~TextObject()
{
}

void TextObject::update(float dt)
{
	Root::getSingleton().getCurSceneMgr()->registerForRendering(this, getQueueID());
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

void TextObject::getWorldTransform(Matrix4x4& mat) const
{
	if (_parentNode)
		mat = _parentNode->getAbsTransformation();
	else
		Renderable::getWorldTransform(mat);
}

void TextObject::serialize(XMLElement* outXml)
{

}

bool TextObject::deserialize(XMLElement* inXml)
{
	return true;
}

NS_VAYO_END
