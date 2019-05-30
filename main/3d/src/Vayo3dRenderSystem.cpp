#include "Vayo3dRenderSystem.h"
#include "Vayo3dDisplayList.h"
#include "Vayo3dTesselator.h"
#include "VayoLog.h"
#include "Vayo3dRoot.h"
#include "VayoDevice.h"
#include "Vayo3dImage.h"
#include "VayoFileIO.h"
#include "Vayo3dMesh.h"

NS_VAYO3D_BEGIN

void RenderSystem::addDynamicLight(const LightData& light)
{
	_lightDataset.push_back(light);
}

void RenderSystem::removeAllDynamicLights()
{
	_lightDataset.clear();
}

RenderSystem::RenderSystem(const wstring& name)
	: _name(name)
	, _primitivesDrawn(0)
	, _minVertCntForHwbuffer(500)
{
}

RenderSystem::~RenderSystem()
{
}

bool RenderSystem::beginScene(bool backBuffer, bool zBuffer, bool stencilBuffer, Device* renderWnd)
{
	if (NULL == renderWnd)
		return false;
	_primitivesDrawn = 0;
	return true;
}

bool RenderSystem::endScene()
{
	updateAllHardwareBuffers();
	return true;
}

void RenderSystem::draw2DRect(Colour color, const Recti& rc, const Recti* clip /*= NULL*/)
{
	draw2DRect(rc, color, color, color, color, clip);
}

void RenderSystem::draw2DRectOutline(const Recti& rc, Colour color/*=Colour(255,255,255,255)*/)
{
	draw2DLine(rc._upperLeftCorner, Position2di(rc._lowerRightCorner._x, rc._upperLeftCorner._y), color);
	draw2DLine(Position2di(rc._lowerRightCorner._x, rc._upperLeftCorner._y), rc._lowerRightCorner, color);
	draw2DLine(rc._lowerRightCorner, Position2di(rc._upperLeftCorner._x, rc._lowerRightCorner._y), color);
	draw2DLine(Position2di(rc._upperLeftCorner._x, rc._lowerRightCorner._y), rc._upperLeftCorner, color);
}

void RenderSystem::draw3DTriangle(const Triangle3df& triangle, Colour color /*= Colour(255, 255, 255, 255)*/)
{
	Vertex vertices[3];
	vertices[0]._position = triangle._pointA;
	vertices[0]._color = color;
	vertices[0]._normal = triangle.getNormal().normalize();
	vertices[0]._texCoord.set(0.f, 0.f);
	vertices[1]._position = triangle._pointB;
	vertices[1]._color = color;
	vertices[1]._normal = vertices[0]._normal;
	vertices[1]._texCoord.set(0.5f, 1.f);
	vertices[2]._position = triangle._pointC;
	vertices[2]._color = color;
	vertices[2]._normal = vertices[0]._normal;
	vertices[2]._texCoord.set(1.f, 0.f);
	unsigned int indexList[] = {0,1,2};
	drawVertexPrimitiveListBegan(vertices, 3);
	drawVertexPrimitiveList(indexList, 1, EPT_TRIANGLES);
	drawVertexPrimitiveListEnded();
}

void RenderSystem::draw3DBox(const Aabbox3df box, Colour color /*= Colour(255, 255, 255, 255)*/)
{
	Vector3df edges[8];
	box.getEdges(edges);
	draw3DLine(edges[5], edges[1], color);
	draw3DLine(edges[1], edges[3], color);
	draw3DLine(edges[3], edges[7], color);
	draw3DLine(edges[7], edges[5], color);
	draw3DLine(edges[0], edges[2], color);
	draw3DLine(edges[2], edges[6], color);
	draw3DLine(edges[6], edges[4], color);
	draw3DLine(edges[4], edges[0], color);
	draw3DLine(edges[1], edges[0], color);
	draw3DLine(edges[3], edges[2], color);
	draw3DLine(edges[7], edges[6], color);
	draw3DLine(edges[5], edges[4], color);
}

void RenderSystem::updateAllHardwareBuffers()
{
	map<SubMesh*, HardwareBufferLink*>::iterator it = _hardwareBufferMap.begin();
	for (; it != _hardwareBufferMap.end();)
	{
		HardwareBufferLink* link = it->second;
		link->LastUsed++;
		if (link->LastUsed > 20000)
			it = deleteHardwareBuffer(link);
		else
			++it;
	}
}

void RenderSystem::removeHardwareBuffer(SubMesh* mb)
{
	map<SubMesh*, HardwareBufferLink*>::iterator it = _hardwareBufferMap.find(mb);
	if (it != _hardwareBufferMap.end())
		deleteHardwareBuffer(it->second);
}

void RenderSystem::removeAllHardwareBuffers()
{
	map<SubMesh*, HardwareBufferLink*>::iterator it = _hardwareBufferMap.begin();
	for (; it != _hardwareBufferMap.end();)
	{
		it = deleteHardwareBuffer(it->second);
	}
	_hardwareBufferMap.clear();
}

bool RenderSystem::isHardwareBufferRecommend(SubMesh* mb)
{
	if (!mb || (mb->getMappedVertexHint() == EHM_NEVER && mb->getMappedIndexHint() == EHM_NEVER))
		return false;

	if (mb->getVertexCount() < _minVertCntForHwbuffer)
		return false;

	return true;
}

RenderSystem::HardwareBufferLink* RenderSystem::getBufferLink(SubMesh* mb)
{
	if (!mb || !isHardwareBufferRecommend(mb))
		return NULL;

	map<SubMesh*, HardwareBufferLink*>::iterator it = _hardwareBufferMap.find(mb);
	if (it != _hardwareBufferMap.end())
		return it->second;

	return createHardwareBuffer(mb);
}

RenderSystem::HardwareBufferLinkItor RenderSystem::deleteHardwareBuffer(HardwareBufferLink* hwBuffer)
{
	HardwareBufferLinkItor it = _hardwareBufferMap.end();
	if (hwBuffer)
	{
		it = _hardwareBufferMap.find(hwBuffer->MeshBuffer);
		if (it != _hardwareBufferMap.end())
			it = _hardwareBufferMap.erase(it);
		delete hwBuffer;
	}
	return it;
}

int RenderSystem::addShaderMaterialFromFiles(const wstring& vertexShaderProgramFileName /*= L""*/,
	const wstring& pixelShaderProgramFileName /*= L""*/, ShaderConstantSetCallback* callback /*= NULL*/,
	EMaterialType baseMaterial /*= EMT_SOLID*/)
{
	string vs;
	string ps;

	if (vertexShaderProgramFileName != L"")
	{
		ReadFile vsfile(vertexShaderProgramFileName);
		if (!vsfile.isOpen())
		{
			Log::wprint(ELL_WARNING, L"Could not open vertex shader program file(%s)", vertexShaderProgramFileName.c_str());
			return -1;
		}

		int size = vsfile.getSize();
		if (size > 0)
		{
			vs.resize(size);
			vsfile.read(&vs[0], size);
		}
	}

	if (pixelShaderProgramFileName != L"")
	{
		ReadFile psfile(pixelShaderProgramFileName);
		if (!psfile.isOpen())
		{
			Log::wprint(ELL_WARNING, L"Could not open pixel shader program file(%s)", pixelShaderProgramFileName.c_str());
			return -1;
		}

		int size = psfile.getSize();
		if (size > 0)
		{
			ps.resize(size);
			psfile.read(&ps[0], size);
		}
	}

	return addShaderMaterial(vs.c_str(), ps.c_str(), callback, baseMaterial);
}

MaterialRenderer* RenderSystem::getMaterialRenderer(int idx) const
{
	if (idx < 0 || idx >= (int)_materialRenderers.size())
		return NULL;
	return _materialRenderers[idx].get();
}

bool RenderSystem::checkPrimitiveCount(unsigned int primCnt) const
{
	const unsigned int m = getMaximalPrimitiveCount();
	if (primCnt > m)
	{
		Log::print(ELL_ERROR, "Could not draw, too many primitives(%u), maxium is %u.", primCnt, m);
		return false;
	}
	return true;
}

void RenderSystem::destroyDisplayList(DisplayList* pDisplayList)
{
	auto it = std::find(_displayListSet.begin(), _displayListSet.end(), pDisplayList);
	if (it != _displayListSet.end())
	{
		SAFE_DELETE(*it);
		_displayListSet.erase(it);
	}
}

void RenderSystem::destroyAllDisplayList()
{
	auto it = _displayListSet.begin();
	for (; it != _displayListSet.end(); ++it)
	{
		SAFE_DELETE(*it);
	}
	_displayListSet.clear();
}

void RenderSystem::destroyTesselator(Tesselator* pTesselator)
{
	auto it = std::find(_tesselators.begin(), _tesselators.end(), pTesselator);
	if (it != _tesselators.end())
	{
		SAFE_DELETE(*it);
		_tesselators.erase(it);
	}
}

void RenderSystem::destroyAllTesselators()
{
	auto it = _tesselators.begin();
	for (; it != _tesselators.end(); ++it)
	{
		SAFE_DELETE(*it);
	}
	_tesselators.clear();
}

NS_VAYO3D_END
