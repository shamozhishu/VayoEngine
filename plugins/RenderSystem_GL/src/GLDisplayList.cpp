#include "GLDisplayList.h"
#include "GLRenderSystem.h"

bool GLDisplayList::isFillingList()
{
	return _isFillingList;
}

GLDisplayList::GLDisplayList(const wstring& name, GLRenderSystem* renderSys)
	: DisplayList(name)
	, _isBeginDraw(false)
	, _isFillingList(false)
	, _renderSystem(renderSys)
{
	static unsigned long long idx = 0;
	if (0 == _name.compare(L""))
	{
		std::wstringstream ss;
		ss << L"OpenGL_DisplayList" << idx;
		++idx;
		_name = ss.str();
	}
	_displayListName = glGenLists(1);
}

void GLDisplayList::newList()
{
	_isFillingList = true;
	_renderSystem->fillingDisplayList(true);
	glNewList(_displayListName, GL_COMPILE);
}

void GLDisplayList::endList(const wstring& lastMaterialName /*= L""*/)
{
	_isFillingList = false;
	glEndList();
	_renderSystem->fillingDisplayList(false);
	// 重置材质为显示列表最后一次调用的材质内容.
	_lastMaterialName = lastMaterialName;
}

GLDisplayList::~GLDisplayList()
{
	if (glIsList(_displayListName))
		glDeleteLists(_displayListName, 1);
}

bool GLDisplayList::beginDraw(EPrimitiveType primType, const wstring& materialName /*= L""*/)
{
	if (_isBeginDraw)
		return false;

	_renderSystem->setRenderMode3D();

	switch (primType)
	{
	case EPT_POINTS:
		glBegin(GL_POINTS); _isBeginDraw = true; break;
	case EPT_LINE_STRIP:
		glBegin(GL_LINE_STRIP); _isBeginDraw = true; break;
	case EPT_LINE_LOOP:
		glBegin(GL_LINE_LOOP); _isBeginDraw = true; break;
	case EPT_LINES:
		glBegin(GL_LINES); _isBeginDraw = true; break;
	case EPT_TRIANGLE_STRIP:
		glBegin(GL_TRIANGLE_STRIP); _isBeginDraw = true; break;
	case EPT_TRIANGLE_FAN:
		glBegin(GL_TRIANGLE_FAN); _isBeginDraw = true; break;
	case EPT_TRIANGLES:
		glBegin(GL_TRIANGLES); _isBeginDraw = true; break;
	case EPT_QUAD_STRIP:
		glBegin(GL_QUAD_STRIP); _isBeginDraw = true; break;
	case EPT_QUADS:
		glBegin(GL_QUADS); _isBeginDraw = true; break;
	case EPT_POLYGON:
		glBegin(GL_POLYGON); _isBeginDraw = true; break;
	}

	return _isBeginDraw;
}

void GLDisplayList::endDraw()
{
	if (_isBeginDraw)
		glEnd();
	_isBeginDraw = false;
}

void GLDisplayList::position(float x, float y, float z)
{
	if (_isBeginDraw)
		glVertex3f(x, y, z);
}

void GLDisplayList::normal(float x, float y, float z)
{
	if (_isBeginDraw)
		glNormal3f(x, y, z);
}

void GLDisplayList::textureCoord(float u, float v)
{
	if (_isBeginDraw)
		glTexCoord2f(u, v);
}

void GLDisplayList::colour(int r, int g, int b, int a /*= 255*/)
{
	if (_isBeginDraw)
		glColor4ub(r, g, b, a);
}
