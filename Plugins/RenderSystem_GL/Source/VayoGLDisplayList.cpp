#include "VayoGLDisplayList.h"
#include "VayoGLRenderSystem.h"

NS_VAYO_BEGIN

void GLDisplayList::newList()
{
	_renderSystem->fillingDisplayList(true);
	glNewList(_displayListName, GL_COMPILE);
}

void GLDisplayList::endList(const wstring& lastMaterialName /*= L""*/)
{
	glEndList();
	_renderSystem->fillingDisplayList(false);
	// ���ò���Ϊ��ʾ�б����һ�ε��õĲ�������.
	_lastMaterialName = lastMaterialName;
}

GLDisplayList::GLDisplayList(const wstring& name, GLRenderSystem* renderSys)
	: DisplayList(name)
	, _renderSystem(renderSys)
{
	static unsigned short idx = 0;
	if (0 == _name.compare(L""))
	{
		std::wstringstream ss;
		ss << L"OpenGL_DisplayList" << idx;
		++idx;
		_name = ss.str();
	}
	_displayListName = glGenLists(1);
}

GLDisplayList::~GLDisplayList()
{
	if (glIsList(_displayListName))
		glDeleteLists(_displayListName, 1);
}

NS_VAYO_END