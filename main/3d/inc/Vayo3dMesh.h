/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 网格
\*************************************************************************/
#ifndef __VAYO3D_MESH_H__
#define __VAYO3D_MESH_H__

#include "Vayo3dSupport.h"
#include "Vayo3dVertex.h"
#include "VayoAabbox3d.h"
NS_VAYO3D_BEGIN

// 顶点缓冲区
class _Vayo3dExport VertexBuffer
{
public:
	VertexBuffer();
	void                  setVDirty();
	Vertex*               getVertices();
	const Vertex*         getVertices() const;
	unsigned int          getVertexCount() const;
	void                  addVertex(const Vertex& vert);
	void                  addVertices(const vector<Vertex>& verts);
	void                  addPosition(const Vector3df& pos);
	void                  addNormal(const Vector3df& nor);
	void                  addColor(const Colour& clr);
	void                  addTexCoord(const Vector2df& texCoord);
	void                  setVertex(const Vertex& vert, unsigned i);
	void                  setPosition(const Vector3df& pos, unsigned i);
	void                  setNormal(const Vector3df& nor, unsigned i);
	void                  setColor(const Colour& clr, unsigned i);
	void                  setTexCoord(const Vector2df& texCoord, unsigned i);
	const vector<Vertex>& getVertexList() const;
	void                  setVertexList(const vector<Vertex>& vertsList);
	void                  clearVertexList();
	void                  reverseVList();
	void                  resizeVList(unsigned size);
	void                  reserveVList(unsigned size);
	void                  recalculateBoundingBox();
	void                  setVHardwareMappingHint(EHardwareMapping newMappingHint);

protected:
	PROPERTY_R(Aabbox3df,        _boundingBox,      BoundingBox)
	PROPERTY_R(bool,             _needUpdateAABB,   NeedUpdateAABB)
	PROPERTY_R(unsigned int,     _changedVertexID,  ChangedVertexID)
	PROPERTY_R(EHardwareMapping, _mappedVertexHint, MappedVertexHint)

protected:
	Vertex         _tempVert;
	vector<Vertex> _vertices;
};

// 索引缓冲区
class _Vayo3dExport IndexBuffer
{
public:
	IndexBuffer(EPrimitiveType primType);
	void                    setIDirty();
	unsigned*               getIndices();
	const unsigned*         getIndices() const;
	unsigned int            getIndexCount() const;
	unsigned int            getPrimCount() const;
	void                    addIndex(unsigned int idx);
	void                    setIndex(unsigned int idx, unsigned i);
	const vector<unsigned>& getIndexList() const;
	void                    setIndexList(const vector<unsigned>& idxList);
	void                    clearIndexList();
	void                    resizeIList(unsigned size);
	void                    reserveIList(unsigned size);
	void                    setIHardwareMappingHint(EHardwareMapping newMappingHint);

protected:
	PROPERTY_R(EPrimitiveType,   _primType,        PrimType)
	PROPERTY_R(unsigned int,     _changedIndexID,  ChangedIndexID)
	PROPERTY_R(EHardwareMapping, _mappedIndexHint, MappedIndexHint)

protected:
	vector<unsigned> _indices;
};

// 子网格
class _Vayo3dExport SubMesh : public VertexBuffer, public IndexBuffer
{
	friend class Mesh;
	SubMesh(Mesh* parent, EPrimitiveType primType);
public:
	Mesh*          getParent() const;
	const wstring& getMaterialName() const;
	void           setMaterialName(const wstring& materialName);
	void           resizeList(unsigned vertSize, unsigned idxSize);
	void           reserveList(unsigned vertSize, unsigned idxSize);
	bool           computeFaceNormals();
	bool           computeVertexNormals();
	void           setDirty(EBufferType bufferType = EBT_VERTEX_AND_INDEX);
	void           setHardwareMappingHint(EHardwareMapping newMappingHint, EBufferType bufferType = EBT_VERTEX_AND_INDEX);

private:
	Mesh*   _parent;
	wstring _materialName;
};

// 共享子网格
class _Vayo3dExport SharedSubMesh : public VertexBuffer
{
	friend class Mesh;
	SharedSubMesh();
	~SharedSubMesh();
public:
	IndexBuffer*               createIBuffer(EPrimitiveType primType);
	IndexBuffer*               getIBuffer(unsigned int index);
	const vector<IndexBuffer>& getIBufferList() const;
	unsigned int               getIBufferCount() const;
	void                       destroyIBuffer(unsigned int index);
	void                       destroyAllIBuffers();
	wstring                    getMaterialName(unsigned int ibufIndex) const;
	void                       setMaterialName(unsigned int ibufIndex, const wstring& materialName);
	unsigned int               addCombineVertex(const Vertex& vert);
	void                       addCombineVertices(const vector<Vertex>& verts);
	void                       setCombineVertexList(const vector<Vertex>& vertsList);
	void                       clearCombineVertexList();
	void                       mergeCombineVerticesToVBuffer();

private:
	vector<IndexBuffer>     _idxBuffers;
	vector<Vertex>          _combineVertices;
	vector<wstring>         _materialNames;
	map<unsigned, unsigned> _idxBufMaterialNameMap;
	map<unsigned, unsigned> _materialNameIdxBufMap;
};

// 网格
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
	void                    computeNormals(ENormalType normType);
	void                    recalculateBoundingBox();
	void                    setDirty();
	unsigned int            getChangedID() const;
	SharedSubMesh*          createSharedSubMesh();
	void                    destroySharedSubMesh();

protected:
	PROPERTY_R(Aabbox3df, _boundingBox, BoundingBox)
	PROPERTY_R(SharedSubMesh*, _sharedSubMesh, SharedSubMesh)

private:
	wstring                              _name;
	unsigned int                         _changedID;
	vector<SubMesh*>                     _subMeshList;
	unordered_map<wstring, unsigned int> _subMeshNameMap;
};

NS_VAYO3D_END

#endif // __VAYO3D_MESH_H__
