#include "Vayo3dEntity.h"
#include "VayoUtils.h"
#include "Vayo3dRoot.h"
#include "VayoLog.h"
#include "Vayo3dSceneNode.h"
#include "Vayo3dRenderSystem.h"
#include "Vayo3dMeshManager.h"
#include "Vayo3dSceneManager.h"
#include "Vayo3dMaterialManager.h"
#include "Vayo3dManualObject.h"

NS_VAYO3D_BEGIN

Entity* SubEntity::getParent() const
{
	return _parent;
}

SubEntity::SubEntity(Entity* parent, SubMesh* addedSubMesh)
	: _parent(parent)
	, _subMesh(addedSubMesh)
{
}

SubEntity::~SubEntity()
{
}

void SubEntity::render()
{
	if (_subMesh->getVertexCount() != 0)
	{
		RenderSystem* pRenderSys = Root::getSingleton().getActiveRenderer();
		pRenderSys->setMaterial(*_material);
		pRenderSys->drawMeshBuffer(_subMesh);
	}
}

void SubEntity::getWorldTransform(Matrix4x4& mat) const
{
	if (_parent && _parent->getParentNode())
		mat = _parent->getParentNode()->getAbsTransformation();
	else
		Renderable::getWorldTransform(mat);
}

SubMesh* SubEntity::getSubMesh() const
{
	return _subMesh;
}

//////////////////////////////////////////////////////////////////////////
Reflex<Entity, const wstring&, SceneManager*> Entity::_dynReflex;
Entity::Entity(const wstring& name, SceneManager* originSceneMgr)
	: MovableObject(name, originSceneMgr)
	, _changedMeshID(0)
{
}

Entity::~Entity()
{
	vector<SubEntity*>::iterator it = _subEntities.begin();
	for (; it != _subEntities.end(); ++it)
	{
		delete (*it);
	}
	_subEntities.clear();
}

void Entity::update(float dt)
{
	buildSubEntities();
	vector<SubEntity*>::iterator it = _subEntities.begin();
	for (; it != _subEntities.end(); ++it)
	{
		Root::getSingleton().getCurSceneMgr()->registerForRendering(*it, getQueueID());
	}
}

void Entity::setMesh(MeshPtr pMesh)
{
	_mesh = pMesh;
	_changedMeshID = 0;
}

MeshPtr Entity::getMesh() const
{
	return _mesh;
}

SubEntity* Entity::getSubEntity(unsigned int index)
{
	buildSubEntities();
	if (index >= _subEntities.size())
	{
		Log::wprint(ELL_ERROR, L"Index out of bounds(Entity::getSubEntity).");
		return NULL;
	}
	return _subEntities[index];
}

SubEntity* Entity::getSubEntity(const wstring& name)
{
	unsigned int index = _mesh->getSubMeshIndex(name);
	return getSubEntity(index);
}

unsigned int Entity::getSubEntCount(void)
{
	buildSubEntities();
	return _subEntities.size();
}

void Entity::buildSubEntities()
{
	if (!_mesh || 0 == _mesh->getChangedID() || _mesh->getChangedID() != _changedMeshID)
	{
		vector<SubEntity*>::iterator it = _subEntities.begin();
		for (; it != _subEntities.end(); ++it)
			delete (*it);

		_subEntities.clear();

		if (!_mesh || 0 == _mesh->getChangedID())
			return;

		const vector<SubMesh*>& subMeshs = _mesh->getSubMeshList();
		vector<SubMesh*>::const_iterator cit = subMeshs.cbegin();
		for (; cit != subMeshs.cend(); ++cit)
		{
			SubMesh* pSubMesh = *cit;
			SubEntity* pSubEntity = new SubEntity(this, pSubMesh);
			if (pSubEntity)
			{
				pSubEntity->setMaterial(pSubMesh->getMaterialName());
				_subEntities.push_back(pSubEntity);
			}
		}

		_changedMeshID = _mesh->getChangedID();
	}
}

void Entity::serialize(XMLElement* outXml)
{
	MovableObject::serialize(outXml);
	if (_customAttribute != L"")
		outXml->SetAttribute("model", unicodeToUtf8(_customAttribute).c_str());
}

bool Entity::deserialize(XMLElement* inXml)
{
	if (!MovableObject::deserialize(inXml))
		return false;

	const char* pszTmp = inXml->Attribute("model");
	if (pszTmp && strcmp(pszTmp, ""))
		_customAttribute = utf8ToUnicode(pszTmp);

	return true;
}

bool Entity::parseCustomAttrib()
{
	if (_customAttribute == L"")
		return true;

	vector<wstring> container;
	wstringtok(container, _customAttribute, L",");
	int size = container.size();
	if (size > 0)
	{
		if (container[0] == L"Cube")
		{
			if (size >= 5)
			{
				Vector3df size = Vector3df((float)_wtof(container[2].c_str()),
					(float)_wtof(container[3].c_str()),
					(float)_wtof(container[4].c_str()));
				setMesh(Root::getSingleton().getMeshManager()->createCubeMesh(size, container[1]));
			}
			else
				Log::wprint(ELL_WARNING, L"Entity:Cube参数不足");
		}
		else if (container[0] == L"Sphere")
		{
			if (size >= 5)
			{
				setMesh(Root::getSingleton().getMeshManager()->createSphereMesh(
					(float)_wtof(container[2].c_str()),
					(unsigned)_wtoi(container[3].c_str()),
					(unsigned)_wtoi(container[4].c_str()),
					container[1]));
			}
			else
				Log::wprint(ELL_WARNING, L"Entity:Sphere参数不足");
		}
		else if (container[0] == L"Cylinder")
		{
			if (size >= 8)
			{
				bool closeTop = (container[6] == L"false" ? false : true);
				setMesh(Root::getSingleton().getMeshManager()->createCylinderMesh(
					(float)_wtof(container[2].c_str()),
					(float)_wtof(container[3].c_str()),
					(unsigned)_wtoi(container[4].c_str()),
					(unsigned int)strtoul(w2a_(container[5]).c_str(), NULL, 16),
					closeTop,
					(float)_wtof(container[7].c_str()),
					container[1]));
			}
			else
				Log::wprint(ELL_WARNING, L"Entity:Cylinder参数不足");
		}
		else if (container[0] == L"Cone")
		{
			if (size >= 8)
			{
				setMesh(Root::getSingleton().getMeshManager()->createConeMesh(
					(float)_wtof(container[2].c_str()),
					(float)_wtof(container[3].c_str()),
					(unsigned)_wtoi(container[4].c_str()),
					(unsigned int)strtoul(w2a_(container[5]).c_str(), NULL, 16),
					(unsigned int)strtoul(w2a_(container[6]).c_str(), NULL, 16),
					(float)_wtof(container[7].c_str()),
					container[1]));
			}
			else
				Log::wprint(ELL_WARNING, L"Entity:Cone参数不足");
		}
		else
		{
			ManualObject* pObj = Root::getSingleton().getCurSceneMgr()->findObject<ManualObject>(container[0]);
			if (pObj)
			{
				EHardwareMapping mappingHint = EHM_NEVER;
				if (size >= 2)
					mappingHint = (EHardwareMapping)_wtoi(container[1].c_str());
				setMesh(pObj->convertToMesh(mappingHint));
			}
			else
				Log::wprint(ELL_WARNING, L"模型[%s]在创建实体[%s]时不存在", container[0].c_str(), getName());
		}
	}

	return true;
}

NS_VAYO3D_END
