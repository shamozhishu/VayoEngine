#include "GLDisplayList.h"
#include "GLRenderSystem.h"

GLDisplayList::GLDisplayList(GLRenderSystem* renderSys)
	: _isEmpty(true)
	, _isBeginDraw(false)
	, _isFillingList(false)
	, _renderSystem(renderSys)
{
	_displayListName = glGenLists(1);
}

void GLDisplayList::newList()
{
	if (!_isFillingList)
	{
		_isFillingList = true;
		_renderSystem->fillingDisplayList(true);
		glNewList(_displayListName, GL_COMPILE);
	}
}

void GLDisplayList::endList(const wstring& lastMaterialName /*= L""*/)
{
	if (_isFillingList)
	{
		_isFillingList = false;
		_renderSystem->testGLErrorBegan();
		glEndList();
		_isEmpty = _renderSystem->testGLErrorEnded(L"glEndList()");
		_renderSystem->fillingDisplayList(false);
		// 重置材质为显示列表最后一次调用的材质内容.
		_lastMaterialName = lastMaterialName;
	}
}

bool GLDisplayList::isEmpty()
{
	return _isEmpty;
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

	_markColor.set(0x00000000);
	_markNormal = Vector3df::Origin;
	_markTexCoord.set(-1000000.0f, -1000000.0f);
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
	if (_markNormal != Vector3df(x, y, z))
	{
		glNormal3f(x, y, z);
		_markNormal.set(x, y, z);
	}
}

void GLDisplayList::textureCoord(float u, float v)
{
	if (_markTexCoord != Vector2df(u, v))
	{
		glTexCoord2f(u, v);
		_markTexCoord.set(u, v);
	}
}

void GLDisplayList::colour(int r, int g, int b, int a /*= 255*/)
{
	if (_markColor != Colour(a, r, g, b))
	{
		glColor4ub(r, g, b, a);
		_markColor.set(a, r, g, b);
	}
}
