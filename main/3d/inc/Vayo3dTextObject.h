/*************************************************************************\
* ��������1.0
* Copyright (c) 2018-2019 authored by ��Ӻ�
* �ı�����
\*************************************************************************/
#ifndef __VAYO3D_TEXT_OBJECT_H__
#define __VAYO3D_TEXT_OBJECT_H__

#include "Vayo3dMovableObject.h"
#include "Vayo3dRenderable.h"
NS_VAYO3D_BEGIN

class _Vayo3dExport TextObject : public MovableObject, public Renderable
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

NS_VAYO3D_END

#endif // __VAYO3D_TEXT_OBJECT_H__
