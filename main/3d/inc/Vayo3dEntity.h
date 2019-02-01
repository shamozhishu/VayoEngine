/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 实体
\*************************************************************************/
#ifndef __VAYO3D_ENTITY_H__
#define __VAYO3D_ENTITY_H__

#include "Vayo3dSupport.h"
#include "Vayo3dMesh.h"
#include "Vayo3dRenderable.h"
#include "Vayo3dMovableObject.h"
#include "math/VayoMatrix4x4.h"
NS_VAYO3D_BEGIN

class _Vayo3dExport SubEntity : public Renderable
{
	friend class Entity;
protected:
	SubEntity(Entity* parent, SubMesh* addedSubMesh);
	~SubEntity();

public:
	void               render();
	void               getWorldTransform(Matrix4x4& mat) const;
	Entity*            getParent() const;
	SubMesh*           getSubMesh() const;

private:
	Entity*     _parent;
	SubMesh*    _subMesh;
};

class _Vayo3dExport Entity : public MovableObject
{
	static Reflex<Entity, const wstring&, SceneManager*> _dynReflex;
public:
	Entity(const wstring& name, SceneManager* originSceneMgr);
	~Entity();
	void         update(float dt);
	void         setMesh(MeshPtr pMesh);
	MeshPtr      getMesh() const;
	SubEntity*   getSubEntity(unsigned int index);
	SubEntity*   getSubEntity(const wstring& name);
	unsigned int getSubEntCount(void);
	void         serialize(XMLElement* outXml);
	bool         deserialize(XMLElement* inXml);
	bool         parseCustomAttrib();

protected:
	void buildSubEntities();
private:
	MeshPtr            _mesh;
	unsigned int       _changedMeshID;
	vector<SubEntity*> _subEntities;
};

NS_VAYO3D_END

#endif // __VAYO3D_ENTITY_H__
