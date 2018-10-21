/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* OpenGL显示列表
\*************************************************************************/
#ifndef __VAYO_GL_DISPLAY_LIST_H__
#define __VAYO_GL_DISPLAY_LIST_H__

#include "VayoGLSupport.h"
#include "VayoDisplayList.h"
NS_VAYO_BEGIN

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

NS_VAYO_END

#endif // __VAYO_GL_DISPLAY_LIST_H__
