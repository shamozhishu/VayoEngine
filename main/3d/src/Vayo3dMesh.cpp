#include "Vayo3dMesh.h"
#include "VayoLog.h"
#include "VayoTriangle3d.h"

NS_VAYO3D_BEGIN

void VertexBuffer::setVDirty()
{
	++_changedVertexID;
	_needUpdateAABB = true;
}

VertexBuffer::VertexBuffer()
	: _needUpdateAABB(false)
	, _changedVertexID(0)
	, _mappedVertexHint(EHM_NEVER)
{
}

Vertex* VertexBuffer::getVertices()
{
	if (_vertices.size() > 0)
		return &(_vertices[0]);
	return nullptr;
}

const Vertex* VertexBuffer::getVertices() const
{
	if (_vertices.size() > 0)
		return &(_vertices[0]);
	return nullptr;
}

unsigned int VertexBuffer::getVertexCount() const
{
	return _vertices.size();
}

void VertexBuffer::addVertex(const Vertex& vert)
{
	_vertices.push_back(vert);
}

void VertexBuffer::addVertices(const vector<Vertex>& verts)
{
	_vertices.insert(_vertices.end(), verts.begin(), verts.end());
}

void VertexBuffer::addPosition(const Vector3df& pos)
{
	_tempVert._position = pos;
	_vertices.push_back(_tempVert);
}

void VertexBuffer::addNormal(const Vector3df& nor)
{
	_tempVert._normal = nor;
}

void VertexBuffer::addColor(const Colour& clr)
{
	_tempVert._color = clr;
}

void VertexBuffer::addTexCoord(const Vector2df& texCoord)
{
	_tempVert._texCoord = texCoord;
}

void VertexBuffer::setVertex(const Vertex& vert, unsigned i)
{
	if (i < _vertices.size())
		_vertices[i] = vert;
}

void VertexBuffer::setPosition(const Vector3df& pos, unsigned i)
{
	if (i < _vertices.size())
		_vertices[i]._position = pos;
}

void VertexBuffer::setNormal(const Vector3df& nor, unsigned i)
{
	if (i < _vertices.size())
		_vertices[i]._normal = nor;
}

void VertexBuffer::setColor(const Colour& clr, unsigned i)
{
	if (i < _vertices.size())
		_vertices[i]._color = clr;
}

void VertexBuffer::setTexCoord(const Vector2df& texCoord, unsigned i)
{
	if (i < _vertices.size())
		_vertices[i]._texCoord = texCoord;
}

const vector<Vertex>& VertexBuffer::getVertexList() const
{
	return _vertices;
}

void VertexBuffer::setVertexList(const vector<Vertex>& vertsList)
{
	_vertices = vertsList;
}

void VertexBuffer::clearVertexList()
{
	_vertices.clear();
}

void VertexBuffer::reverseVList()
{
	std::reverse(_vertices.begin(), _vertices.end());
}

void VertexBuffer::resizeVList(unsigned size)
{
	if (size != (unsigned)-1)
		_vertices.resize(size);
}

void VertexBuffer::reserveVList(unsigned size)
{
	if (size != (unsigned)-1)
		_vertices.reserve(size);
}

void VertexBuffer::recalculateBoundingBox()
{
	if (!_needUpdateAABB)
		return;

	if (_vertices.empty())
		_boundingBox.reset(0, 0, 0);
	else
	{
		_boundingBox.reset(_vertices[0]._position);
		for (unsigned int i = 1; i < _vertices.size(); ++i)
			_boundingBox.addInternalPoint(_vertices[i]._position);
	}

	_needUpdateAABB = false;
}

void VertexBuffer::setVHardwareMappingHint(EHardwareMapping newMappingHint)
{
	_mappedVertexHint = newMappingHint;
}

//////////////////////////////////////////////////////////////////////////
IndexBuffer::IndexBuffer(EPrimitiveType primType)
	: _primType(primType)
	, _changedIndexID(0)
	, _mappedIndexHint(EHM_NEVER)
{
}

void IndexBuffer::setIDirty()
{
	++_changedIndexID;
}

unsigned* IndexBuffer::getIndices()
{
	if (_indices.size() > 0)
		return &(_indices[0]);
	return nullptr;
}

const unsigned* IndexBuffer::getIndices() const
{
	if (_indices.size() > 0)
		return &(_indices[0]);
	return nullptr;
}

unsigned int IndexBuffer::getIndexCount() const
{
	return _indices.size();
}

unsigned int IndexBuffer::getPrimCount() const
{
	unsigned int idxNum = _indices.size();
	switch (_primType)
	{
	case EPT_POINTS:
	case EPT_LINE_LOOP:
	case EPT_POLYGON:
		return idxNum;
	case EPT_LINES:
		return idxNum / 2;
	case EPT_LINE_STRIP:
		return idxNum - 1;
	case EPT_TRIANGLES:
		return idxNum / 3;
	case EPT_TRIANGLE_STRIP:
	case EPT_TRIANGLE_FAN:
		return idxNum - 2;
	case EPT_QUADS:
		return idxNum / 4;
	case EPT_QUAD_STRIP:
		return (idxNum - 2) / 2;
	}
	return 0;
}

void IndexBuffer::addIndex(unsigned int idx)
{
	_indices.push_back(idx);
}

void IndexBuffer::setIndex(unsigned int idx, unsigned i)
{
	if (i >= _indices.size())
		return;
	_indices[i] = idx;
}

const vector<unsigned>& IndexBuffer::getIndexList() const
{
	return _indices;
}

void IndexBuffer::setIndexList(const vector<unsigned>& idxList)
{
	_indices = idxList;
}

void IndexBuffer::clearIndexList()
{
	_indices.clear();
}

void IndexBuffer::resizeIList(unsigned size)
{
	if (size != (unsigned)-1)
		_indices.resize(size);
}

void IndexBuffer::reserveIList(unsigned size)
{
	if (size != (unsigned)-1)
		_indices.reserve(size);
}

void IndexBuffer::setIHardwareMappingHint(EHardwareMapping newMappingHint)
{
	_mappedIndexHint = newMappingHint;
}

//////////////////////////////////////////////////////////////////////////
SubMesh::SubMesh(Mesh* parent, EPrimitiveType primType)
	: IndexBuffer(primType)
	, _parent(parent)
{
}

Mesh* SubMesh::getParent() const
{
	return _parent;
}

const wstring& SubMesh::getMaterialName() const
{
	return _materialName;
}

void SubMesh::setMaterialName(const wstring& materialName)
{
	_materialName = materialName;
}

void SubMesh::resizeList(unsigned vertSize, unsigned idxSize)
{
	VertexBuffer::resizeVList(vertSize);
	IndexBuffer::resizeIList(idxSize);
}

void SubMesh::reserveList(unsigned vertSize, unsigned idxSize)
{
	VertexBuffer::reserveVList(vertSize);
	IndexBuffer::reserveIList(idxSize);
}

bool SubMesh::computeFaceNormals()
{
	if (_primType != EPT_TRIANGLES)
		return false;

	int vertexCount = (int)_vertices.size();
	if (vertexCount <= 0)
		return false;

	int indexCount = (int)_indices.size();
	if (indexCount <= 0)
		return false;

	for (int i = 0; i < vertexCount; ++i)
		_vertices[i]._normal.set(0, 0, 0);

	Triangle3df tri;
	Vector3df norm;
	unsigned idx1, idx2, idx3;
	for (int i = 0; i < indexCount; i += 3)
	{
		idx1 = _indices[i];
		idx2 = _indices[i + 1];
		idx3 = _indices[i + 2];
		tri.set(_vertices[idx1]._position, _vertices[idx2]._position, _vertices[idx3]._position);
		norm = tri.getNormal().normalize();
		if (_vertices[idx1]._normal == Vector3df::Origin)
			_vertices[idx1]._normal = norm;
		if (_vertices[idx2]._normal == Vector3df::Origin)
			_vertices[idx2]._normal = norm;
		if (_vertices[idx3]._normal == Vector3df::Origin)
			_vertices[idx3]._normal = norm;
	}

	return true;
}

bool SubMesh::computeVertexNormals()
{
	if (_primType != EPT_TRIANGLES)
		return false;

	int vertexCount = (int)_vertices.size();
	if (vertexCount <= 0)
		return false;

	int indexCount = (int)_indices.size();
	if (indexCount <= 0)
		return false;

	for (int i = 0; i < vertexCount; ++i)
		_vertices[i]._normal.set(0, 0, 0);

	Triangle3df tri;
	unsigned idx1, idx2, idx3;
	for (int i = 0; i < indexCount; i += 3)
	{
		idx1 = _indices[i];
		idx2 = _indices[i + 1];
		idx3 = _indices[i + 2];
		tri.set(_vertices[idx1]._position, _vertices[idx2]._position, _vertices[idx3]._position);
		_vertices[idx1]._normal = _vertices[idx2]._normal = _vertices[idx3]._normal += tri.getNormal().normalize();
	}

	for (int i = 0; i < vertexCount; ++i)
		_vertices[i]._normal.normalize();

	return true;
}

void SubMesh::setDirty(EBufferType bufferType /*= EBT_VERTEX_AND_INDEX*/)
{
	if (bufferType == EBT_VERTEX_AND_INDEX || bufferType == EBT_VERTEX)
		VertexBuffer::setVDirty();
	if (bufferType == EBT_VERTEX_AND_INDEX || bufferType == EBT_INDEX)
		IndexBuffer::setIDirty();
}

void SubMesh::setHardwareMappingHint(EHardwareMapping newMappingHint, EBufferType bufferType /*= EBT_VERTEX_AND_INDEX*/)
{
	if (bufferType == EBT_VERTEX_AND_INDEX || bufferType == EBT_VERTEX)
		VertexBuffer::setVHardwareMappingHint(newMappingHint);
	if (bufferType == EBT_VERTEX_AND_INDEX || bufferType == EBT_INDEX)
		IndexBuffer::setIHardwareMappingHint(newMappingHint);
}

//////////////////////////////////////////////////////////////////////////
SharedSubMesh::SharedSubMesh()
{
}

SharedSubMesh::~SharedSubMesh()
{
	destroyAllIBuffers();
}

IndexBuffer* SharedSubMesh::createIBuffer(EPrimitiveType primType)
{
	_idxBuffers.push_back(IndexBuffer(primType));
	return &_idxBuffers.back();
}

IndexBuffer* SharedSubMesh::getIBuffer(unsigned int index)
{
	VAYO_ASSERT(index < _idxBuffers.size());
	return &_idxBuffers[index];
}

const vector<IndexBuffer>& SharedSubMesh::getIBufferList() const
{
	return _idxBuffers;
}

unsigned int SharedSubMesh::getIBufferCount() const
{
	return _idxBuffers.size();
}

void SharedSubMesh::destroyIBuffer(unsigned int index)
{
	if (index < _idxBuffers.size())
	{
		vector<IndexBuffer>::iterator i = _idxBuffers.begin();
		std::advance(i, index);
		_idxBuffers.erase(i);
	}
}

void SharedSubMesh::destroyAllIBuffers()
{
	_idxBuffers.clear();
}

wstring SharedSubMesh::getMaterialName(unsigned int ibufIndex) const
{
	auto it = _idxBufMaterialNameMap.find(ibufIndex);
	if (it == _idxBufMaterialNameMap.end())
		return L"";

	unsigned int materialIdx = it->second;
	if (materialIdx >= _materialNames.size())
		return L"";

	return _materialNames[materialIdx];
}

void SharedSubMesh::setMaterialName(unsigned int ibufIndex, const wstring& materialName)
{
	unsigned materialIdx = -1;
	unsigned size = _materialNames.size();
	for (unsigned i = 0; i < size; ++i)
	{
		if (_materialNames[i] == materialName)
		{
			materialIdx = i;
			break;
		}
	}

	if (materialIdx == -1)
	{
		materialIdx = _materialNames.size();
		_idxBufMaterialNameMap[ibufIndex] = materialIdx;
		_materialNames.push_back(materialName);
		_materialNameIdxBufMap[materialIdx] = ibufIndex;
	}
	else
	{
		_idxBufMaterialNameMap[ibufIndex] = materialIdx;
		_materialNameIdxBufMap[materialIdx] = ibufIndex;
	}
}

unsigned int SharedSubMesh::addCombineVertex(const Vertex& vert)
{
	_combineVertices.push_back(vert);
	return _combineVertices.size() - 1;
}

void SharedSubMesh::addCombineVertices(const vector<Vertex>& verts)
{
	_combineVertices.insert(_combineVertices.end(), verts.begin(), verts.end());
}

void SharedSubMesh::setCombineVertexList(const vector<Vertex>& vertsList)
{
	_combineVertices = vertsList;
}

void SharedSubMesh::clearCombineVertexList()
{
	_combineVertices.clear();
}

void SharedSubMesh::mergeCombineVerticesToVBuffer()
{
	if (_combineVertices.size() > 0)
	{
		addVertices(_combineVertices);
		_combineVertices.clear();
	}
}

//////////////////////////////////////////////////////////////////////////
Mesh::Mesh(const wstring& name)
	: _name(name)
	, _changedID(0)
	, _sharedSubMesh(nullptr)
{
	static unsigned long long idx = 0;
	if (0 == _name.compare(L""))
	{
		std::wstringstream ss;
		ss << L"Mesh" << idx;
		++idx;
		_name = ss.str();
	}
}

Mesh::~Mesh()
{
	destroyAllSubMeshs();
	destroySharedSubMesh();
}

const wstring& Mesh::getName() const
{
	return _name;
}

SubMesh* Mesh::createSubMesh(EPrimitiveType primType)
{
	SubMesh* sub = new SubMesh(this, primType);
	_subMeshList.push_back(sub);
	setDirty();
	return sub;
}

SubMesh* Mesh::createSubMesh(const wstring& name, EPrimitiveType primType)
{
	SubMesh *sub = createSubMesh(primType);
	_subMeshNameMap[name] = _subMeshList.size() - 1;
	return sub;
}

SubMesh* Mesh::getSubMesh(unsigned int index) const
{
	if (index >= _subMeshList.size())
	{
		Log::wprint(ELL_ERROR, L"Ë÷Òý³ö½ç(Mesh::getSubMesh)");
		return NULL;
	}
	return _subMeshList[index];
}

SubMesh* Mesh::getSubMesh(const wstring& name) const
{
	unsigned int index = getSubMeshIndex(name);
	return getSubMesh(index);
}

const vector<SubMesh*>& Mesh::getSubMeshList() const
{
	return _subMeshList;
}

unsigned int Mesh::getSubMeshCount() const
{
	return _subMeshList.size();
}

void Mesh::destroySubMesh(unsigned int index)
{
	if (index >= _subMeshList.size())
	{
		Log::wprint(ELL_WARNING, L"Index out of bounds.(Mesh::destroySubMesh)");
		return;
	}
	vector<SubMesh*>::iterator i = _subMeshList.begin();
	std::advance(i, index);
	delete *i;
	_subMeshList.erase(i);
	setDirty();
	for (unordered_map<wstring, unsigned int>::iterator ni = _subMeshNameMap.begin(); ni != _subMeshNameMap.end();)
	{
		if (ni->second == index)
		{
			ni = _subMeshNameMap.erase(ni);
		}
		else
		{
			if (ni->second > index)
				ni->second = ni->second - 1;
			++ni;
		}
	}
}

void Mesh::destroySubMesh(const wstring& name)
{
	unsigned short index = getSubMeshIndex(name);
	destroySubMesh(index);
}

void Mesh::destroyAllSubMeshs()
{
	vector<SubMesh*>::iterator it = _subMeshList.begin();
	for (; it != _subMeshList.end(); ++it)
		delete *it;
	_subMeshList.clear();
	_subMeshNameMap.clear();
}

unsigned int Mesh::getSubMeshIndex(const wstring& name) const
{
	unordered_map<wstring, unsigned int>::const_iterator i = _subMeshNameMap.find(name);
	if (i == _subMeshNameMap.end())
	{
		Log::wprint(ELL_WARNING, L"No SubMesh named %s found(Mesh::getSubMeshIndex)", name.c_str());
		return (unsigned int)-1;
	}
	return i->second;
}

void Mesh::nameSubMesh(const wstring& name, unsigned int index)
{
	_subMeshNameMap[name] = index;
}

void Mesh::unnameSubMesh(const wstring& name)
{
	unordered_map<wstring, unsigned int>::iterator i = _subMeshNameMap.find(name);
	if (i != _subMeshNameMap.end())
		_subMeshNameMap.erase(i);
}

bool Mesh::isEmptyMesh() const
{
	if (_subMeshList.empty() && (!_sharedSubMesh || _sharedSubMesh->_vertices.empty()))
		return true;

	bool isEmpty = true;
	if (_sharedSubMesh)
	{
		isEmpty = _sharedSubMesh->_vertices.empty();
		if (!isEmpty)
			return false;
	}

	SubMesh* pSubMesh = NULL;
	vector<SubMesh*>::const_iterator cit = _subMeshList.cbegin();
	for (; cit != _subMeshList.cend(); ++cit)
	{
		pSubMesh = (*cit);
		isEmpty = pSubMesh->_vertices.empty();
		if (!isEmpty)
			break;
	}

	return isEmpty;
}

void Mesh::computeNormals(ENormalType normType)
{
	unsigned size = _subMeshList.size();
	switch (normType)
	{
	case ENT_FACE:
		for (unsigned int i = 0; i < size; ++i)
			_subMeshList[i]->computeFaceNormals();
		break;
	case ENT_VERTEX:
		for (unsigned int i = 0; i < size; ++i)
			_subMeshList[i]->computeVertexNormals();
		break;
	default:
		return;
	}
}

void Mesh::recalculateBoundingBox()
{
	if (_subMeshList.size())
	{
		_subMeshList[0]->recalculateBoundingBox();
		_boundingBox = _subMeshList[0]->getBoundingBox();
		for (unsigned int i = 1; i < _subMeshList.size(); ++i)
		{
			_subMeshList[i]->recalculateBoundingBox();
			_boundingBox.addInternalBox(_subMeshList[i]->getBoundingBox());
		}
	}
	else
		_boundingBox.reset(0.0f, 0.0f, 0.0f);
}

void Mesh::setDirty()
{
	++_changedID;
}

unsigned int Mesh::getChangedID() const
{
	return _changedID;
}

SharedSubMesh* Mesh::createSharedSubMesh()
{
	if (!_sharedSubMesh)
		_sharedSubMesh = new SharedSubMesh();
	return _sharedSubMesh;
}

void Mesh::destroySharedSubMesh()
{
	SAFE_DELETE(_sharedSubMesh);
}

NS_VAYO3D_END
