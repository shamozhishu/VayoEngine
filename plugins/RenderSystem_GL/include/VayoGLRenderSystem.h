/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* OpenGL渲染系统
\*************************************************************************/
#ifndef __VAYO_GL_RENDER_SYSTEM_H__
#define __VAYO_GL_RENDER_SYSTEM_H__

#include "VayoGLSupport.h"
#include "VayoRenderSystem.h"
NS_VAYO_BEGIN

class GLRenderSystem : public RenderSystem
{
	DISALLOW_COPY_AND_ASSIGN(GLRenderSystem)
public:
	GLRenderSystem(const wstring& name);
	~GLRenderSystem();

	TexturePtr   createTexture(const wstring& name, Image* image, bool generateMipLevels);
	DisplayList* createDisplayList(const wstring& name = L"");
	Tesselator*  createTesselator(const wstring& name = L"");

	bool init(bool mark = false);
	bool isActive() const;
	bool beginScene(bool backBuffer, bool zBuffer, bool stencilBuffer, Colour color);
	bool endScene();
	const Matrix4x4& getTransform(ETransformationState state) const;
	void setTransform(ETransformationState state, const Matrix4x4& mat);
	const Dimension2di& getCurrentRenderTargetSize() const;
	void setViewpot(const Recti& area);
	void setAmbientLight(const Colour& color);
	void setMaterial(const Material& material);
	void addDynamicLight(const LightData& light);
	void removeAllDynamicLights();

	void drawDisplayList(DisplayList* displayList);
	void drawPixel(int x, int y, const Colour& color);
	void draw2DLine(const Position2di& startPos, const Position2di& endPos, Colour color = Colour(255, 255, 255, 255));
	void draw3DLine(const Vector3df& start, const Vector3df& end, Colour color = Colour(255, 255, 255, 255));
	void draw2DRect(const Recti& rc, Colour colorLeftUp, Colour colorRightUp, Colour colorLeftDown, Colour colorRightDown, const Recti* clip = NULL);

	void draw2DImage(TexturePtr texture, const Position2di& destPos,
		const Recti& sourceRect, const Recti* clipRect = NULL,
		Colour color = Colour(255, 255, 255, 255), bool useAlphaChannelOfTexture = false);

	void draw2DImage(TexturePtr texture, const Recti& destRect,
		const Recti& sourceRect, const Recti* clipRect = NULL,
		const Colour* const colors = NULL, bool useAlphaChannelOfTexture = false);

	void draw2DImageBatch(TexturePtr texture,
		const vector<Position2di>& positions,
		const vector<Recti>& sourceRects,
		const Recti* clipRect = NULL,
		Colour color = Colour(255, 255, 255, 255),
		bool useAlphaChannelOfTexture = false);

	void draw2DImageBatch(TexturePtr texture,
		const vector<Recti>& destRects,
		const vector<Recti>& sourceRects,
		const Recti* clipRect = NULL,
		const vector<Colour>* colors = NULL,
		bool useAlphaChannelOfTexture = false);

	void              testGLErrorBegan();
	bool              testGLErrorEnded(const wstring& errLog = L"");
	ERenderSystemType getRenderSystemType() { return ERST_OPENGL; }
	unsigned int      getMaximalPrimitiveCount() const { return 0x7fffffff; }
	bool              setActiveTexture(int stage, Texture* texture);
	bool              disableTextures(int fromStage = 0);

	int addShaderMaterial(const char* vertexShaderProgram = "",
		const char* pixelShaderProgram = "",
		ShaderConstantSetCallback* callback = NULL,
		EMaterialType baseMaterial = EMT_SOLID);

	void setBasicRenderStates(const Material& material, const Material& lastMaterial, bool resetAllRenderStates);
	void fillingDisplayList(bool isFilling) { _isFillDisplayList = isFilling; }

protected:
	void drawVertexPrimitiveList(const void* vertices, unsigned int vertexCount,
		const unsigned int* indexList, unsigned int primitiveCount, EPrimitiveType primType);

	typedef struct tagHardwareBufferLink_OpenGL : public HardwareBufferLink
	{
		GLuint VBO_verticesID;
		GLuint EBO_indicesID;
		GLuint VBO_verticesSize;
		GLuint EBO_indicesSize;

		tagHardwareBufferLink_OpenGL(SubMesh* meshBuffer)
			: HardwareBufferLink(meshBuffer)
			, VBO_verticesID(0)
			, EBO_indicesID(0)
			, VBO_verticesSize(0)
			, EBO_indicesSize(0) {}
	} HardwareBufferLink_OpenGL;

	HardwareBufferLink* createHardwareBuffer(SubMesh* mb);
	bool updateHardwareBuffer(HardwareBufferLink* hwBuffer);
	void drawHardwareBuffer(HardwareBufferLink* hwBuffer);
	HardwareBufferLinkItor deleteHardwareBuffer(HardwareBufferLink* hwBuffer);

private:
	void  setRenderMode3D();
	void  setRenderMode2D(bool alpha, bool texture, bool alphaChannel);
	void  setWrapMode(const Material& material);
	GLint getTextureWrapMode(unsigned char clamp);
	void  getGLTextureMatrix(GLfloat gl_matrix[16], const Matrix4x4& mat);
	void  gainColorBuffer(const void* vertices, unsigned int vertexCount);
	void  renderArray(const void* indexList, unsigned int primitiveCount, EPrimitiveType primType);
	void  createMaterialRenderers();
	bool  updateVertexHardwareBuffer(HardwareBufferLink_OpenGL* hwBuffer);
	bool  updateIndexHardwareBuffer(HardwareBufferLink_OpenGL* hwBuffer);

private:
	enum ERenderMode
	{
		ERM_NONE,
		ERM_2D,
		ERM_3D
	};
	ERenderMode           _currentRenderMode;
	Matrix4x4             _matrizes[ETS_COUNT];
	vector<unsigned char> _colorBuffer;
	Material              _curMaterial;
	Material              _lastMaterial;
	int                   _lastSetLight;
	int                   _maxTextureUnits;
	float                 _maxTextureLODBias;
	Dimension2di          _currentRenderTargetSize;
	bool                  _resetRenderStates;
	bool                  _transformation3DChanged;
	bool                  _isFillDisplayList;
	Texture*              _texturesUnitSet[MATERIAL_MAX_TEXTURES];

protected:
	PROPERTY_R(HDC,   _hCurrentDC,     CurrentDC)
	PROPERTY_R(HGLRC, _hCurrentRC,     CurrentRC)
	PROPERTY_R(int,   _pixelFormat,    PixelFormat)
	PROPERTY_R(GLint, _maxTextureSize, MaxTextureSize)
};

NS_VAYO_END

#endif // __VAYO_GL_RENDER_SYSTEM_H__
