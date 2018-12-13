#include "VayoMesh.h"
#include "VayoLog.h"

NS_VAYO_BEGIN

Mesh* SubMesh::getParent() const
{
	return _parent;
}

SubMesh::SubMesh(Mesh* parent, EPrimitiveType primType)
	: _parent(parent)
	, _needUpdateAABB(false)
	, _primType(primType)
	, _changedVertexID(0)
	, _changedIndexID(0)
	, _mappedVertexHint(EHM_NEVER)
	, _mappedIndexHint(EHM_NEVER)
{
}

const wstring& SubMesh::getMaterialName() const
{
	return _materialName;
}

void SubMesh::setMaterialName(const wstring& materialName)
{
	_materialName = materialName;
}

Vertex* SubMesh::getVertices()
{
	if (_vertices.size() > 0)
		return &(_vertices[0]);
	return NULL;
}

const Vertex* SubMesh::getVertices() const
{
	if (_vertices.size() > 0)
		return &(_vertices[0]);
	return NULL;
}

unsigned int SubMesh::getVertexCount() const
{
	return _vertices.size();
}

unsigned* SubMesh::getIndices()
{
	if (_indices.size() > 0)
		return &(_indices[0]);
	return NULL;
}

const unsigned* SubMesh::getIndices() const
{
	if (_indices.size() > 0)
		return &(_indices[0]);
	return NULL;
}

unsigned int SubMesh::getIndexCount() const
{
	return _indices.size();
}

unsigned int SubMesh::getPrimCount() const
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

void SubMesh::addVertex(const Vertex& vert)
{
	_vertices.push_back(vert);
}

void SubMesh::addIndex(unsigned int idx)
{
	_indices.push_back(idx);
}

void SubMesh::addPosition(const Vector3df& pos)
{
	_tempVert._position = pos;
	_vertices.push_back(_tempVert);
}

void SubMesh::addNormal(const Vector3df& nor)
{
	_tempVert._normal = nor;
}

void SubMesh::addColor(const Colour& clr)
{
	_tempVert._color = clr;
}

void SubMesh::addTexCoord(const Vector2df& texCoord)
{
	_tempVert._texCoord = texCoord;
}

void SubMesh::setVertex(const Vertex& vert, unsigned i)
{
	if (i >= _vertices.size())
		return;
	_vertices[i] = vert;
}

void SubMesh::setIndex(unsigned int idx, unsigned i)
{
	if (i >= _indices.size())
		return;
	_indices[i] = idx;
}

void SubMesh::setPosition(const Vector3df& pos, unsigned i)
{
	if (i >= _vertices.size())
		return;
	_vertices[i]._position = pos;
}

void SubMesh::setNormal(const Vector3df& nor, unsigned i)
{
	if (i >= _vertices.size())
		return;
	_vertices[i]._normal = nor;
}

void SubMesh::setColor(const Colour& clr, unsigned i)
{
	if (i >= _vertices.size())
		return;
	_vertices[i]._color = clr;
}

void SubMesh::setTexCoord(const Vector2df& texCoord, unsigned i)
{
	if (i >= _vertices.size())
		return;
	_vertices[i]._texCoord = texCoord;
}

const vector<Vertex>& SubMesh::getVertexList() const
{
	return _vertices;
}

const vector<unsigned>& SubMesh::getIndexList() const
{
	return _indices;
}

void SubMesh::setVertexList(const vector<Vertex>& vertsList)
{
	_vertices = vertsList;
}

void SubMesh::setIndexList(const vector<unsigned>& idxList)
{
	_indices = idxList;
}

void SubMesh::clearVertexList()
{
	_vertices.clear();
}

void SubMesh::clearIndexList()
{
	_indices.clear();
}

void SubMesh::resizeList(unsigned vertSize, unsigned idxSize)
{
	if (vertSize != (unsigned)-1)
		_vertices.resize(vertSize);
	if (idxSize != (unsigned)-1)
		_indices.resize(idxSize);
}

void SubMesh::reserveList(unsigned vertSize, unsigned idxSize)
{
	if (vertSize != (unsigned)-1)
		_vertices.reserve(vertSize);
	if (idxSize != (unsigned)-1)
		_indices.reserve(idxSize);
}

void SubMesh::recalculateBoundingBox()
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

void SubMesh::setDirty(EBufferType bufferType /*= EBT_VERTEX_AND_INDEX*/)
{
	if (bufferType == EBT_VERTEX_AND_INDEX || bufferType == EBT_VERTEX)
	{
		++_changedVertexID;
		_needUpdateAABB = true;
	}

	if (bufferType == EBT_VERTEX_AND_INDEX || bufferType == EBT_INDEX)
		++_changedIndexID;
}

void SubMesh::setHardwareMappingHint(EHardwareMapping newMappingHint, EBufferType bufferType /*= EBT_VERTEX_AND_INDEX*/)
{
	if (bufferType == EBT_VERTEX_AND_INDEX || bufferType == EBT_VERTEX)
		_mappedVertexHint = newMappingHint;
	if (bufferType == EBT_VERTEX_AND_INDEX || bufferType == EBT_INDEX)
		_mappedIndexHint = newMappingHint;
}

//////////////////////////////////////////////////////////////////////////
Mesh::Mesh(const wstring& name)
	: _name(name)
	, _changedID(0)
{
	static unsigned short idx = 0;
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
	if (_subMeshList.empty())
		return true;

	bool isEmpty = true;
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

NS_VAYO_END
