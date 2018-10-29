/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* 实体
\*************************************************************************/
#ifndef __VAYO_ENTITY_H__
#define __VAYO_ENTITY_H__

#include "VayoSupport.h"
#include "VayoMovableObject.h"
#include "VayoMesh.h"
#include "VayoRenderable.h"
#include "VayoMatrix4x4.h"
NS_VAYO_BEGIN

class _VayoExport SubEntity : public Renderable
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

class _VayoExport Entity : public MovableObject
{
	VAYO_REFLEX_WITHPARA_DECLARE(Entity, const wstring&)
public:
	Entity(const wstring& name);
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

NS_VAYO_END

#endif // __VAYO_ENTITY_H__
