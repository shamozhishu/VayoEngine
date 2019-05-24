/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* OpenGL显示列表
\*************************************************************************/
#ifndef __GL_DISPLAY_LIST_H__
#define __GL_DISPLAY_LIST_H__

#include "GLSupport.h"
#include "VayoVector2d.h"
#include "VayoVector3d.h"
#include "Vayo3dColour.h"
#include "Vayo3dDisplayList.h"

class GLDisplayList : public DisplayList
{
public:
	GLDisplayList(const wstring& name, GLRenderSystem* renderSys);
	~GLDisplayList();
	void newList();
	void endList(const wstring& lastMaterialName = L"");
	bool isEmpty();
	bool beginDraw(EPrimitiveType primType, const wstring& materialName = L"");
	void endDraw();
	void position(float x, float y, float z);
	void normal(float x, float y, float z);
	void textureCoord(float u, float v);
	void colour(int r, int g, int b, int a = 255);

private:
	bool _isEmpty;
	bool _isBeginDraw;
	bool _isFillingList;
	Colour _markColor;
	Vector3df _markNormal;
	Vector2df _markTexCoord;
	GLRenderSystem* _renderSystem;
	PROPERTY_R(GLuint,      _displayListName,  DisplayListName)
	PROPERTY_R_REF(wstring, _lastMaterialName, LastMaterialName)
};

#endif // __GL_DISPLAY_LIST_H__
