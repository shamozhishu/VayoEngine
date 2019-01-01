/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 渲染系统
\*************************************************************************/
#ifndef __VAYO_RENDER_SYSTEM_H__
#define __VAYO_RENDER_SYSTEM_H__

#include "VayoSupport.h"
#include "VayoDevice.h"
#include "VayoColour.h"
#include "VayoVertex.h"
#include "VayoVector2d.h"
#include "VayoRectangle.h"
#include "VayoLightData.h"
#include "VayoTexture.h"
#include "VayoMaterial.h"
#include "VayoMatrix4x4.h"
#include "VayoTriangle3d.h"
#include "VayoMaterialRenderer.h"

NS_VAYO_BEGIN

enum ETransformationState
{
	ETS_VIEW = 0,
	ETS_WORLD,
	ETS_PROJECTION,
	ETS_TEXTURE_0,
	ETS_TEXTURE_1,
	ETS_TEXTURE_2,
	ETS_TEXTURE_3,
#if VAYO_MATERIAL_MAX_TEXTURES > 4
	ETS_TEXTURE_4,
#if VAYO_MATERIAL_MAX_TEXTURES > 5
	ETS_TEXTURE_5,
#if VAYO_MATERIAL_MAX_TEXTURES > 6
	ETS_TEXTURE_6,
#if VAYO_MATERIAL_MAX_TEXTURES > 7
	ETS_TEXTURE_7,
#endif
#endif
#endif
#endif
	ETS_COUNT
};

class _VayoExport RenderSystem
{
public:
	RenderSystem(const wstring& name);
	virtual ~RenderSystem();

	virtual Device*      createDevice(int deviceID, const Device::Attrib& attrib) = 0;
	virtual TexturePtr   createTexture(const wstring& name, Image* image, bool generateMipLevels) = 0;

	virtual DisplayList* createDisplayList(const wstring& name = L"") = 0;
	virtual DisplayList* findDisplayList(const wstring& name);
	virtual void         destroyDisplayList(const wstring& name);
	virtual void         destroyAllDisplayList();

	virtual Tesselator*  createTesselator(const wstring& name = L"") = 0;
	virtual Tesselator*  findTesselator(const wstring& name);
	virtual void         destroyTesselator(const wstring& name);
	virtual void         destroyAllTesselators();

	virtual bool init(unsigned char antiAliasFactor = 0, bool handleSRGB = false) = 0;
	virtual bool isActive() const = 0;
	virtual void restoreContext() const = 0;
	virtual bool beginScene(bool backBuffer, bool zBuffer, bool stencilBuffer, Device* renderWnd);
	virtual bool endScene();
	virtual const Matrix4x4& getTransform(ETransformationState state) const = 0;
	virtual void setTransform(ETransformationState state, const Matrix4x4& mat) = 0;
	virtual const Dimension2di& getCurRenderTargetSize() const = 0;
	virtual void setViewpot(const Recti& area) = 0;
	virtual void setAmbientLight(const Colour& color) = 0;
	virtual void setMaterial(const Material& material) = 0;
	virtual void addDynamicLight(const LightData& light);
	virtual void removeAllDynamicLights();

	virtual void drawDisplayList(DisplayList* displayList) = 0;
	virtual void drawPixel(int x, int y, const Colour& color) = 0;
	virtual void draw2DLine(const Position2di& start, const Position2di& end, Colour color=Colour(255,255,255,255)) = 0;
	virtual void draw2DRect(Colour color, const Recti& rc, const Recti* clip = NULL);
	virtual void draw2DRect(const Recti& rc, Colour colorLeftUp, Colour colorRightUp, Colour colorLeftDown, Colour colorRightDown, const Recti* clip = NULL) = 0;
	virtual void draw2DRectOutline(const Recti& rc, Colour color=Colour(255,255,255,255));
	virtual void draw3DLine(const Vector3df& start, const Vector3df& end, Colour color = Colour(255, 255, 255, 255)) = 0;
	virtual void draw3DTriangle(const Triangle3df& triangle, Colour color = Colour(255, 255, 255, 255));
	virtual void draw3DBox(const Aabbox3df box, Colour color = Colour(255, 255, 255, 255));

	virtual void draw2DImage(TexturePtr texture, const Position2di& destPos,
		const Recti& sourceRect, const Recti* clipRect = NULL,
		Colour color = Colour(255, 255, 255, 255), bool useAlphaChannelOfTexture = false) = 0;

	virtual void draw2DImage(TexturePtr texture, const Recti& destRect,
		const Recti& sourceRect, const Recti* clipRect = NULL,
		const Colour* const colors = NULL, bool useAlphaChannelOfTexture = false) = 0;

	virtual void draw2DImageBatch(TexturePtr texture,
		const vector<Position2di>& positions,
		const vector<Recti>& sourceRects,
		const Recti* clipRect = NULL,
		Colour color = Colour(255, 255, 255, 255),
		bool useAlphaChannelOfTexture = false) = 0;

	virtual void draw2DImageBatch(TexturePtr texture,
		const vector<Recti>& destRects,
		const vector<Recti>& sourceRects,
		const Recti* clipRect = NULL,
		const vector<Colour>* colors = NULL,
		bool useAlphaChannelOfTexture = false) = 0;

	virtual void drawMeshBuffer(SubMesh* mb);
	virtual void updateAllHardwareBuffers();
	virtual void removeHardwareBuffer(SubMesh* mb);
	virtual void removeAllHardwareBuffers();
	virtual bool isHardwareBufferRecommend(SubMesh* mb);

	virtual EColorFormat getColorBufferFormat() const = 0;
	virtual ERenderSystemType getRenderSystemType() const { return ERST_NULL; }
	virtual unsigned int getMaximalPrimitiveCount() const { return 0xFFFFFFFF; }
	inline unsigned int getPrimTotalNum() const { return _primitivesDrawn; }

	virtual int addShaderMaterial(const char* vertexShaderProgram = "",
		const char* pixelShaderProgram = "",
		ShaderConstantSetCallback* callback = NULL,
		EMaterialType baseMaterial = EMT_SOLID) = 0;
	virtual int addShaderMaterialFromFiles(const wstring& vertexShaderProgramFileName = L"",
		const wstring& pixelShaderProgramFileName = L"",
		ShaderConstantSetCallback* callback = NULL,
		EMaterialType baseMaterial = EMT_SOLID);

	MaterialRenderer* getMaterialRenderer(int idx) const;

protected:
	bool checkPrimitiveCount(unsigned int primCnt) const;
	virtual void drawVertexPrimitiveList(const void* vertices, unsigned int vertexCount,
		const unsigned int* indexList, unsigned int primitiveCount, EPrimitiveType primType) {
		_primitivesDrawn += primitiveCount;
	}

	typedef struct tagHardwareBufferLink
	{
		SubMesh*     MeshBuffer;
		unsigned int ChangedVertexID;
		unsigned int ChangedIndexID;
		unsigned int LastUsed;
		EHardwareMapping MappedVertex;
		EHardwareMapping MappedIndex;

		tagHardwareBufferLink(SubMesh* meshBuffer)
			: MeshBuffer(meshBuffer)
			, ChangedVertexID(0)
			, ChangedIndexID(0)
			, LastUsed(0)
			, MappedVertex(EHM_NEVER)
			, MappedIndex(EHM_NEVER) {}
	} HardwareBufferLink;

	typedef map<SubMesh*, HardwareBufferLink*>::iterator HardwareBufferLinkItor;

	virtual HardwareBufferLink* getBufferLink(SubMesh* mb);
	virtual HardwareBufferLink* createHardwareBuffer(SubMesh* mb) { return NULL; }
	virtual bool updateHardwareBuffer(HardwareBufferLink* hwBuffer) { return false; }
	virtual void drawHardwareBuffer(HardwareBufferLink* hwBuffer) {}
	virtual HardwareBufferLinkItor deleteHardwareBuffer(HardwareBufferLink* hwBuffer);

protected:
	unsigned int                       _primitivesDrawn;
	unsigned int                       _minVertCntForHwbuffer;
	vector<LightData>                  _lightDataset;
	vector<MaterialRendererPtr>        _materialRenderers;
	map<wstring, DisplayList*>         _displayListSet;
	map<wstring, Tesselator*>          _tesselators;
	map<SubMesh*, HardwareBufferLink*> _hardwareBufferMap;

protected:
	PROPERTY_R_REF(wstring, _name,     Name)
	PROPERTY_R_REF(Recti,   _viewPort, ViewPort)
};

NS_VAYO_END

#endif // __VAYO_RENDER_SYSTEM_H__
