/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 网格
\*************************************************************************/
#ifndef __VAYO3D_MESH_H__
#define __VAYO3D_MESH_H__

#include "Vayo3dSupport.h"
#include "Vayo3dVertex.h"
#include "math/VayoAabbox3d.h"
NS_VAYO3D_BEGIN

class _Vayo3dExport SubMesh
{
	friend class Mesh;
	SubMesh(Mesh* parent, EPrimitiveType primType);
public:
	Mesh*                   getParent() const;
	const wstring&          getMaterialName() const;
	void                    setMaterialName(const wstring& materialName);
	Vertex*                 getVertices();
	const Vertex*           getVertices() const;
	unsigned int            getVertexCount() const;
	unsigned*               getIndices();
	const unsigned*         getIndices() const;
	unsigned int            getIndexCount() const;
	unsigned int            getPrimCount() const;
	void                    addVertex(const Vertex& vert);
	void                    addIndex(unsigned int idx);
	void                    addPosition(const Vector3df& pos);
	void                    addNormal(const Vector3df& nor);
	void                    addColor(const Colour& clr);
	void                    addTexCoord(const Vector2df& texCoord);
	void                    setVertex(const Vertex& vert, unsigned i);
	void                    setIndex(unsigned int idx, unsigned i);
	void                    setPosition(const Vector3df& pos, unsigned i);
	void                    setNormal(const Vector3df& nor, unsigned i);
	void                    setColor(const Colour& clr, unsigned i);
	void                    setTexCoord(const Vector2df& texCoord, unsigned i);
	const vector<Vertex>&   getVertexList() const;
	const vector<unsigned>& getIndexList() const;
	void                    setVertexList(const vector<Vertex>& vertsList);
	void                    setIndexList(const vector<unsigned>& idxList);
	void                    clearVertexList();
	void                    clearIndexList();
	void                    resizeList(unsigned vertSize, unsigned idxSize);
	void                    reserveList(unsigned vertSize, unsigned idxSize);
	bool                    computeVertexNormals();
	void                    recalculateBoundingBox();
	void                    setDirty(EBufferType bufferType = EBT_VERTEX_AND_INDEX);
	void                    setHardwareMappingHint(EHardwareMapping newMappingHint, EBufferType bufferType = EBT_VERTEX_AND_INDEX);

protected:
	PROPERTY_R(EPrimitiveType,   _primType,         PrimType)
	PROPERTY_R(Aabbox3df,        _boundingBox,      BoundingBox)
	PROPERTY_R(bool,             _needUpdateAABB,   NeedUpdateAABB)
	PROPERTY_R(unsigned int,     _changedVertexID,  ChangedVertexID)
	PROPERTY_R(unsigned int,     _changedIndexID,   ChangedIndexID)
	PROPERTY_R(EHardwareMapping, _mappedVertexHint, MappedVertexHint)
	PROPERTY_R(EHardwareMapping, _mappedIndexHint,  MappedIndexHint)

private:
	Mesh*            _parent;
	vector<Vertex>   _vertices;
	vector<unsigned> _indices;
	wstring          _materialName;
	Vertex           _tempVert;
};

class _Vayo3dExport Mesh
{
	friend class MeshManager;
	Mesh(const wstring& name);
public:
	~Mesh();
	const wstring&          getName() const;
	SubMesh*                createSubMesh(EPrimitiveType primType);
	SubMesh*                createSubMesh(const wstring& name, EPrimitiveType primType);
	SubMesh*                getSubMesh(unsigned int index) const;
	SubMesh*                getSubMesh(const wstring& name) const;
	const vector<SubMesh*>& getSubMeshList() const;
	unsigned int            getSubMeshCount() const;
	void                    destroySubMesh(unsigned int index);
	void                    destroySubMesh(const wstring& name);
	void                    destroyAllSubMeshs();
	unsigned int            getSubMeshIndex(const wstring& name) const;
	void                    nameSubMesh(const wstring& name, unsigned int index);
	void                    unnameSubMesh(const wstring& name);
	bool                    isEmptyMesh() const;
	void                    recalculateBoundingBox();
	void                    setDirty();
	unsigned int            getChangedID() const;

protected:
	PROPERTY_R(Aabbox3df, _boundingBox, BoundingBox)

private:
	wstring                              _name;
	unsigned int                         _changedID;
	vector<SubMesh*>                     _subMeshList;
	unordered_map<wstring, unsigned int> _subMeshNameMap;
};

NS_VAYO3D_END

#endif // __VAYO3D_MESH_H__
