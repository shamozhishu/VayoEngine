#include "GLTesselator.h"
#include "VayoLog.h"
#include "Vayo3dVertex.h"

const char* GLTesselator::getPrimitiveType(GLenum type)
{
	switch (type)
	{
	case 0x0000: return "GL_POINTS";
	case 0x0001: return "GL_LINES";
	case 0x0002: return "GL_LINE_LOOP";
	case 0x0003: return "GL_LINE_STRIP";
	case 0x0004: return "GL_TRIANGLES";
	case 0x0005: return "GL_TRIANGLE_STRIP";
	case 0x0006: return "GL_TRIANGLE_FAN";
	case 0x0007: return "GL_QUADS";
	case 0x0008: return "GL_QUAD_STRIP";
	case 0x0009: return "GL_POLYGON";
	}
	return "";
}

GLTesselator::GLTesselator(const wstring& name)
	: Tesselator(name)
	, _tess(NULL)
{
	static unsigned long long idx = 0;
	if (0 == _name.compare(L""))
	{
		std::wstringstream ss;
		ss << L"OpenGL_Tesselator" << idx;
		++idx;
		_name = ss.str();
	}

	_tess = gluNewTess();
	gluTessCallback(_tess, GLU_TESS_BEGIN, (void(__stdcall*)(void))tessBeginCB);
	gluTessCallback(_tess, GLU_TESS_VERTEX, (void(__stdcall*)(void))tessVertexCB);
	gluTessCallback(_tess, GLU_TESS_END, (void(__stdcall*)(void))tessEndCB);
	gluTessCallback(_tess, GLU_TESS_ERROR, (void(__stdcall*)(void))tessErrorCB);
}

GLTesselator::~GLTesselator()
{
	if (_tess)
		gluDeleteTess(_tess);
}

void GLTesselator::beginPolygon(void* data)
{
	gluTessBeginPolygon(_tess, data);
}

void GLTesselator::beginContour()
{
	gluTessBeginContour(_tess);
}

void GLTesselator::vertex(VertIdxPair* vert)
{
	GLdouble coords[3];
	coords[0] = (GLdouble)vert[0]._vert._position._x;
	coords[1] = (GLdouble)vert[0]._vert._position._y;
	coords[2] = (GLdouble)vert[0]._vert._position._z;
	gluTessVertex(_tess, coords, vert);
}

void GLTesselator::endContour()
{
	gluTessEndContour(_tess);
}

void GLTesselator::endPolygon()
{
	gluTessEndPolygon(_tess);
}

void GLTesselator::setBeginCallback(Tesselator::TessBeginDataProc pBegin)
{
	gluTessCallback(_tess, GLU_TESS_BEGIN_DATA, (void(__stdcall*)(void))pBegin);
}

void GLTesselator::setVertexCallback(Tesselator::TessVertexDataProc pVertex)
{
	gluTessCallback(_tess, GLU_TESS_VERTEX_DATA, (void(__stdcall*)(void))pVertex);
}

void GLTesselator::setEndCallback(Tesselator::TessEndDataProc pEnd)
{
	gluTessCallback(_tess, GLU_TESS_END_DATA, (void(__stdcall*)(void))pEnd);
}

void GLTesselator::setErrorCallback(Tesselator::TessErrorDataProc pError)
{
	gluTessCallback(_tess, GLU_TESS_ERROR_DATA, (void(__stdcall*)(void))pError);
}

void GLTesselator::setCombineCallback(Tesselator::TessCombineDataProc pCombine)
{
	gluTessCallback(_tess, GLU_TESS_COMBINE_DATA, (void(__stdcall*)(void))pCombine);
}

void GLTesselator::setWindingProperty(ETessWinding value)
{
	GLdouble val = GLU_TESS_WINDING_ODD;
	switch (value)
	{
	case Tesselator::ETW_WINDING_ODD: val = GLU_TESS_WINDING_ODD; break;
	case Tesselator::ETW_WINDING_NONZERO: val = GLU_TESS_WINDING_NONZERO; break;
	case Tesselator::ETW_WINDING_POSITIVE: val = GLU_TESS_WINDING_POSITIVE; break;
	case Tesselator::ETW_WINDING_NEGATIVE: val = GLU_TESS_WINDING_NEGATIVE; break;
	case Tesselator::ETW_WINDING_ABS_GEQ_TWO: val = GLU_TESS_WINDING_ABS_GEQ_TWO; break;
	}
	gluTessProperty(_tess, GLU_TESS_WINDING_RULE, val);
}

void CALLBACK GLTesselator::tessBeginCB(GLenum which)
{
	glBegin(which);
#ifdef _DEBUG
	Log::print(ELL_DEBUG, "glBegin(%s);", getPrimitiveType(which));
#endif
}

void CALLBACK GLTesselator::tessVertexCB(const VertIdxPair* vert)
{
	glColor4ub(vert[0]._vert._color.getRed(), vert[0]._vert._color.getGreen(), vert[0]._vert._color.getBlue(), vert[0]._vert._color.getAlpha());
	glTexCoord2f(vert[0]._vert._texCoord._x, vert[0]._vert._texCoord._y);
	glNormal3f(vert[0]._vert._normal._x, vert[0]._vert._normal._y, vert[0]._vert._normal._z);
	glVertex3f(vert[0]._vert._position._x, vert[0]._vert._position._y, vert[0]._vert._position._z);
#ifdef _DEBUG
	Log::print(ELL_DEBUG, "    glColor4ub(%d,%d,%d,%d);", vert[0]._vert._color.getRed(), vert[0]._vert._color.getGreen(), vert[0]._vert._color.getBlue(), vert[0]._vert._color.getAlpha());
	Log::print(ELL_DEBUG, "    glTexCoord2f(%f,%f);", vert[0]._vert._texCoord._x, vert[0]._vert._texCoord._y);
	Log::print(ELL_DEBUG, "    glNormal3f(%f,%f,%f);", vert[0]._vert._normal._x, vert[0]._vert._normal._y, vert[0]._vert._normal._z);
	Log::print(ELL_DEBUG, "    glVertex3f(%f,%f,%f);", vert[0]._vert._position._x, vert[0]._vert._position._y, vert[0]._vert._position._z);
#endif
}

void CALLBACK GLTesselator::tessEndCB()
{
	glEnd();
#ifdef _DEBUG
	Log::print(ELL_DEBUG, "glEnd();");
#endif
}

void CALLBACK GLTesselator::tessErrorCB(GLenum errorCode)
{
	const GLubyte* errorStr = gluErrorString(errorCode);
#ifdef _DEBUG
	Log::print(ELL_ERROR, "[GLTesselator::tessErrorCB]:%s", (const char*)errorStr);
#endif
}
