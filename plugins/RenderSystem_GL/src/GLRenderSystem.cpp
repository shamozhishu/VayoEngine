#include "GLRenderSystem.h"
#include <wglext.h>
#include "VayoLog.h"
#include "VayoUtils.h"
#include "Vayo3dMesh.h"
#include "Vayo3dRoot.h"
#include "VayoDevice.h"
#include "GLTexture.h"
#include "GLTesselator.h"
#include "GLDisplayList.h"
#include "Vayo3dMaterialManager.h"
#include "GLMaterialRenderer.h"

static PIXELFORMATDESCRIPTOR g_pfd = {
	sizeof(PIXELFORMATDESCRIPTOR), // Size Of This Pixel Format Descriptor
	1,                             // Version Number
	PFD_DRAW_TO_WINDOW |           // Format Must Support Window
	PFD_SUPPORT_OPENGL |           // Format Must Support OpenGL
	PFD_DOUBLEBUFFER,              // Must Support Double Buffering
	PFD_TYPE_RGBA,                 // Request An RGBA Format
	32,                            // Select Our Color Depth
	0, 0, 0, 0, 0, 0,              // Color Bits Ignored
	0,                             // No Alpha Buffer
	0,                             // Shift Bit Ignored
	0,                             // No Accumulation Buffer
	0, 0, 0, 0,                    // Accumulation Bits Ignored
	24,                            // Z-Buffer (Depth Buffer)
	8,                             // Stencil Buffer Depth
	0,                             // No Auxiliary Buffer
	PFD_MAIN_PLANE,                // Main Drawing Layer
	0,                             // Reserved
	0, 0, 0 };                     // Layer Masks Ignored

static inline unsigned char* buffer_offset(const long offset)
{
	return ((unsigned char*)0 + offset);
}

GLRenderSystem::GLRenderSystem(const wstring& name)
	: RenderSystem(name)
	, _hCurrentDC(NULL)
	, _hCurrentRC(NULL)
	, _hCurrentWnd(NULL)
	, _pixelFormat(0)
	, _currentRenderMode(ERM_NONE)
	, _colorBufferFormat(ECF_BGRA8888)
	, _resetRenderStates(true)
	, _transformation3DChanged(true)
	, _antiAliasFactor(0)
	, _lastSetLight(-1)
	, _maxTextureSize(1)
	, _isFillDisplayList(false)
	, _maxTextureUnits(MATERIAL_MAX_TEXTURES)
	, _maxTextureLODBias(0.0f)
	, _curMaterial(L"GLRenderSystem_current_material")
	, _lastMaterial(L"GLRenderSystem_last_material")
{
	_dimAliasedLine[0] = 1.0f;
	_dimAliasedLine[1] = 1.0f;
	_dimAliasedPoint[0] = 1.0f;
	_dimAliasedPoint[1] = 1.0f;
	_dimSmoothedLine[0] = 1.0f;
	_dimSmoothedLine[1] = 1.0f;
	_dimSmoothedPoint[0] = 1.0f;
	_dimSmoothedPoint[1] = 1.0f;
	memset(_texturesUnitSet, 0, sizeof(_texturesUnitSet));
}

GLRenderSystem::~GLRenderSystem()
{
	_materialRenderers.clear();
	removeAllHardwareBuffers();
	destroyAllDisplayList();
	destroyAllTesselators();

	if (_hCurrentRC)
	{
		if (!wglMakeCurrent(_hCurrentDC, NULL))
			Log::print(ELL_WARNING, "Release of dc and rc failed.");
		if (!wglDeleteContext(_hCurrentRC))
			Log::print(ELL_WARNING, "Release of rendering context failed.");
	}

	if (_hCurrentDC)
		ReleaseDC(_hCurrentWnd, _hCurrentDC);
}

bool GLRenderSystem::init(unsigned char antiAliasFactor /*= 0*/, bool handleSRGB /*= false*/)
{
	g_pfd = { sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 8, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 };

	_pixelFormat = 0;
	const Device::Attrib& devAttrib = Root::getSingleton().getActiveDevice()->getAttrib();
	_antiAliasFactor = antiAliasFactor;
	if (antiAliasFactor > 1)
	{
		// Create a window to test antialiasing support.
		const wchar_t* szClassName = L"GLRenderSystem";
		HINSTANCE lhInstance = GetModuleHandle(0);

		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = (WNDPROC)DefWindowProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = lhInstance;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = 0;
		wcex.lpszClassName = szClassName;
		wcex.hIconSm = 0;
		wcex.hIcon = 0;

		RegisterClassEx(&wcex);

		RECT clientSize;
		clientSize.top = 0;
		clientSize.left = 0;
		clientSize.right = devAttrib.ScreenSize._width;
		clientSize.bottom = devAttrib.ScreenSize._height;

		DWORD style = WS_POPUP;
		if (!devAttrib.FullScreen)
			style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

		AdjustWindowRect(&clientSize, style, FALSE);

		const int realWidth = clientSize.right - clientSize.left;
		const int realHeight = clientSize.bottom - clientSize.top;

		const int windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
		const int windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

		HWND temporary_wnd = CreateWindow(szClassName, __TEXT(""), style, windowLeft,
			windowTop, realWidth, realHeight, NULL, NULL, lhInstance, NULL);

		if (!temporary_wnd)
		{
			Log::print(ELL_ERROR, "Cannot create a temporary window.");
			UnregisterClass(szClassName, lhInstance);
			return false;
		}

		_hCurrentDC = GetDC(temporary_wnd);
		_pixelFormat = ChoosePixelFormat(_hCurrentDC, &g_pfd);
		if (!_pixelFormat)
		{
			Log::print(ELL_ERROR, "Cannot create a GL device context, No suitable pixel format for temporary window.");
			ReleaseDC(temporary_wnd, _hCurrentDC);
			DestroyWindow(temporary_wnd);
			UnregisterClass(szClassName, lhInstance);
			return false;
		}

		SetPixelFormat(_hCurrentDC, _pixelFormat, &g_pfd);
		_hCurrentRC = wglCreateContext(_hCurrentDC);
		if (!_hCurrentRC)
		{
			Log::print(ELL_ERROR, "Cannot create a temporary GL rendering context.");
			ReleaseDC(temporary_wnd, _hCurrentDC);
			DestroyWindow(temporary_wnd);
			UnregisterClass(szClassName, lhInstance);
			return false;
		}

		if (!wglMakeCurrent(_hCurrentDC, _hCurrentRC))
		{
			Log::print(ELL_ERROR, "Cannot activate a temporary GL rendering context.");
			wglDeleteContext(_hCurrentRC);
			ReleaseDC(temporary_wnd, _hCurrentDC);
			DestroyWindow(temporary_wnd);
			UnregisterClass(szClassName, lhInstance);
			return false;
		}

		string wglExtensions;

#ifdef WGL_ARB_extensions_string
		PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
		if (wglGetExtensionsStringARB)
			wglExtensions = wglGetExtensionsStringARB(_hCurrentDC);
#elif defined(WGL_EXT_extensions_string)
		PFNWGLGETEXTENSIONSSTRINGEXTPROC wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");
		if (wglGetExtensionsStringEXT)
			wglExtensions = wglGetExtensionsStringEXT(_hCurrentDC);
#endif

		const bool pixel_format_supported = (wglExtensions.find("WGL_ARB_pixel_format") != std::string::npos);
		const bool multi_sample_supported = (
			(wglExtensions.find("WGL_ARB_multisample") != std::string::npos) ||
			(wglExtensions.find("WGL_EXT_multisample") != std::string::npos) ||
			(wglExtensions.find("WGL_3DFX_multisample") != std::string::npos));

#ifdef _DEBUG
		Log::print(ELL_DEBUG, "WGL_extensions:\n%s", wglExtensions.c_str());
#endif

#ifdef WGL_ARB_pixel_format
		PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
		if (pixel_format_supported && wglChoosePixelFormatARB)
		{
			// This value determines the number of samples used for antialiasing,
			// My experience is that 8 does not show a big improvement over 4,
			// But 4 shows a big improvement over 2.

			if (_antiAliasFactor > 32)
			{
				_antiAliasFactor = 32;
			}

			float fAttributes[] = { 0.0f, 0.0f };
			int iAttributes[] =
			{
				WGL_DRAW_TO_WINDOW_ARB, 1,
				WGL_SUPPORT_OPENGL_ARB, 1,
				WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
				WGL_COLOR_BITS_ARB, 24,
				WGL_ALPHA_BITS_ARB, 8,
				WGL_DEPTH_BITS_ARB, 24, // 10,11
				WGL_STENCIL_BITS_ARB, 8,
				WGL_DOUBLE_BUFFER_ARB, 1,
				WGL_STEREO_ARB, 0,
				WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
#ifdef WGL_ARB_multisample
				WGL_SAMPLES_ARB, _antiAliasFactor, // 20,21
				WGL_SAMPLE_BUFFERS_ARB, 1,
#elif defined(WGL_EXT_multisample)
				WGL_SAMPLES_EXT, _antiAliasFactor, // 20,21
				WGL_SAMPLE_BUFFERS_EXT, 1,
#elif defined(WGL_3DFX_multisample)
				WGL_SAMPLES_3DFX,_antiAliasFactor, // 20,21
				WGL_SAMPLE_BUFFERS_3DFX, 1,
#endif
#ifdef WGL_ARB_framebuffer_sRGB
				WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, handleSRGB ? 1 : 0,
#elif defined(WGL_EXT_framebuffer_sRGB)
				WGL_FRAMEBUFFER_SRGB_CAPABLE_EXT, handleSRGB ? 1 : 0,
#endif
				0,0,0,0
			};

			int iAttrSize = sizeof(iAttributes) / sizeof(int);
			const bool framebuffer_srgb_supported = (
				(wglExtensions.find("WGL_ARB_framebuffer_sRGB") != std::string::npos) ||
				(wglExtensions.find("WGL_EXT_framebuffer_sRGB") != std::string::npos));

			if (!framebuffer_srgb_supported)
			{
				memmove(&iAttributes[24], &iAttributes[26], sizeof(int)*(iAttrSize - 26));
				iAttrSize -= 2;
			}

			if (!multi_sample_supported)
			{
				memmove(&iAttributes[20], &iAttributes[24], sizeof(int)*(iAttrSize - 24));
				iAttrSize -= 4;
			}

			int rv = 0;

			do
			{
				int pixelFmt = 0;
				UINT numFmt = 0;
				const BOOL valid = wglChoosePixelFormatARB(_hCurrentDC, iAttributes, fAttributes, 1, &pixelFmt, &numFmt);
				if (valid && numFmt)
					rv = pixelFmt;
				else
					iAttributes[21] -= 1;

			} while (rv == 0 && iAttributes[21] > 1);

			if (rv)
			{
				_pixelFormat = rv;
				_antiAliasFactor = iAttributes[21];
			}
		}
		else
#endif
		{
			_antiAliasFactor = 0;
		}

		wglMakeCurrent(_hCurrentDC, NULL);
		wglDeleteContext(_hCurrentRC);
		ReleaseDC(temporary_wnd, _hCurrentDC);
		DestroyWindow(temporary_wnd);
		UnregisterClass(szClassName, lhInstance);
	}

	if (!setWndPixelFormat(Root::getSingleton().getActiveDevice()))
		return false;

#ifdef WGL_ARB_create_context
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	if (wglCreateContextAttribsARB)
	{
		int iAttribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 1,
			0
		};

		_hCurrentRC = wglCreateContextAttribsARB(_hCurrentDC, 0, iAttribs);
	}
	else
#endif
	{
		_hCurrentRC = wglCreateContext(_hCurrentDC);
	}

	if (!_hCurrentRC)
	{
		Log::print(ELL_ERROR, "Cannot create a GL rendering context.");
		return false;
	}

	if (!wglMakeCurrent(_hCurrentDC, _hCurrentRC))
	{
		Log::print(ELL_ERROR, "Cannot activate GL rendering context.");
		wglDeleteContext(_hCurrentRC);
		return false;
	}

	int pf = GetPixelFormat(_hCurrentDC);
	DescribePixelFormat(_hCurrentDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &g_pfd);
	if (g_pfd.cAlphaBits != 0)
	{
		if (g_pfd.cRedBits == 8)
			_colorBufferFormat = ECF_BGRA8888;
		else
			_colorBufferFormat = ECF_BGRA5551;
	}
	else
	{
		if (g_pfd.cRedBits == 8)
			_colorBufferFormat = ECF_BGR888;
		else
			_colorBufferFormat = ECF_BGR565;
	}

	//////////////////////////////////////////////////////////////////////////
	if (!gladLoadGL())
	{
		Log::print(ELL_ERROR, "Failed to initialize GLAD");
		return false;
	}

	Log::print(ELL_DEBUG, "OpenGL version: %d.%d", GLVersion.major, GLVersion.minor);
	createMaterialRenderers();
	const Dimension2di& renderTargetSize = getCurRenderTargetSize();
	setViewpot(Recti(0, 0, renderTargetSize._width, renderTargetSize._height));
	setAmbientLight(Colour(0, 0, 0, 0));
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &_maxTextureSize);
	GLint num = 0;
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &num);
	_maxTextureUnits = min_(MATERIAL_MAX_TEXTURES, (int)num);
	glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS, &_maxTextureLODBias);
	glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, _dimAliasedLine);
	glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, _dimAliasedPoint);
	glGetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE, _dimSmoothedLine);
	glGetFloatv(GL_SMOOTH_POINT_SIZE_RANGE, _dimSmoothedPoint);
	glClearDepth(1.0f);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_FASTEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_NORMALIZE);
	glFrontFace(GL_CCW);
	if (handleSRGB)
		glEnable(GL_FRAMEBUFFER_SRGB);

	setRenderMode3D();

	return true;
}

bool GLRenderSystem::isActive() const
{
	return _hCurrentDC != NULL && _hCurrentRC == wglGetCurrentContext();
}

void GLRenderSystem::restoreContext() const
{
	if (wglGetCurrentContext() != _hCurrentRC)
		wglMakeCurrent(_hCurrentDC, _hCurrentRC);
}

bool GLRenderSystem::beginScene(bool backBuffer, bool zBuffer, bool stencilBuffer, Device* renderWnd)
{
	if (!RenderSystem::beginScene(backBuffer, zBuffer, stencilBuffer, renderWnd) || !changeRenderContext(renderWnd))
		return false;

	GLbitfield mask = 0;
	if (backBuffer)
	{
		float inv = 1.0f / 255.0f;
		Colour color = renderWnd->getAttrib().BgClearColor;
		glClearColor(color.getRed() * inv, color.getGreen() * inv, color.getBlue() * inv, color.getAlpha() * inv);
		mask |= GL_COLOR_BUFFER_BIT;
	}

	if (zBuffer)
	{
		glDepthMask(GL_TRUE);
		mask |= GL_DEPTH_BUFFER_BIT;
	}

	if (stencilBuffer)
	{
		glStencilMask(0xFF);
		mask |= GL_STENCIL_BUFFER_BIT;
	}

	glClear(mask);
	return true;
}

bool GLRenderSystem::endScene()
{
	RenderSystem::endScene();
	return SwapBuffers(_hCurrentDC) == TRUE;
}

const Matrix4x4& GLRenderSystem::getTransform(ETransformState state) const
{
	return _matrizes[state];
}

void GLRenderSystem::setTransform(ETransformState state, const Matrix4x4& mat)
{
	_transformation3DChanged = true;
	_matrizes[state] = mat;

	switch (state)
	{
	case ETS_VIEW:
	case ETS_WORLD:
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf((_matrizes[ETS_VIEW] * _matrizes[ETS_WORLD]).pointer());
		break;
	case ETS_PROJECTION:
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(_matrizes[ETS_PROJECTION].pointer());
		break;
	case ETS_TEXTURE_0:
	case ETS_TEXTURE_1:
	case ETS_TEXTURE_2:
	case ETS_TEXTURE_3:
#if VAYO_MATERIAL_MAX_TEXTURES > 4
	case ETS_TEXTURE_4:
#if VAYO_MATERIAL_MAX_TEXTURES > 5
	case ETS_TEXTURE_5:
#if VAYO_MATERIAL_MAX_TEXTURES > 6
	case ETS_TEXTURE_6:
#if VAYO_MATERIAL_MAX_TEXTURES > 7
	case ETS_TEXTURE_7:
#endif
#endif
#endif
#endif
		{
			const unsigned int i = state - ETS_TEXTURE_0;
			if (i >= MATERIAL_MAX_TEXTURES)
				break;

			glActiveTexture(GL_TEXTURE0 + i);
			glMatrixMode(GL_TEXTURE);

			if (mat == IdentityMat4)
				glLoadIdentity();
			else
			{
				GLfloat glmat[16];
				getGLTextureMatrix(glmat, mat);
				glLoadMatrixf(glmat);
			}

			break;
		}
	}
}

void GLRenderSystem::setViewpot(const Recti& area)
{
	const Dimension2di& renderTargetSize = getCurRenderTargetSize();
	Recti vp = area;
	Recti rendert(0, 0, renderTargetSize._width, renderTargetSize._height);
	vp.clipAgainst(rendert);

	if (vp.getHeight() > 0 && vp.getWidth() > 0)
		glViewport(vp._upperLeftCorner._x,
			renderTargetSize._height - vp._upperLeftCorner._y - vp.getHeight(),
			vp.getWidth(),
			vp.getHeight());

	_viewPort = vp;
}

void GLRenderSystem::setAmbientLight(const Colour& color)
{
	GLfloat data[4] = { (GLfloat)color.getRed() / 255, (GLfloat)color.getGreen() / 255,
		(GLfloat)color.getBlue() / 255, (GLfloat)color.getAlpha() / 255 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, data);
}

void GLRenderSystem::setMaterial(const Material& material)
{
	_curMaterial = material;
	for (int i = _maxTextureUnits - 1; i >= 0; --i)
	{
		setActiveTexture(i, material._textureLayer[i]._texture.get());
		setTransform((ETransformState)(ETS_TEXTURE_0 + i), _curMaterial._textureLayer[i]._textureMatrix);
	}
}

bool GLRenderSystem::setWndPixelFormat(Device* renderWnd)
{
	_hCurrentDC = GetDC((HWND)renderWnd->getWndHandle());
	if (!_hCurrentDC)
	{
		Log::print(ELL_ERROR, "Cannot create a GL device context.");
		return false;
	}

	if (_pixelFormat == 0 || !SetPixelFormat(_hCurrentDC, _pixelFormat, &g_pfd))
	{
		_pixelFormat = ChoosePixelFormat(_hCurrentDC, &g_pfd);
		if (!_pixelFormat)
		{
			Log::print(ELL_ERROR, "Cannot create a GL device context, No suitable format.");
			return false;
		}

		if (!SetPixelFormat(_hCurrentDC, _pixelFormat, &g_pfd))
		{
			wchar_t szErrHint[256];
			wsprintf(szErrHint, L"Can't set the pixel format %d", _pixelFormat);
			printLastError(szErrHint);
			return false;
		}
	}

	Log::print(ELL_DEBUG, "Pixel Format %d", _pixelFormat);
	return true;
}

void GLRenderSystem::addDynamicLight(const LightData& light)
{
	++_lastSetLight;
	if (!(_lastSetLight < GL_MAX_LIGHTS))
		return;

	setTransform(ETS_WORLD, Matrix4x4());

	RenderSystem::addDynamicLight(light);

	int lidx = GL_LIGHT0 + _lastSetLight;
	GLfloat data[4];
	GLfloat spotDir[4];

	data[0] = light._diffuseColor.getRed() / 255.0f;
	data[1] = light._diffuseColor.getGreen() / 255.0f;
	data[2] = light._diffuseColor.getBlue() / 255.0f;
	data[3] = light._diffuseColor.getAlpha() / 255.0f;
	glLightfv(lidx, GL_DIFFUSE, data);

	data[0] = light._specularColor.getRed() / 255.0f;
	data[1] = light._specularColor.getGreen() / 255.0f;
	data[2] = light._specularColor.getBlue() / 255.0f;
	data[3] = light._specularColor.getAlpha() / 255.0f;
	glLightfv(lidx, GL_SPECULAR, data);

	data[0] = light._ambientColor.getRed() / 255.0f;
	data[1] = light._ambientColor.getGreen() / 255.0f;
	data[2] = light._ambientColor.getBlue() / 255.0f;
	data[3] = light._ambientColor.getAlpha() / 255.0f;
	glLightfv(lidx, GL_AMBIENT, data);

	data[0] = light._position._x;
	data[1] = light._position._y;
	data[2] = light._position._z;
	data[3] = 1.0f;
	float a0 = 0.0f, a1 = 1.0f, a2 = 0.0f;
	switch (light._lightType)
	{
	case ELT_DIRECTIONAL_LIGHT:
		data[0] = light._direction._x;
		data[1] = light._direction._y;
		data[2] = light._direction._z;
		data[3] = 0.0f;
		a0 = 1.0f; a1 = 0.0f; a2 = 0.0f;
		break;
	case ELT_POINT_LIGHT:
		break;
	case ELT_SPOT_LIGHT:
		spotDir[0] = light._direction._x;
		spotDir[1] = light._direction._y;
		spotDir[2] = light._direction._z;
		spotDir[3] = 0.0f;
		glLightfv(lidx, GL_SPOT_DIRECTION, spotDir);
		glLightf(lidx, GL_SPOT_EXPONENT, light._exponent);
		glLightf(lidx, GL_SPOT_CUTOFF, light._cutoff);
		break;
	}

	glLightfv(lidx, GL_POSITION, data);

	glLightf(lidx, GL_CONSTANT_ATTENUATION, a0);
	glLightf(lidx, GL_LINEAR_ATTENUATION, a1);
	glLightf(lidx, GL_QUADRATIC_ATTENUATION, a2);

	glEnable(lidx);
}

void GLRenderSystem::removeAllDynamicLights()
{
	for (int i = 0; i < _lastSetLight + 1; ++i)
		glDisable(GL_LIGHT0 + i);

	_lastSetLight = -1;
	RenderSystem::removeAllDynamicLights();
}

void GLRenderSystem::drawDisplayList(DisplayList* displayList)
{
	GLDisplayList* pGLDisplayList = dynamic_cast<GLDisplayList*>(displayList);
	if (NULL == pGLDisplayList)
		return;

	GLuint uDisplayListName = pGLDisplayList->getDisplayListName();
	if (glIsList(uDisplayListName))
	{
		// 设置显示列表的公共材质(这些材质属性不能装入显示列表).
		setRenderMode3D();
		// 各个子网格的特定材质在执行显示列表时由显示列表自行设置.
		glCallList(uDisplayListName);
		wstring lastMaterialName = pGLDisplayList->getLastMaterialName();
		if (lastMaterialName != L"")
			_lastMaterial = *Root::getSingleton().getMaterialManager()->findMaterial(lastMaterialName);
	}
}

bool GLRenderSystem::setActiveTexture(int stage, Texture* texture)
{
	if (stage >= _maxTextureUnits)
		return false;

	if (_texturesUnitSet[stage] == texture)
		return true;

	glActiveTexture(GL_TEXTURE0 + stage);
	_texturesUnitSet[stage] = texture;

	if (texture && texture->getRenderSystemType() != ERST_OPENGL)
	{
		glDisable(GL_TEXTURE_2D);
		_texturesUnitSet[stage] = NULL;
		Log::wprint(ELL_ERROR, L"Fatal Error: Tried to set a texture not owned by GLRenderSystem.");
		return false;
	}

	if (texture == NULL)
		glDisable(GL_TEXTURE_2D);
	else
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, ((GLTexture*)(texture))->getOpenGLTextureName());
	}

	return true;
}

bool GLRenderSystem::disableTextures(int fromStage /*= 0*/)
{
	bool result = true;
	for (int i = fromStage; i < _maxTextureUnits; ++i)
		result &= setActiveTexture(i, NULL);
	return result;
}

void GLRenderSystem::setRenderMode3D()
{
	if (_currentRenderMode != ERM_3D)
	{
		glDisable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if (!_isFillDisplayList)
		{
			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixf((_matrizes[ETS_VIEW] * _matrizes[ETS_WORLD]).pointer());
			glMatrixMode(GL_PROJECTION);
			glLoadMatrixf(_matrizes[ETS_PROJECTION].pointer());
		}

		_resetRenderStates = true;
	}

	if (_resetRenderStates || _lastMaterial != _curMaterial)
	{
		if (_lastMaterial._materialType != _curMaterial._materialType &&
			_lastMaterial._materialType >= 0 && _lastMaterial._materialType < (int)_materialRenderers.size())
			_materialRenderers[_lastMaterial._materialType]->onUnsetMaterial();

		if (_curMaterial._materialType >= 0 && _curMaterial._materialType < (int)_materialRenderers.size())
			_materialRenderers[_curMaterial._materialType]->onSetMaterial(
				_curMaterial, _lastMaterial, _resetRenderStates);
	}

	_lastMaterial = _curMaterial;
	_resetRenderStates = false;
	if (!_isFillDisplayList)
		_currentRenderMode = ERM_3D;
}

void GLRenderSystem::setRenderMode2D(bool alpha, bool texture, bool alphaChannel)
{
	if (_currentRenderMode == ERM_3D)
	{
		if (static_cast<unsigned int>(_lastMaterial._materialType) < _materialRenderers.size())
			_materialRenderers[_lastMaterial._materialType]->onUnsetMaterial();
	}

	if (_transformation3DChanged)
	{
		glMatrixMode(GL_PROJECTION);
		const Dimension2di& renderTargetSize = getCurRenderTargetSize();
		Matrix4x4 m;
		m.buildProjectionMatrixOrthoRH(float(renderTargetSize._width), float(-(renderTargetSize._height)), -1.0f, 1.0f);
		m.setTranslation(Vector3df(-1, 1, 0));
		glLoadMatrixf(m.pointer());

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);
		glDisable(GL_FOG);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_LIGHTING);

		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);

		_transformation3DChanged = false;
	}

	if (texture)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		if (alphaChannel)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glDisable(GL_ALPHA_TEST);
		}
		else
		{
			if (alpha)
			{
				glDisable(GL_ALPHA_TEST);
				glEnable(GL_BLEND);
			}
			else
			{
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				glDisable(GL_ALPHA_TEST);
				glDisable(GL_BLEND);
			}
		}
	}
	else
	{
		if (alpha)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glDisable(GL_ALPHA_TEST);
		}
		else
		{
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glDisable(GL_BLEND);
			glDisable(GL_ALPHA_TEST);
		}
	}

	_currentRenderMode = ERM_2D;
}

void GLRenderSystem::setWrapMode(const Material& material)
{
	for (int i = 0; i < _maxTextureUnits; ++i)
	{
		if (!_texturesUnitSet[i])
			continue;

		glActiveTexture(GL_TEXTURE0 + i);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getTextureWrapMode(material._textureLayer[i]._textureWrapU));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getTextureWrapMode(material._textureLayer[i]._textureWrapV));
	}
}

bool GLRenderSystem::changeRenderContext(Device* renderWnd)
{
	if (_hCurrentWnd != renderWnd->getWndHandle())
	{
		if (_hCurrentWnd && _hCurrentDC)
		{
			ReleaseDC(_hCurrentWnd, _hCurrentDC);
		}

		_hCurrentWnd = (HWND)renderWnd->getWndHandle();
		if (_hCurrentWnd)
		{
			_hCurrentDC = GetDC(_hCurrentWnd);
			if (!wglMakeCurrent(_hCurrentDC, _hCurrentRC))
			{
				Log::wprint(ELL_ERROR, L"wglMakeCurrent() call failed![error code: %d]", GetLastError());
				return false;
			}
			Dimension2di screenSize = renderWnd->getScreenSize();
			setViewpot(Recti(0, 0, screenSize._width, screenSize._height));
		}
	}
	return true;
}

GLint GLRenderSystem::getTextureWrapMode(unsigned char clamp)
{
	GLint mode = GL_REPEAT;
	switch (clamp)
	{
	case ETC_REPEAT: mode = GL_REPEAT; break;
	case ETC_CLAMP: mode = GL_CLAMP; break;
	case ETC_CLAMP_TO_EDGE: mode = GL_CLAMP_TO_EDGE; break;
	case ETC_CLAMP_TO_BORDER: mode = GL_CLAMP_TO_BORDER; break;
	case ETC_MIRROR: mode = GL_MIRRORED_REPEAT; break;
	case ETC_MIRROR_CLAMP: mode = GL_CLAMP; break;
	case ETC_MIRROR_CLAMP_TO_EDGE: mode = GL_MIRROR_CLAMP_TO_EDGE; break;
	case ETC_MIRROR_CLAMP_TO_BORDER: mode = GL_CLAMP; break;
	}
	return mode;
}

inline void GLRenderSystem::getGLTextureMatrix(GLfloat gl_matrix[16], const Matrix4x4& mat)
{
	gl_matrix[0] = mat[0];
	gl_matrix[1] = mat[1];
	gl_matrix[2] = 0.0f;
	gl_matrix[3] = 0.0f;

	gl_matrix[4] = mat[4];
	gl_matrix[5] = mat[5];
	gl_matrix[6] = 0.0f;
	gl_matrix[7] = 0.0f;

	gl_matrix[8] = 0.0f;
	gl_matrix[9] = 0.0f;
	gl_matrix[10] = 1.0f;
	gl_matrix[11] = 0.0f;

	gl_matrix[12] = mat[8];
	gl_matrix[13] = mat[9];
	gl_matrix[14] = 0.0f;
	gl_matrix[15] = 1.0f;
}

void GLRenderSystem::gainVertexColorBuffer(const void* vertices, unsigned int vertexCount)
{
	vertexCount *= 4;
	_vertexColorBuffer.clear();
	_vertexColorBuffer.resize(vertexCount);

	const Vertex* p = static_cast<const Vertex*>(vertices);
	for (unsigned int i = 0; i < vertexCount; i += 4)
	{
		p->_color.toOpenGLColor(&_vertexColorBuffer[i]);
		++p;
	}
}

const Dimension2di& GLRenderSystem::getCurRenderTargetSize() const
{
	if (_currentRenderTargetSize._width == 0)
		return Root::getSingleton().getActiveDevice()->getScreenSize();
	return _currentRenderTargetSize;
}

void GLRenderSystem::renderArray(const void* indexList, unsigned int primitiveCount, EPrimitiveType primType)
{
	GLenum indexSize = GL_UNSIGNED_INT;
	switch (primType)
	{
	case EPT_POINTS:
		glDrawArrays(GL_POINTS, 0, primitiveCount);
		break;
	case EPT_LINE_STRIP:
		glDrawElements(GL_LINE_STRIP, primitiveCount + 1, indexSize, indexList);
		break;
	case EPT_LINE_LOOP:
		glDrawElements(GL_LINE_LOOP, primitiveCount, indexSize, indexList);
		break;
	case EPT_LINES:
		glDrawElements(GL_LINES, primitiveCount * 2, indexSize, indexList);
		break;
	case EPT_TRIANGLE_STRIP:
		glDrawElements(GL_TRIANGLE_STRIP, primitiveCount + 2, indexSize, indexList);
		break;
	case EPT_TRIANGLE_FAN:
		glDrawElements(GL_TRIANGLE_FAN, primitiveCount + 2, indexSize, indexList);
		break;
	case EPT_TRIANGLES:
		glDrawElements(GL_TRIANGLES, primitiveCount * 3, indexSize, indexList);
		break;
	case EPT_QUAD_STRIP:
		glDrawElements(GL_QUAD_STRIP, primitiveCount * 2 + 2, indexSize, indexList);
		break;
	case EPT_QUADS:
		glDrawElements(GL_QUADS, primitiveCount * 4, indexSize, indexList);
		break;
	case EPT_POLYGON:
		glDrawElements(GL_POLYGON, primitiveCount, indexSize, indexList);
		break;
	}
}

void GLRenderSystem::createMaterialRenderers()
{
	_materialRenderers.push_back(new GLMaterialRenderer_SOLID(this));
	_materialRenderers.push_back(new GLMaterialRenderer_SOLID_2_LAYER(this));
	MaterialRendererPtr lmr = new GLMaterialRenderer_LIGHTMAP(this);
	_materialRenderers.push_back(lmr);
	_materialRenderers.push_back(lmr);
	_materialRenderers.push_back(lmr);
	_materialRenderers.push_back(lmr);
	_materialRenderers.push_back(lmr);
	_materialRenderers.push_back(lmr);
	_materialRenderers.push_back(lmr);
	_materialRenderers.push_back(new GLMaterialRenderer_SPHERE_MAP(this));
	_materialRenderers.push_back(new GLMaterialRenderer_REFLECTION_2_LAYER(this));
	_materialRenderers.push_back(new GLMaterialRenderer_TRANSPARENT_ADD_COLOR(this));
	_materialRenderers.push_back(new GLMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL(this));
	_materialRenderers.push_back(new GLMaterialRenderer_TRANSPARENT_VERTEX_ALPHA(this));
	_materialRenderers.push_back(new GLMaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER(this));
}

bool GLRenderSystem::updateVertexHardwareBuffer(HardwareBufferLink_OpenGL* hwBuffer)
{
	if (!hwBuffer)
		return false;

	const SubMesh* mb = hwBuffer->MeshBuffer;
	const Vertex* vertices = mb->getVertices();
	const unsigned int vertexCount = mb->getVertexCount();
	const unsigned int vertexSize = sizeof(Vertex);
	const unsigned char* vbuf = (const unsigned char*)vertices;

	bool newBuffer = false;
	if (!hwBuffer->VBO_verticesID)
	{
		glGenBuffers(1, &hwBuffer->VBO_verticesID);
		if (!hwBuffer->VBO_verticesID)
			return false;

		newBuffer = true;
	}
	else if (hwBuffer->VBO_verticesSize < vertexCount * vertexSize)
	{
		newBuffer = true;
	}

	glBindBuffer(GL_ARRAY_BUFFER, hwBuffer->VBO_verticesID);

	glGetError(); // clear error storage.

	if (!newBuffer)
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertexCount * vertexSize, vbuf);
	else
	{
		hwBuffer->VBO_verticesSize = vertexCount * vertexSize;

		if (hwBuffer->MappedVertex == EHM_STATIC)
			glBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSize, vbuf, GL_STATIC_DRAW);
		else if (hwBuffer->MappedVertex == EHM_DYNAMIC)
			glBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSize, vbuf, GL_DYNAMIC_DRAW);
		else
			glBufferData(GL_ARRAY_BUFFER, vertexCount * vertexSize, vbuf, GL_STREAM_DRAW);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return (glGetError() == GL_NO_ERROR);
}

bool GLRenderSystem::updateIndexHardwareBuffer(HardwareBufferLink_OpenGL* hwBuffer)
{
	if (!hwBuffer)
		return false;

	const SubMesh* mb = hwBuffer->MeshBuffer;
	const unsigned int* indices = mb->getIndices();
	unsigned int indexCount = mb->getIndexCount();
	GLenum indexSize = sizeof(unsigned int);

	bool newBuffer = false;
	if (!hwBuffer->EBO_indicesID)
	{
		glGenBuffers(1, &hwBuffer->EBO_indicesID);
		if (!hwBuffer->EBO_indicesID)
			return false;

		newBuffer = true;
	}
	else if (hwBuffer->EBO_indicesSize < indexCount * indexSize)
	{
		newBuffer = true;
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hwBuffer->EBO_indicesID);

	glGetError(); // clear error storage.

	if (!newBuffer)
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexCount * indexSize, indices);
	else
	{
		hwBuffer->EBO_indicesSize = indexCount * indexSize;

		if (hwBuffer->MappedIndex == EHM_STATIC)
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * indexSize, indices, GL_STATIC_DRAW);
		else if (hwBuffer->MappedIndex == EHM_DYNAMIC)
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * indexSize, indices, GL_DYNAMIC_DRAW);
		else
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * indexSize, indices, GL_STREAM_DRAW);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return (glGetError() == GL_NO_ERROR);
}

void GLRenderSystem::drawPixel(int x, int y, const Colour& color)
{
	const Dimension2di& renderTargetSize = getCurRenderTargetSize();
	if (x > renderTargetSize._width || x < 0 || y > renderTargetSize._height || y < 0)
		return;
	disableTextures();
	setRenderMode2D(color.getAlpha() < 255, false, false);
	glBegin(GL_POINTS);
	glColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
	glVertex2i(x, y);
	glEnd();
}

void GLRenderSystem::draw2DLine(const Position2di& startPos, const Position2di& endPos, Colour color/*=Color(255,255,255,255)*/)
{
	if (startPos == endPos)
		drawPixel(startPos._x, startPos._y, color);
	else
	{
		disableTextures();
		setRenderMode2D(color.getAlpha() < 255, false, false);
		glBegin(GL_LINES);
		glColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
		glVertex2f(startPos._x, startPos._y);
		glVertex2f(endPos._x, endPos._y);
		glEnd();
	}
}

void GLRenderSystem::draw3DLine(const Vector3df& start, const Vector3df& end, Colour color /*= Colour(255, 255, 255, 255)*/)
{
	setRenderMode3D();
	glBegin(GL_LINES);
	glColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
	glVertex3f(start._x, start._y, start._z);
	glVertex3f(end._x, end._y, end._z);
	glEnd();
}

void GLRenderSystem::draw2DRect(const Recti& rc, Colour colorLeftUp, Colour colorRightUp, Colour colorLeftDown, Colour colorRightDown, const Recti* clip /*= NULL*/)
{
	Recti rect = rc;

	if (clip)
		rect.clipAgainst(*clip);

	if (!rect.isValid())
		return;

	disableTextures();

	setRenderMode2D(colorLeftUp.getAlpha() < 255 || colorRightUp.getAlpha() < 255 || colorLeftDown.getAlpha() < 255 ||
		colorRightDown.getAlpha() < 255, false, false);

	glBegin(GL_QUADS);

	glColor4ub(colorLeftUp.getRed(), colorLeftUp.getGreen(), colorLeftUp.getBlue(), colorLeftUp.getAlpha());
	glVertex2f(GLfloat(rect._upperLeftCorner._x), GLfloat(rect._upperLeftCorner._y));

	glColor4ub(colorLeftDown.getRed(), colorLeftDown.getGreen(), colorLeftDown.getBlue(), colorLeftDown.getAlpha());
	glVertex2f(GLfloat(rect._upperLeftCorner._x), GLfloat(rect._lowerRightCorner._y));

	glColor4ub(colorRightDown.getRed(), colorRightDown.getGreen(), colorRightDown.getBlue(), colorRightDown.getAlpha());
	glVertex2f(GLfloat(rect._lowerRightCorner._x), GLfloat(rect._lowerRightCorner._y));

	glColor4ub(colorRightUp.getRed(), colorRightUp.getGreen(), colorRightUp.getBlue(), colorRightUp.getAlpha());
	glVertex2f(GLfloat(rect._lowerRightCorner._x), GLfloat(rect._upperLeftCorner._y));

	glEnd();
}

void GLRenderSystem::draw2DImage(TexturePtr texture, const Position2di& destPos,
	const Recti& sourceRect, const Recti* clipRect /*= NULL*/, Colour color /*= Colour(255, 255, 255, 255)*/,
	bool useAlphaChannelOfTexture /*= false*/)
{
	if (!texture)
		return;

	if (!sourceRect.isValid())
		return;

	Position2di targetPos(destPos);
	Position2di sourcePos(sourceRect._upperLeftCorner);
	Dimension2di sourceSize(sourceRect.getSize());
	if (clipRect)
	{
		if (targetPos._x < clipRect->_upperLeftCorner._x)
		{
			sourceSize._width += targetPos._x - clipRect->_upperLeftCorner._x;
			if (sourceSize._width <= 0)
				return;

			sourcePos._x -= targetPos._x - clipRect->_upperLeftCorner._x;
			targetPos._x = clipRect->_upperLeftCorner._x;
		}

		if (targetPos._x + sourceSize._width > clipRect->_lowerRightCorner._x)
		{
			sourceSize._width -= (targetPos._x + sourceSize._width) - clipRect->_lowerRightCorner._x;
			if (sourceSize._width <= 0)
				return;
		}

		if (targetPos._y < clipRect->_upperLeftCorner._y)
		{
			sourceSize._height += targetPos._y - clipRect->_upperLeftCorner._y;
			if (sourceSize._height <= 0)
				return;

			sourcePos._y -= targetPos._y - clipRect->_upperLeftCorner._y;
			targetPos._y = clipRect->_upperLeftCorner._y;
		}

		if (targetPos._y + sourceSize._height > clipRect->_lowerRightCorner._y)
		{
			sourceSize._height -= (targetPos._y + sourceSize._height) - clipRect->_lowerRightCorner._y;
			if (sourceSize._height <= 0)
				return;
		}
	}

	if (targetPos._x < 0)
	{
		sourceSize._width += targetPos._x;
		if (sourceSize._width <= 0)
			return;

		sourcePos._x -= targetPos._x;
		targetPos._x = 0;
	}

	const Dimension2di& renderTargetSize = getCurRenderTargetSize();

	if (targetPos._x + sourceSize._width > renderTargetSize._width)
	{
		sourceSize._width -= (targetPos._x + sourceSize._width) - renderTargetSize._width;
		if (sourceSize._width <= 0)
			return;
	}

	if (targetPos._y < 0)
	{
		sourceSize._height += targetPos._y;
		if (sourceSize._height <= 0)
			return;

		sourcePos._y -= targetPos._y;
		targetPos._y = 0;
	}

	if (targetPos._y + sourceSize._height > renderTargetSize._height)
	{
		sourceSize._height -= (targetPos._y + sourceSize._height) - renderTargetSize._height;
		if (sourceSize._height <= 0)
			return;
	}

	const Dimension2di& ss = texture->getImgSize();
	const float invW = 1.f / static_cast<float>(ss._width);
	const float invH = 1.f / static_cast<float>(ss._height);
	const Rectf tcoords(
		sourcePos._x * invW,
		sourcePos._y * invH,
		(sourcePos._x + sourceSize._width) * invW,
		(sourcePos._y + sourceSize._height) * invH);

	const Recti poss(targetPos, sourceSize);

	disableTextures(1);
	if (!setActiveTexture(0, texture.get()))
		return;

	setRenderMode2D(color.getAlpha() < 255, true, useAlphaChannelOfTexture);

	glColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());

	glBegin(GL_QUADS);

	glTexCoord2f(tcoords._upperLeftCorner._x, tcoords._upperLeftCorner._y);
	glVertex2f(GLfloat(poss._upperLeftCorner._x), GLfloat(poss._upperLeftCorner._y));

	glTexCoord2f(tcoords._upperLeftCorner._x, tcoords._lowerRightCorner._y);
	glVertex2f(GLfloat(poss._upperLeftCorner._x), GLfloat(poss._lowerRightCorner._y));

	glTexCoord2f(tcoords._lowerRightCorner._x, tcoords._lowerRightCorner._y);
	glVertex2f(GLfloat(poss._lowerRightCorner._x), GLfloat(poss._lowerRightCorner._y));

	glTexCoord2f(tcoords._lowerRightCorner._x, tcoords._upperLeftCorner._y);
	glVertex2f(GLfloat(poss._lowerRightCorner._x), GLfloat(poss._upperLeftCorner._y));

	glEnd();
}

void GLRenderSystem::draw2DImage(TexturePtr texture, const Recti& destRect, const Recti& sourceRect,
	const Recti* clipRect /*= NULL*/, const Colour* const colors /*= NULL*/, bool useAlphaChannelOfTexture /*= false*/)
{
	if (!texture)
		return;

	const Dimension2di& ss = texture->getImgSize();
	const float invW = 1.0f / static_cast<float>(ss._width);
	const float invH = 1.0f / static_cast<float>(ss._height);
	const Rectf tcoords(
		sourceRect._upperLeftCorner._x * invW,
		sourceRect._upperLeftCorner._y * invH,
		sourceRect._lowerRightCorner._x * invW,
		sourceRect._lowerRightCorner._y * invH);

	const Colour temp[4] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
	const Colour* const useColor = colors ? colors : temp;

	disableTextures(1);
	setActiveTexture(0, texture.get());
	setRenderMode2D(useColor[0].getAlpha() < 255 || useColor[1].getAlpha() < 255 ||
		useColor[2].getAlpha() < 255 || useColor[3].getAlpha() < 255,
		true, useAlphaChannelOfTexture);

	if (clipRect)
	{
		if (!clipRect->isValid())
			return;

		glEnable(GL_SCISSOR_TEST);
		const Dimension2di& renderTargetSize = getCurRenderTargetSize();
		glScissor(clipRect->_upperLeftCorner._x, renderTargetSize._height - clipRect->_lowerRightCorner._y,
			clipRect->getWidth(), clipRect->getHeight());
	}

	glBegin(GL_QUADS);

	glColor4ub(useColor[0].getRed(), useColor[0].getGreen(), useColor[0].getBlue(), useColor[0].getAlpha());
	glTexCoord2f(tcoords._upperLeftCorner._x, tcoords._upperLeftCorner._y);
	glVertex2f(GLfloat(destRect._upperLeftCorner._x), GLfloat(destRect._upperLeftCorner._y));

	glColor4ub(useColor[1].getRed(), useColor[1].getGreen(), useColor[1].getBlue(), useColor[1].getAlpha());
	glTexCoord2f(tcoords._upperLeftCorner._x, tcoords._lowerRightCorner._y);
	glVertex2f(GLfloat(destRect._upperLeftCorner._x), GLfloat(destRect._lowerRightCorner._y));

	glColor4ub(useColor[2].getRed(), useColor[2].getGreen(), useColor[2].getBlue(), useColor[2].getAlpha());
	glTexCoord2f(tcoords._lowerRightCorner._x, tcoords._lowerRightCorner._y);
	glVertex2f(GLfloat(destRect._lowerRightCorner._x), GLfloat(destRect._lowerRightCorner._y));

	glColor4ub(useColor[3].getRed(), useColor[3].getGreen(), useColor[3].getBlue(), useColor[3].getAlpha());
	glTexCoord2f(tcoords._lowerRightCorner._x, tcoords._upperLeftCorner._y);
	glVertex2f(GLfloat(destRect._lowerRightCorner._x), GLfloat(destRect._upperLeftCorner._y));

	glEnd();

	if (clipRect)
		glDisable(GL_SCISSOR_TEST);
}

void GLRenderSystem::draw2DImageBatch(TexturePtr texture, const vector<Position2di>& positions,
	const vector<Recti>& sourceRects, const Recti* clipRect /*= NULL*/,
	Colour color /*= Colour(255, 255, 255, 255)*/, bool useAlphaChannelOfTexture /*= false*/)
{
	if (!texture)
		return;

	const unsigned int drawCount = min_<unsigned int>(positions.size(), sourceRects.size());

	const Dimension2di& ss = texture->getImgSize();
	const float invW = 1.f / static_cast<float>(ss._width);
	const float invH = 1.f / static_cast<float>(ss._height);
	const Dimension2di& renderTargetSize = getCurRenderTargetSize();

	disableTextures(1);
	if (!setActiveTexture(0, texture.get()))
		return;

	setRenderMode2D(color.getAlpha() < 255, true, useAlphaChannelOfTexture);

	glColor4ub(color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
	glBegin(GL_QUADS);

	for (unsigned int i = 0; i < drawCount; ++i)
	{
		if (!sourceRects[i].isValid())
			continue;

		Position2di targetPos(positions[i]);
		Position2di sourcePos(sourceRects[i]._upperLeftCorner);
		Dimension2di sourceSize(sourceRects[i].getSize());
		if (clipRect)
		{
			if (targetPos._x < clipRect->_upperLeftCorner._x)
			{
				sourceSize._width += targetPos._x - clipRect->_upperLeftCorner._x;
				if (sourceSize._width <= 0)
					continue;

				sourcePos._x -= targetPos._x - clipRect->_upperLeftCorner._x;
				targetPos._x = clipRect->_upperLeftCorner._x;
			}

			if (targetPos._x + sourceSize._width > clipRect->_lowerRightCorner._x)
			{
				sourceSize._width -= (targetPos._x + sourceSize._width) - clipRect->_lowerRightCorner._x;
				if (sourceSize._width <= 0)
					continue;
			}

			if (targetPos._y < clipRect->_upperLeftCorner._y)
			{
				sourceSize._height += targetPos._y - clipRect->_upperLeftCorner._y;
				if (sourceSize._height <= 0)
					continue;

				sourcePos._y -= targetPos._y - clipRect->_upperLeftCorner._y;
				targetPos._y = clipRect->_upperLeftCorner._y;
			}

			if (targetPos._y + sourceSize._height > clipRect->_lowerRightCorner._y)
			{
				sourceSize._height -= (targetPos._y + sourceSize._height) - clipRect->_lowerRightCorner._y;
				if (sourceSize._height <= 0)
					continue;
			}
		}

		if (targetPos._x < 0)
		{
			sourceSize._width += targetPos._x;
			if (sourceSize._width <= 0)
				continue;

			sourcePos._x -= targetPos._x;
			targetPos._x = 0;
		}

		if (targetPos._x + sourceSize._width > renderTargetSize._width)
		{
			sourceSize._width -= (targetPos._x + sourceSize._width) - renderTargetSize._width;
			if (sourceSize._width <= 0)
				continue;
		}

		if (targetPos._y < 0)
		{
			sourceSize._height += targetPos._y;
			if (sourceSize._height <= 0)
				continue;

			sourcePos._y -= targetPos._y;
			targetPos._y = 0;
		}

		if (targetPos._y + sourceSize._height > renderTargetSize._height)
		{
			sourceSize._height -= (targetPos._y + sourceSize._height) - renderTargetSize._height;
			if (sourceSize._height <= 0)
				continue;
		}

		const Rectf tcoords(
			sourcePos._x * invW,
			sourcePos._y * invH,
			(sourcePos._x + sourceSize._width) * invW,
			(sourcePos._y + sourceSize._height) * invH);

		const Recti poss(targetPos, sourceSize);

		glTexCoord2f(tcoords._upperLeftCorner._x, tcoords._upperLeftCorner._y);
		glVertex2f(GLfloat(poss._upperLeftCorner._x), GLfloat(poss._upperLeftCorner._y));

		glTexCoord2f(tcoords._upperLeftCorner._x, tcoords._lowerRightCorner._y);
		glVertex2f(GLfloat(poss._upperLeftCorner._x), GLfloat(poss._lowerRightCorner._y));

		glTexCoord2f(tcoords._lowerRightCorner._x, tcoords._lowerRightCorner._y);
		glVertex2f(GLfloat(poss._lowerRightCorner._x), GLfloat(poss._lowerRightCorner._y));

		glTexCoord2f(tcoords._lowerRightCorner._x, tcoords._upperLeftCorner._y);
		glVertex2f(GLfloat(poss._lowerRightCorner._x), GLfloat(poss._upperLeftCorner._y));
	}
	glEnd();
}

void GLRenderSystem::draw2DImageBatch(TexturePtr texture, const vector<Recti>& destRects,
	const vector<Recti>& sourceRects, const Recti* clipRect /*= NULL*/,
	const vector<Colour>* colors /*= NULL*/, bool useAlphaChannelOfTexture /*= false*/)
{
	if (!texture)
		return;
	disableTextures(1);
	if (!setActiveTexture(0, texture.get()))
		return;
	setRenderMode2D(true, true, useAlphaChannelOfTexture);

	const Dimension2di& ss = texture->getImgSize();
	const float invW = 1.0f / static_cast<float>(ss._width);
	const float invH = 1.0f / static_cast<float>(ss._height);

	const unsigned int drawCount = MIN(destRects.size(), sourceRects.size());
	Colour useColor(0xFFFFFFFF);

	if (clipRect)
	{
		if (!clipRect->isValid())
			return;

		glEnable(GL_SCISSOR_TEST);
		glScissor(clipRect->_upperLeftCorner._x, getCurRenderTargetSize()._height - clipRect->_lowerRightCorner._y,
			clipRect->getWidth(), clipRect->getHeight());
	}

	glBegin(GL_QUADS);
	for (unsigned int i = 0; i < drawCount; ++i)
	{
		if (!destRects[i].isValid() || !sourceRects[i].isValid())
			continue;

		const Rectf tcoords(
			sourceRects[i]._upperLeftCorner._x * invW,
			sourceRects[i]._upperLeftCorner._y * invH,
			sourceRects[i]._lowerRightCorner._x * invW,
			sourceRects[i]._lowerRightCorner._y * invH);

		if (NULL != colors && i < colors->size())
			useColor = (*colors)[i];
		else
			useColor.set(0xFFFFFFFF);

		glColor4ub(useColor.getRed(), useColor.getGreen(), useColor.getBlue(), useColor.getAlpha());

		glTexCoord2f(tcoords._upperLeftCorner._x, tcoords._upperLeftCorner._y);
		glVertex2f(GLfloat(destRects[i]._upperLeftCorner._x), GLfloat(destRects[i]._upperLeftCorner._y));

		glTexCoord2f(tcoords._upperLeftCorner._x, tcoords._lowerRightCorner._y);
		glVertex2f(GLfloat(destRects[i]._upperLeftCorner._x), GLfloat(destRects[i]._lowerRightCorner._y));

		glTexCoord2f(tcoords._lowerRightCorner._x, tcoords._lowerRightCorner._y);
		glVertex2f(GLfloat(destRects[i]._lowerRightCorner._x), GLfloat(destRects[i]._lowerRightCorner._y));

		glTexCoord2f(tcoords._lowerRightCorner._x, tcoords._upperLeftCorner._y);
		glVertex2f(GLfloat(destRects[i]._lowerRightCorner._x), GLfloat(destRects[i]._upperLeftCorner._y));
	}
	glEnd();

	if (clipRect)
		glDisable(GL_SCISSOR_TEST);
}

void GLRenderSystem::drawVertexPrimitiveListBegan(const void* vertices, unsigned int vertexCount)
{
	if (!vertexCount)
		return;

	setRenderMode3D();

	if (_curMaterial._materialType > EMT_TRANSPARENT_REFLECTION_2_LAYER)
	{
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);

		if (vertices)
		{
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), &(static_cast<const Vertex*>(vertices))[0]._position);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), &(static_cast<const Vertex*>(vertices))[0]._normal);
			glVertexAttribPointer(2, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(Vertex), &(static_cast<const Vertex*>(vertices))[0]._color);
			glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), &(static_cast<const Vertex*>(vertices))[0]._texCoord);
		}
		else
		{
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), buffer_offset(12));
			glVertexAttribPointer(2, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(Vertex), buffer_offset(24));
			glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), buffer_offset(28));
		}
	}
	else
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		if (vertices)
		{
			gainVertexColorBuffer(vertices, vertexCount);
			glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &(static_cast<const Vertex*>(vertices))[0]._position);
			glNormalPointer(GL_FLOAT, sizeof(Vertex), &(static_cast<const Vertex*>(vertices))[0]._normal);
			glColorPointer(4, GL_UNSIGNED_BYTE, 0, &_vertexColorBuffer[0]);
			glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &(static_cast<const Vertex*>(vertices))[0]._texCoord);
		}
		else
		{
			glVertexPointer(3, GL_FLOAT, sizeof(Vertex), 0);
			glNormalPointer(GL_FLOAT, sizeof(Vertex), buffer_offset(12));
			glColorPointer(GL_BGRA, GL_UNSIGNED_BYTE, sizeof(Vertex), buffer_offset(24));
			glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), buffer_offset(28));
		}
	}
}

void GLRenderSystem::drawVertexPrimitiveList(const unsigned int* indexList, unsigned int primitiveCount, EPrimitiveType primType)
{
	if (!checkPrimitiveCount(primitiveCount))
		return;

	setRenderMode3D();
	_primitivesDrawn += primitiveCount;
	renderArray(indexList, primitiveCount, primType);
}

void GLRenderSystem::drawVertexPrimitiveListEnded()
{
	if (_curMaterial._materialType > EMT_TRANSPARENT_REFLECTION_2_LAYER)
	{
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
	}
	else
	{
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}

RenderSystem::HardwareBufferLink* GLRenderSystem::createHardwareBuffer(SubMesh* mb)
{
	if (!mb || (mb->getMappedVertexHint() == EHM_NEVER && mb->getMappedIndexHint() == EHM_NEVER))
		return NULL;

	HardwareBufferLink_OpenGL* hwBuffer = new HardwareBufferLink_OpenGL(mb);
	_hardwareBufferMap.insert(make_pair(hwBuffer->MeshBuffer, hwBuffer));

	hwBuffer->ChangedVertexID = hwBuffer->MeshBuffer->getChangedVertexID();
	hwBuffer->ChangedIndexID = hwBuffer->MeshBuffer->getChangedIndexID();
	hwBuffer->MappedVertex = mb->getMappedVertexHint();
	hwBuffer->MappedIndex = mb->getMappedIndexHint();

	if (!updateHardwareBuffer(hwBuffer))
	{
		deleteHardwareBuffer(hwBuffer);
		return NULL;
	}

	return hwBuffer;
}

bool GLRenderSystem::updateHardwareBuffer(HardwareBufferLink* hwBuffer)
{
	if (!hwBuffer)
		return false;

	if (hwBuffer->MappedVertex != EHM_NEVER)
	{
		if (hwBuffer->ChangedVertexID != hwBuffer->MeshBuffer->getChangedVertexID()
			|| !((HardwareBufferLink_OpenGL*)hwBuffer)->VBO_verticesID)
		{

			hwBuffer->ChangedVertexID = hwBuffer->MeshBuffer->getChangedVertexID();
			if (!updateVertexHardwareBuffer((HardwareBufferLink_OpenGL*)hwBuffer))
				return false;
		}
	}

	if (hwBuffer->MappedIndex != EHM_NEVER)
	{
		if (hwBuffer->ChangedIndexID != hwBuffer->MeshBuffer->getChangedIndexID()
			|| !((HardwareBufferLink_OpenGL*)hwBuffer)->EBO_indicesID)
		{

			hwBuffer->ChangedIndexID = hwBuffer->MeshBuffer->getChangedIndexID();
			if (!updateIndexHardwareBuffer((HardwareBufferLink_OpenGL*)hwBuffer))
				return false;
		}
	}

	return true;
}

void GLRenderSystem::drawHardwareBuffer(HardwareBufferLink* hwBuffer)
{
	if (!hwBuffer)
		return;

	updateHardwareBuffer(hwBuffer);
	hwBuffer->LastUsed = 0;

	HardwareBufferLink_OpenGL *glhwBuffer = (HardwareBufferLink_OpenGL*)hwBuffer;

	const SubMesh* mb = glhwBuffer->MeshBuffer;
	const void* vertices = mb->getVertices();
	const unsigned int* indexList = mb->getIndices();

	if (glhwBuffer->MappedVertex != EHM_NEVER)
	{
		glBindBuffer(GL_ARRAY_BUFFER, glhwBuffer->VBO_verticesID);
		vertices = NULL;
	}

	if (glhwBuffer->MappedIndex != EHM_NEVER)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glhwBuffer->EBO_indicesID);
		indexList = NULL;
	}

	drawVertexPrimitiveListBegan(vertices, mb->getVertexCount());
	drawVertexPrimitiveList(indexList, mb->getPrimCount(), mb->getPrimType());
	drawVertexPrimitiveListEnded();

	if (glhwBuffer->MappedVertex != EHM_NEVER)
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	if (glhwBuffer->MappedIndex != EHM_NEVER)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

GLRenderSystem::HardwareBufferLinkItor GLRenderSystem::deleteHardwareBuffer(HardwareBufferLink* hwBuffer)
{
	HardwareBufferLinkItor it = _hardwareBufferMap.end();
	if (!hwBuffer)
		return it;

	HardwareBufferLink_OpenGL* glhwBuffer = (HardwareBufferLink_OpenGL*)hwBuffer;
	if (glhwBuffer->VBO_verticesID)
	{
		glDeleteBuffers(1, &glhwBuffer->VBO_verticesID);
		glhwBuffer->VBO_verticesID = 0;
	}

	if (glhwBuffer->EBO_indicesID)
	{
		glDeleteBuffers(1, &glhwBuffer->EBO_indicesID);
		glhwBuffer->EBO_indicesID = 0;
	}

	return RenderSystem::deleteHardwareBuffer(hwBuffer);
}

TexturePtr GLRenderSystem::createTexture(const wstring& filename, Image* image, bool generateMipLevels)
{
	if (GLVersion.major < 3)
		generateMipLevels = false;
	return new GLTexture(filename, image, generateMipLevels, this);
}

DisplayList* GLRenderSystem::createDisplayList(const wstring& name /*= L""*/)
{
	DisplayList* displayList = new GLDisplayList(name, this);
	map<wstring, DisplayList*>::iterator it = _displayListSet.find(displayList->getName());
	if (it != _displayListSet.end())
	{
		SAFE_DELETE(it->second);
		_displayListSet.erase(it);
	}
	_displayListSet[displayList->getName()] = displayList;
	return displayList;
}

Tesselator* GLRenderSystem::createTesselator(const wstring& name /*= L""*/)
{
	Tesselator* tesselator = new GLTesselator(name);
	map<wstring, Tesselator*>::iterator it = _tesselators.find(tesselator->getName());
	if (it != _tesselators.end())
	{
		SAFE_DELETE(it->second);
		_tesselators.erase(it);
	}
	_tesselators[tesselator->getName()] = tesselator;
	return tesselator;
}

int GLRenderSystem::addShaderMaterial(const char* vertexShaderProgram /*= ""*/,
	const char* pixelShaderProgram /*= ""*/, ShaderConstantSetCallback* callback /*= NULL*/,
	EMaterialType baseMaterial /*= EMT_SOLID*/)
{
	bool succ = false;
	MaterialRendererPtr smr(new GLShaderMaterialRenderer(this, succ,
		vertexShaderProgram, pixelShaderProgram, callback, getMaterialRenderer(baseMaterial)));
	if (succ)
	{
		_materialRenderers.push_back(smr);
		return (int)_materialRenderers.size() - 1;
	}
	return -1;
}

void GLRenderSystem::setBasicRenderStates(const Material& material, const Material& lastMaterial, bool resetAllRenderStates)
{
	if (resetAllRenderStates || lastMaterial._clockwise != material._clockwise)
		glFrontFace(material._clockwise ? GL_CW : GL_CCW);

	if (resetAllRenderStates || lastMaterial._gouraudShading != material._gouraudShading)
		glShadeModel(material._gouraudShading ? GL_SMOOTH : GL_FLAT);

	if (resetAllRenderStates ||
		lastMaterial._ambientColor != material._ambientColor ||
		lastMaterial._diffuseColor != material._diffuseColor ||
		lastMaterial._specularColor != material._specularColor ||
		lastMaterial._emissiveColor != material._emissiveColor ||
		lastMaterial._shininess != material._shininess)
	{
		GLfloat color[4];

		float inv = 1.0f / 255.0f;

		color[0] = material._ambientColor.getRed() * inv;
		color[1] = material._ambientColor.getGreen() * inv;
		color[2] = material._ambientColor.getBlue() * inv;
		color[3] = material._ambientColor.getAlpha() * inv;
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, color);

		color[0] = material._diffuseColor.getRed() * inv;
		color[1] = material._diffuseColor.getGreen() * inv;
		color[2] = material._diffuseColor.getBlue() * inv;
		color[3] = material._diffuseColor.getAlpha() * inv;
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);

		color[0] = material._specularColor.getRed() * inv;
		color[1] = material._specularColor.getGreen() * inv;
		color[2] = material._specularColor.getBlue() * inv;
		color[3] = material._specularColor.getAlpha() * inv;
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color);

		color[0] = material._emissiveColor.getRed() * inv;
		color[1] = material._emissiveColor.getGreen() * inv;
		color[2] = material._emissiveColor.getBlue() * inv;
		color[3] = material._emissiveColor.getAlpha() * inv;
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, color);

		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material._shininess);
	}

	if (resetAllRenderStates || lastMaterial._wireframe != material._wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, material._wireframe ? GL_LINE : GL_FILL);

	if (resetAllRenderStates || lastMaterial._lighting != material._lighting)
	{
		if (material._lighting)
			glEnable(GL_LIGHTING);
		else
			glDisable(GL_LIGHTING);
	}

	if (resetAllRenderStates || lastMaterial._zBuffer != material._zBuffer)
	{
		if (material._zBuffer)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}

	if (resetAllRenderStates || lastMaterial._zWriteEnable != material._zWriteEnable)
		glDepthMask(material._zWriteEnable ? GL_TRUE : GL_FALSE);

	if (resetAllRenderStates || lastMaterial._stencilBuffer != material._stencilBuffer)
	{
		if (material._stencilBuffer)
			glEnable(GL_STENCIL_TEST);
		else
			glDisable(GL_STENCIL_TEST);
	}

	if (material._stencilBuffer)
	{
		if (lastMaterial._stencilFunc != material._stencilFunc ||
			lastMaterial._stencilRef != material._stencilRef ||
			lastMaterial._stencilFuncMask != material._stencilFuncMask)
		{
			GLenum func = GL_NEVER;
			GLint ref = material._stencilRef;
			GLuint mask = material._stencilFuncMask;

			switch (material._stencilFunc)
			{
			case ESF_NEVER: func = GL_NEVER; break;
			case ESF_LESS: func = GL_LESS; break;
			case ESF_LEQUAL: func = GL_LEQUAL; break;
			case ESF_GREATER: func = GL_GREATER; break;
			case ESF_GEQUAL: func = GL_GEQUAL; break;
			case ESF_EQUAL: func = GL_EQUAL; break;
			case ESF_NOTEQUAL: func = GL_NOTEQUAL; break;
			case ESF_ALWAYS: func = GL_ALWAYS; break;
			default: break;
			}

			glStencilFunc(func, ref, mask);
		}

		if (lastMaterial._stencilMask != material._stencilMask)
		{
			GLuint mask = material._stencilMask;
			glStencilMask(mask);
		}

		if (lastMaterial._stencilFail != material._stencilFail ||
			lastMaterial._depthFail != material._depthFail ||
			lastMaterial._stencilDepthPass != material._stencilDepthPass)
		{
			GLenum fail = GL_KEEP;
			GLenum zfail = GL_KEEP;
			GLenum zpass = GL_KEEP;

			switch (material._stencilFail)
			{
			case ESO_KEEP: fail = GL_KEEP; break;
			case ESO_ZERO: fail = GL_ZERO; break;
			case ESO_REPLACE: fail = GL_REPLACE; break;
			case ESO_INCR: fail = GL_INCR; break;
			case ESO_INCR_WRAP: fail = GL_INCR_WRAP; break;
			case ESO_DECR: fail = GL_DECR; break;
			case ESO_DECR_WRAP: fail = GL_DECR_WRAP; break;
			case ESO_INVERT: fail = GL_INVERT; break;
			default: break;
			}

			switch (material._depthFail)
			{
			case ESO_KEEP: zfail = GL_KEEP; break;
			case ESO_ZERO: zfail = GL_ZERO; break;
			case ESO_REPLACE: zfail = GL_REPLACE; break;
			case ESO_INCR: zfail = GL_INCR; break;
			case ESO_INCR_WRAP: zfail = GL_INCR_WRAP; break;
			case ESO_DECR: zfail = GL_DECR; break;
			case ESO_DECR_WRAP: zfail = GL_DECR_WRAP; break;
			case ESO_INVERT: zfail = GL_INVERT; break;
			default: break;
			}

			switch (material._stencilDepthPass)
			{
			case ESO_KEEP: zpass = GL_KEEP; break;
			case ESO_ZERO: zpass = GL_ZERO; break;
			case ESO_REPLACE: zpass = GL_REPLACE; break;
			case ESO_INCR: zpass = GL_INCR; break;
			case ESO_INCR_WRAP: zpass = GL_INCR_WRAP; break;
			case ESO_DECR: zpass = GL_DECR; break;
			case ESO_DECR_WRAP: zpass = GL_DECR_WRAP; break;
			case ESO_INVERT: zpass = GL_INVERT; break;
			default: break;
			}

			glStencilOp(fail, zfail, zpass);
		}
	}

	if (resetAllRenderStates || lastMaterial._backfaceCulling != material._backfaceCulling)
	{
		if (material._backfaceCulling)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
	}

	if (resetAllRenderStates || lastMaterial._fogEnable != material._fogEnable)
	{
		if (material._fogEnable)
			glEnable(GL_FOG);
		else
			glDisable(GL_FOG);
	}

	for (int i = 0; i < _maxTextureUnits; ++i)
	{
		if (!_texturesUnitSet[i])
			continue;

		glActiveTexture(GL_TEXTURE0 + i);

		if (material._textureLayer[i]._lodBias)
		{
			const float tmp = clamp_(material._textureLayer[i]._lodBias * 0.125f, -_maxTextureLODBias, _maxTextureLODBias);
			glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, tmp);
		}
		else
			glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, 0.0f);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
			(material._textureLayer[i]._bilinearFilter || material._textureLayer[i]._trilinearFilter) ? GL_LINEAR : GL_NEAREST);

		if (material._useMipMaps && _texturesUnitSet[i]->hasMipMaps())
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				material._textureLayer[i]._trilinearFilter ? GL_LINEAR_MIPMAP_LINEAR :
				material._textureLayer[i]._bilinearFilter ? GL_LINEAR_MIPMAP_NEAREST :
				GL_NEAREST_MIPMAP_NEAREST);
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			(material._textureLayer[i]._bilinearFilter || material._textureLayer[i]._trilinearFilter) ? GL_LINEAR : GL_NEAREST);
	}

	if (resetAllRenderStates || lastMaterial._thickness != material._thickness)
	{
		if (_antiAliasFactor)
		{
			// we don't use point smoothing.
			glPointSize(clamp_(static_cast<GLfloat>(material._thickness), _dimAliasedPoint[0], _dimAliasedPoint[1]));
			glLineWidth(clamp_(static_cast<GLfloat>(material._thickness), _dimSmoothedLine[0], _dimSmoothedLine[1]));
		}
		else
		{
			glPointSize(clamp_(static_cast<GLfloat>(material._thickness), _dimAliasedPoint[0], _dimAliasedPoint[1]));
			glLineWidth(clamp_(static_cast<GLfloat>(material._thickness), _dimAliasedLine[0], _dimAliasedLine[1]));
		}
	}

	if (resetAllRenderStates || lastMaterial._antiAliasing != material._antiAliasing)
	{
		if (material._antiAliasing & EAAM_ALPHA_TO_COVERAGE)
			glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		else if (lastMaterial._antiAliasing & EAAM_ALPHA_TO_COVERAGE)
			glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);

		if ((_antiAliasFactor >= 2) && (material._antiAliasing & EAAM_ON))
			glEnable(GL_MULTISAMPLE);
		else
			glDisable(GL_MULTISAMPLE);

		if ((material._antiAliasing & EAAM_LINE_SMOOTH) != (lastMaterial._antiAliasing & EAAM_LINE_SMOOTH))
		{
			if (material._antiAliasing & EAAM_LINE_SMOOTH)
				glEnable(GL_LINE_SMOOTH);
			else if (lastMaterial._antiAliasing & EAAM_LINE_SMOOTH)
				glDisable(GL_LINE_SMOOTH);
		}

		if ((material._antiAliasing & EAAM_POINT_SMOOTH) != (lastMaterial._antiAliasing & EAAM_POINT_SMOOTH))
		{
			if (material._antiAliasing & EAAM_POINT_SMOOTH)
				glEnable(GL_POINT_SMOOTH);
			else if (lastMaterial._antiAliasing & EAAM_POINT_SMOOTH)
				glDisable(GL_POINT_SMOOTH);
		}
	}

	setWrapMode(material);
	if (_maxTextureUnits > 0)
		glActiveTexture(GL_TEXTURE0);
}

void GLRenderSystem::testGLErrorBegan()
{
	glGetError(); // clear error storage.
}

bool GLRenderSystem::testGLErrorEnded(const wstring& errLog /*= L""*/)
{
	wstring infoLog;
	GLenum glErr = glGetError();
	switch (glErr)
	{
	case GL_NO_ERROR: return false;
	case GL_INVALID_ENUM: infoLog = L"GL_INVALID_ENUM"; break;
	case GL_INVALID_VALUE: infoLog = L"GL_INVALID_VALUE"; break;
	case GL_INVALID_OPERATION: infoLog = L"GL_INVALID_OPERATION"; break;
	case GL_STACK_OVERFLOW: infoLog = L"GL_STACK_OVERFLOW"; break;
	case GL_STACK_UNDERFLOW: infoLog = L"GL_STACK_UNDERFLOW"; break;
	case GL_OUT_OF_MEMORY: infoLog = L"GL_OUT_OF_MEMORY"; break;
	case GL_INVALID_FRAMEBUFFER_OPERATION: infoLog = L"GL_INVALID_FRAMEBUFFER_OPERATION"; break;
	case GL_CONTEXT_LOST: infoLog = L"GL_CONTEXT_LOST"; break;
	}
	Log::wprint(ELL_ERROR, L"%s[%s]", errLog.c_str(), infoLog.c_str());
	return true;
}
