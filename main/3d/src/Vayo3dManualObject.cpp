#include "Vayo3dManualObject.h"
#include "Vayo3dRoot.h"
#include "Vayo3dSceneNode.h"
#include "Vayo3dRenderSystem.h"
#include "Vayo3dMaterialManager.h"
#include "Vayo3dMeshManager.h"
#include "Vayo3dSceneManager.h"
#include "VayoUtils.h"
#include "VayoLog.h"
#include "Vayo3dDisplayList.h"

NS_VAYO3D_BEGIN

Reflex<ManualObject, const wstring&, SceneManager*> ManualObject::_dynReflex;
ManualObject::ManualObject(const wstring& name, SceneManager* oriSceneMgr)
	: MovableObject(name, oriSceneMgr)
	, _needSubmit(true)
	, _opSubMesh(NULL)
	, _displayList(NULL)
	, _lastVertNum(0)
	, _lastIdxNum(0)
{
	_meshData = Root::getSingleton().getMeshManager()->createEmptyMesh();
	_displayList = Root::getSingleton().getActiveRenderer()->createDisplayList();
}

ManualObject::~ManualObject()
{
	Root::getSingleton().getMeshManager()->destroyMesh(_meshData);
	Root::getSingleton().getActiveRenderer()->destroyDisplayList(_displayList->getName());
}

void ManualObject::update(float dt)
{
	Root::getSingleton().getCurSceneMgr()->registerForRendering(this, getQueueID());
}

void ManualObject::render()
{
	if (!_meshData->isEmptyMesh())
	{
		// 设置所有子网格的公共材质,例如启用光照,因为类似glEnable(GL_LIGHTING)这类调用不能装入显示列表中.
		Root::getSingleton().getActiveRenderer()->setMaterial(*getMaterial());
		if (_needSubmit)
		{
			submitDisplay();
			_needSubmit = false;
		}
		Root::getSingleton().getActiveRenderer()->drawDisplayList(_displayList);
	}
}

void ManualObject::setMaterial(const wstring& name)
{
	if (getMaterial()->_materialName != name)
		_needSubmit = true;
	Renderable::setMaterial(name);
}

void ManualObject::setMaterial(MaterialPtr material)
{
	if (getMaterial().get() != material.get())
		_needSubmit = true;
	Renderable::setMaterial(material);
}

void ManualObject::getWorldTransform(Matrix4x4& mat) const
{
	SceneNode* sn = getParentNode();
	if (sn)
		mat = sn->getAbsTransform();
	else
		Renderable::getWorldTransform(mat);
}

void ManualObject::begin(EPrimitiveType primType, const wstring& materialName /*= L""*/)
{
	VAYO_ASSERT(!_opSubMesh);

	_opSubMesh = _meshData->createSubMesh(primType);
	_opSubMesh->setMaterialName(materialName);
	_lastVertNum = _opSubMesh->getVertexCount();
	_lastIdxNum = _opSubMesh->getIndexCount();
}

void ManualObject::beginUpdate(unsigned int idx)
{
	VAYO_ASSERT(!_opSubMesh);

	if (idx >= _meshData->getSubMeshCount())
	{
		Log::wprint(ELL_ERROR, L"Index out of bounds(ManualObject::beginUpdate).");
		return;
	}

	_opSubMesh = _meshData->getSubMesh(idx);
	_opSubMesh->clearVertexList();
	_opSubMesh->clearIndexList();
	_lastVertNum = _opSubMesh->getVertexCount();
	_lastIdxNum = _opSubMesh->getIndexCount();
}

void ManualObject::end()
{
	VAYO_ASSERT(_opSubMesh && "ManualObject::_opSubMesh == NULL");
	_opSubMesh->computeVertexNormals();

	if (_lastVertNum != _opSubMesh->getVertexCount())
	{
		_opSubMesh->setDirty(EBT_VERTEX);
		_needSubmit = true;
	}

	if (_lastIdxNum != _opSubMesh->getIndexCount())
	{
		_opSubMesh->setDirty(EBT_INDEX);
		_needSubmit = true;
	}

	_opSubMesh = NULL;
}

void ManualObject::position(const Vector3df& pos)
{
	position(pos._x, pos._y, pos._z);
}

void ManualObject::position(float x, float y, float z)
{
	VAYO_ASSERT(_opSubMesh && "ManualObject::_opSubMesh == NULL");
	_opSubMesh->addIndex(_opSubMesh->getVertexCount());
	_opSubMesh->addPosition(Vector3df(x, y, z));
}

void ManualObject::normal(const Vector3df& norm)
{
	normal(norm._x, norm._y, norm._z);
}

void ManualObject::normal(float x, float y, float z)
{
	VAYO_ASSERT(_opSubMesh && "ManualObject::_opSubMesh == NULL");
	_opSubMesh->addNormal(Vector3df(x, y, z));
}

void ManualObject::textureCoord(const Vector2df& uv)
{
	textureCoord(uv._x, uv._y);
}

void ManualObject::textureCoord(float u, float v)
{
	VAYO_ASSERT(_opSubMesh && "ManualObject::_opSubMesh == NULL");
	_opSubMesh->addTexCoord(Vector2df(u, v));
}

void ManualObject::colour(const Colour& col)
{
	colour(col.getRed(), col.getGreen(), col.getBlue(), col.getAlpha());
}

void ManualObject::colour(int r, int g, int b, int a /*= 255*/)
{
	VAYO_ASSERT(_opSubMesh && "ManualObject::_opSubMesh == NULL");
	_opSubMesh->addColor(Colour(a, r, g, b));
}

void ManualObject::index(unsigned int idx)
{
	VAYO_ASSERT(_opSubMesh && "ManualObject::_opSubMesh == NULL");
	_opSubMesh->addIndex(idx);
}

void ManualObject::triangle(unsigned int i1, unsigned int i2, unsigned int i3)
{
	VAYO_ASSERT(_opSubMesh && "ManualObject::_opSubMesh == NULL");
	_opSubMesh->addIndex(i1);
	_opSubMesh->addIndex(i2);
	_opSubMesh->addIndex(i3);
}

void ManualObject::quad(unsigned int i1, unsigned int i2, unsigned int i3, unsigned int i4)
{
	VAYO_ASSERT(_opSubMesh && "ManualObject::_opSubMesh == NULL");
	_opSubMesh->addIndex(i1);
	_opSubMesh->addIndex(i2);
	_opSubMesh->addIndex(i3);
	_opSubMesh->addIndex(i4);
}

MeshPtr ManualObject::convertToMesh(EHardwareMapping mappingHint /*= EHM_NEVER*/) const
{
	SubMesh* pCurSubMesh = NULL;
	const vector<SubMesh*>& subMeshList = _meshData->getSubMeshList();
	vector<SubMesh*>::const_iterator cit = subMeshList.cbegin();
	for (; cit != subMeshList.cend(); ++cit)
	{
		pCurSubMesh = (*cit);
		if (pCurSubMesh)
		{
			pCurSubMesh->setHardwareMappingHint(mappingHint);
			if (pCurSubMesh->getMaterialName() == L"" && getMaterial()->_materialName != L"")
				pCurSubMesh->setMaterialName(getMaterial()->_materialName);
		}
	}
	return _meshData;
}

MeshPtr ManualObject::getMesh() const
{
	return _meshData;
}

SubMesh* ManualObject::getOpSubMesh() const
{
	return _opSubMesh;
}

void ManualObject::resetSubmit()
{
	_needSubmit = true;
}

void ManualObject::generatePlane(float extent, float step, const wstring& materialName)
{
	float s = 0.0f;
	float t = 0.0f;
	float texStep = 1.0f / (extent * 0.075f);
	for (int i = -extent; i < extent; i += step)
	{
		t = 0.0f;
		begin(EPT_TRIANGLE_STRIP, materialName);
		for (int j = extent; j >= -extent; j -= step)
		{
			textureCoord(s, t);
			normal(0.0f, 1.0f, 0.0f);
			position(i, 0, j);

			textureCoord(s + texStep, t);
			normal(0.0f, 1.0f, 0.0f);
			position(i + step, 0, j);

			t += texStep;
		}
		end();
		s += texStep;
	}
}

void ManualObject::generateSphere(float fRadius, int iSlices, int iStacks, const wstring& materialName)
{
	float drho = (float)(3.141592653589) / (float)iStacks;
	float dtheta = 2.0f * (float)(3.141592653589) / (float)iSlices;
	float ds = 1.0f / (float)iSlices;
	float dt = 1.0f / (float)iStacks;
	float t = 1.0f;
	float s = 0.0f;
	int i, j;
	for (i = 0; i < iStacks; i++)
	{
		float rho = (float)i * drho;
		float srho = (float)(sin(rho));
		float crho = (float)(cos(rho));
		float srhodrho = (float)(sin(rho + drho));
		float crhodrho = (float)(cos(rho + drho));
		s = 0.0f;
		begin(EPT_TRIANGLE_STRIP, materialName);
		for (j = 0; j <= iSlices; j++)
		{
			float theta = (j == iSlices) ? 0.0f : j * dtheta;
			float stheta = (float)(-sin(theta));
			float ctheta = (float)(cos(theta));

			float x = stheta * srho;
			float y = ctheta * srho;
			float z = crho;

			textureCoord(s, t);
			normal(x, y, z);
			position(x * fRadius, y * fRadius, z * fRadius);

			x = stheta * srhodrho;
			y = ctheta * srhodrho;
			z = crhodrho;
			textureCoord(s, t - dt);
			s += ds;
			normal(x, y, z);
			position(x * fRadius, y * fRadius, z * fRadius);
		}
		end();
		t -= dt;
	}
}

void ManualObject::generateTorus(float majorRadius, float minorRadius, int numMajor, int numMinor, const wstring& materialName)
{
	float vNormal[3];
	double majorStep = 2.0f*VAYO_MATH_PI / numMajor;
	double minorStep = 2.0f*VAYO_MATH_PI / numMinor;
	int i, j;
	for (i = 0; i < numMajor; ++i)
	{
		double a0 = i * majorStep;
		double a1 = a0 + majorStep;
		float x0 = (float)cos(a0);
		float y0 = (float)sin(a0);
		float x1 = (float)cos(a1);
		float y1 = (float)sin(a1);
		begin(EPT_TRIANGLE_STRIP, materialName);
		for (j = 0; j <= numMinor; ++j)
		{
			double b = j * minorStep;
			float c = (float)cos(b);
			float r = minorRadius * c + majorRadius;
			float z = minorRadius * (float)sin(b);

			textureCoord((float)(i) / (float)(numMajor), (float)(j) / (float)(numMinor));
			vNormal[0] = x0*c;
			vNormal[1] = y0*c;
			vNormal[2] = z / minorRadius;
			float mag = sqrtf(vNormal[0] * vNormal[0] + vNormal[1] * vNormal[1] + vNormal[2] * vNormal[2]);
			vNormal[0] /= mag;
			vNormal[1] /= mag;
			vNormal[2] /= mag;
			normal(vNormal[0], vNormal[1], vNormal[2]);
			position(x0*r, y0*r, z);

			textureCoord((float)(i + 1) / (float)(numMajor), (float)(j) / (float)(numMinor));
			vNormal[0] = x1*c;
			vNormal[1] = y1*c;
			vNormal[2] = z / minorRadius;
			mag = sqrtf(vNormal[0] * vNormal[0] + vNormal[1] * vNormal[1] + vNormal[2] * vNormal[2]);
			vNormal[0] /= mag;
			vNormal[1] /= mag;
			vNormal[2] /= mag;
			normal(vNormal[0], vNormal[1], vNormal[2]);
			position(x1*r, y1*r, z);
		}
		end();
	}
}

void ManualObject::submitDisplay()
{
	if (!_meshData->isEmptyMesh())
	{
		SubMesh* pSubMesh = NULL;
		wstring subMeshMaterialName;
		RenderSystem* pRenderSys = Root::getSingleton().getActiveRenderer();
		MaterialManager* pMaterialMgr = Root::getSingleton().getMaterialManager();
		const vector<SubMesh*>& subMeshList = _meshData->getSubMeshList();
		vector<SubMesh*>::const_iterator cit = subMeshList.cbegin();
		_displayList->newList();

		for (; cit != subMeshList.cend(); ++cit)
		{
			pSubMesh = *cit;
			subMeshMaterialName = pSubMesh->getMaterialName();

			if (subMeshMaterialName != L"")
				pRenderSys->setMaterial(*pMaterialMgr->findMaterial(subMeshMaterialName));
			else
				pRenderSys->setMaterial(*getMaterial());

			EHardwareMapping oldVertHwmapping = pSubMesh->getMappedVertexHint();
			EHardwareMapping oldIdxHwmapping = pSubMesh->getMappedIndexHint();
			pSubMesh->setHardwareMappingHint(EHM_NEVER);
			pRenderSys->drawMeshBuffer(pSubMesh);
			pSubMesh->setHardwareMappingHint(oldVertHwmapping, EBT_VERTEX);
			pSubMesh->setHardwareMappingHint(oldIdxHwmapping, EBT_INDEX);
		}

		wstring lastMaterialName;
		if (pSubMesh)
			lastMaterialName = pSubMesh->getMaterialName();
		if (lastMaterialName == L"")
			lastMaterialName = getMaterial()->_materialName;
		_displayList->endList(lastMaterialName);
	}
}

void ManualObject::serialize(XMLElement* outXml)
{
	MovableObject::serialize(outXml);
	if (_customAttribute != L"")
		outXml->SetAttribute("model", unicodeToUtf8(_customAttribute).c_str());
}

bool ManualObject::deserialize(XMLElement* inXml)
{
	if (!MovableObject::deserialize(inXml))
		return false;

	const char* pszTmp = inXml->Attribute("model");
	if (pszTmp && strcmp(pszTmp, ""))
		_customAttribute = utf8ToUnicode(pszTmp);

	return true;
}

bool ManualObject::parseCustomAttrib()
{
	if (_customAttribute == L"")
		return true;

	vector<wstring> container;
	wstringtok(container, _customAttribute, L",");
	int size = container.size();
	if (size > 0)
	{
		if (container[0] == L"Plane")
		{
			if (size >= 4)
				generatePlane((float)_wtof(container[2].c_str()), (float)_wtof(container[3].c_str()), container[1]);
			else
				Log::wprint(ELL_WARNING, L"ManualObject::generatePlane()参数不足");
		}
		else if (container[0] == L"Torus")
		{
			if (size >= 6)
				generateTorus((float)_wtof(container[2].c_str()), (float)_wtof(container[3].c_str()),
				(float)_wtof(container[4].c_str()), (float)_wtof(container[5].c_str()), container[1]);
			else
				Log::wprint(ELL_WARNING, L"ManualObject::generateTorus()参数不足");
		}
		else if (container[0] == L"Sphere")
		{
			if (size >= 5)
				generateSphere((float)_wtof(container[2].c_str()), (float)_wtof(container[3].c_str()),
				(float)_wtof(container[4].c_str()), container[1]);
			else
				Log::wprint(ELL_WARNING, L"ManualObject::generateSphere()参数不足");
		}
	}

	return true;
}

NS_VAYO3D_END
