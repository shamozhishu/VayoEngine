#include "VayoEntity.h"
#include "VayoUtils.h"
#include "VayoRoot.h"
#include "VayoLog.h"
#include "VayoSceneNode.h"
#include "VayoRenderSystem.h"
#include "VayoMeshManager.h"
#include "VayoSceneManager.h"
#include "VayoMaterialManager.h"
#include "VayoManualObject.h"

NS_VAYO_BEGIN

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
Entity::Entity(const wstring& name)
	: MovableObject(name)
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

}

bool Entity::deserialize(XMLElement* inXml)
{
	if (!inXml)
		return false;

	const char* pszTmp = inXml->Attribute("model");
	if (NULL == pszTmp)
		return true;

	string strTemp = pszTmp;
	vector<string> container;
	stringtok(container, strTemp, ",");
	int size = container.size();
	if (size > 0)
	{
		if (container[0] == "Cube")
		{
			if (size >= 5)
			{
				wstring materialName = utf8ToUnicode(container[1]);
				Vector3df size = Vector3df((float)atof(container[2].c_str()),
					(float)atof(container[3].c_str()),
					(float)atof(container[4].c_str()));
				setMesh(Root::getSingleton().getMeshManager()->createCubeMesh(size, materialName));
			}
			else
				Log::wprint(ELL_WARNING, L"Entity:Cube参数不足");
		}
		else if (container[0] == "Sphere")
		{
			if (size >= 5)
			{
				wstring materialName = utf8ToUnicode(container[1]);
				setMesh(Root::getSingleton().getMeshManager()->createSphereMesh(
					(float)atof(container[2].c_str()),
					(unsigned)atoi(container[3].c_str()),
					(unsigned)atoi(container[4].c_str()),
					materialName));
			}
			else
				Log::wprint(ELL_WARNING, L"Entity:Sphere参数不足");
		}
		else if (container[0] == "Cylinder")
		{
			if (size >= 8)
			{
				bool closeTop = (container[6] == "false" ? false : true);
				wstring materialName = utf8ToUnicode(container[1]);
				setMesh(Root::getSingleton().getMeshManager()->createCylinderMesh(
					(float)atof(container[2].c_str()),
					(float)atof(container[3].c_str()),
					(unsigned)atoi(container[4].c_str()),
					(unsigned int)strtoul(container[5].c_str(), NULL, 16),
					closeTop,
					(float)atof(container[7].c_str()),
					materialName));
			}
			else
				Log::wprint(ELL_WARNING, L"Entity:Cylinder参数不足");
		}
		else if (container[0] == "Cone")
		{
			if (size >= 8)
			{
				wstring materialName = utf8ToUnicode(container[1]);
				setMesh(Root::getSingleton().getMeshManager()->createConeMesh(
					(float)atof(container[2].c_str()),
					(float)atof(container[3].c_str()),
					(unsigned)atoi(container[4].c_str()),
					(unsigned int)strtoul(container[5].c_str(), NULL, 16),
					(unsigned int)strtoul(container[6].c_str(), NULL, 16),
					(float)atof(container[7].c_str()),
					materialName));
			}
			else
				Log::wprint(ELL_WARNING, L"Entity:Cone参数不足");
		}
		else
		{
			wstring objName = utf8ToUnicode(container[0]);
			ManualObject* pObj = Root::getSingleton().getCurSceneMgr()->findObject<ManualObject>(objName);
			if (pObj)
			{
				EHardwareMapping mappingHint = EHM_NEVER;
				if (size >= 2)
					mappingHint = (EHardwareMapping)atoi(container[1].c_str());
				setMesh(pObj->convertToMesh(mappingHint));
			}
			else
				Log::wprint(ELL_WARNING, L"模型[%s]在创建实体[%s]时不存在", objName.c_str(), getName());
		}
	}

	return true;
}

NS_VAYO_END
