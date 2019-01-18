/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 文本对象
\*************************************************************************/
#ifndef __VAYO_TEXT_OBJECT_H__
#define __VAYO_TEXT_OBJECT_H__

#include "VayoMovableObject.h"
#include "VayoRenderable.h"
NS_VAYO_BEGIN

class _VayoExport TextObject : public MovableObject, public Renderable
{
	static Reflex<TextObject, const wstring&, SceneManager*> _dynReflex;
public:
	TextObject(const wstring& name, SceneManager* originSceneMgr);
	~TextObject();
	void update(float dt);
	void render();
	void setText(const wstring& text);
	void setTextColor(Colour color);
	void setFontID(int fontid);
	void getWorldTransform(Matrix4x4& mat) const;
	void serialize(XMLElement* outXml);
	bool deserialize(XMLElement* inXml);

private:
	wstring _text;
	Colour  _color;
	int     _fontid;
};

NS_VAYO_END

#endif // __VAYO_TEXT_OBJECT_H__
