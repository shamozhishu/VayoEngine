/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* OpenGL显示列表
\*************************************************************************/
#ifndef __GL_DISPLAY_LIST_H__
#define __GL_DISPLAY_LIST_H__

#include "GLSupport.h"
#include "Vayo3dDisplayList.h"

class GLDisplayList : public DisplayList
{
public:
	GLDisplayList(const wstring& name, GLRenderSystem* renderSys);
	~GLDisplayList();
	void newList();
	void endList(const wstring& lastMaterialName = L"");

private:
	GLRenderSystem* _renderSystem;
	PROPERTY_R(GLuint,      _displayListName,  DisplayListName)
	PROPERTY_R_REF(wstring, _lastMaterialName, LastMaterialName)
};

#endif // __GL_DISPLAY_LIST_H__
