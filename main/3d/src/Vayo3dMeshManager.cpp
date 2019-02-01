#include "Vayo3dMeshManager.h"
#include "VayoLog.h"

NS_VAYO3D_BEGIN

MeshManager::MeshManager()
{
}

MeshManager::~MeshManager()
{
}

bool MeshManager::init()
{
	_meshPool.clear();
	return true;
}

MeshPtr MeshManager::findMesh(const wstring& name)
{
	unordered_map<wstring, MeshPtr>::iterator it = _meshPool.find(name);
	if (it == _meshPool.end())
	{
		Log::wprint(ELL_WARNING, L"Íø¸ñ%s²»´æÔÚ", name.c_str());
		return NULL;
	}
	return it->second;
}

MeshPtr MeshManager::createEmptyMesh(const wstring& name /*= L""*/)
{
	unordered_map<wstring, MeshPtr>::iterator it = _meshPool.find(name);
	if (it != _meshPool.end())
		return it->second;
	MeshPtr pMesh(new Mesh(name));
	_meshPool[pMesh->getName()] = pMesh;
	return pMesh;
}

void MeshManager::destroyMesh(const wstring& name)
{
	unordered_map<wstring, MeshPtr>::iterator it = _meshPool.find(name);
	if (it != _meshPool.end())
		_meshPool.erase(it);
}

void MeshManager::destroyMesh(MeshPtr mesh)
{
	if (mesh)
		destroyMesh(mesh->getName());
}

MeshPtr MeshManager::createCubeMesh(const Vector3df& size, const wstring& materialName /*= L""*/)
{
	return createCubeMesh(L"", size, materialName);
}

MeshPtr MeshManager::createCubeMesh(const wstring& name, const Vector3df& size, const wstring& materialName /*= L""*/)
{
	MeshPtr pMesh(new Mesh(name));
	SubMesh* pSubMesh = pMesh->createSubMesh(EPT_TRIANGLES);
	pSubMesh->setMaterialName(materialName);
	pSubMesh->setHardwareMappingHint(EHM_STATIC);

	Colour clr(255, 255, 255, 255);
	pSubMesh->resizeList(12, -1);
	pSubMesh->setVertex(Vertex(0, 0, 0, -1, -1, 1, clr, 0, 1), 0);
	pSubMesh->setVertex(Vertex(1, 0, 0, 1, -1, 1, clr, 1, 1), 1);
	pSubMesh->setVertex(Vertex(1, 1, 0, 1, 1, 1, clr, 1, 0), 2);
	pSubMesh->setVertex(Vertex(0, 1, 0, -1, 1, 1, clr, 0, 0), 3);
	pSubMesh->setVertex(Vertex(1, 0, 1, 1, -1, -1, clr, 0, 1), 4);
	pSubMesh->setVertex(Vertex(1, 1, 1, 1, 1, -1, clr, 0, 0), 5);
	pSubMesh->setVertex(Vertex(0, 1, 1, -1, 1, -1, clr, 1, 0), 6);
	pSubMesh->setVertex(Vertex(0, 0, 1, -1, -1, -1, clr, 1, 1), 7);
	pSubMesh->setVertex(Vertex(0, 1, 1, -1, 1, -1, clr, 0, 1), 8);
	pSubMesh->setVertex(Vertex(0, 1, 0, -1, 1, 1, clr, 1, 1), 9);
	pSubMesh->setVertex(Vertex(1, 0, 1, 1, -1, -1, clr, 1, 0), 10);
	pSubMesh->setVertex(Vertex(1, 0, 0, 1, -1, 1, clr, 0, 0), 11);

	const unsigned int u[36] = { 0,2,1,   0,3,2,   1,5,4,   1,2,5,   4,6,7,   4,5,6,
		7,3,0,   7,6,3,   9,5,2,   9,8,5,   0,11,10,   0,10,7 };
	pSubMesh->resizeList(-1, 36);
	for (unsigned int i = 0; i < 36; ++i)
		pSubMesh->setIndex(u[i], i);

	for (unsigned int i = 0; i < 12; ++i)
	{
		pSubMesh->getVertices()[i]._position -= Vector3df(0.5f, 0.5f, 0.5f);
		pSubMesh->getVertices()[i]._position *= size;
	}

	pSubMesh->setDirty();
	_meshPool[pMesh->getName()] = pMesh;
	return pMesh;
}

MeshPtr MeshManager::createSphereMesh(float radius, unsigned int polyCountX,
	unsigned int polyCountY, const wstring& materialName /*= L""*/)
{
	return createSphereMesh(L"", radius, polyCountX, polyCountY, materialName);
}

MeshPtr MeshManager::createSphereMesh(const wstring& name, float radius,
	unsigned int polyCountX, unsigned int polyCountY, const wstring& materialName /*= L""*/)
{
	MeshPtr pMesh(new Mesh(name));
	SubMesh* pSubMesh = pMesh->createSubMesh(EPT_TRIANGLES);
	pSubMesh->setMaterialName(materialName);
	pSubMesh->setHardwareMappingHint(EHM_STATIC);

	// we are creating the sphere mesh here.

	if (polyCountX < 2)
		polyCountX = 2;
	if (polyCountY < 2)
		polyCountY = 2;
	while (polyCountX * polyCountY > 32767) // prevent unsigned short overflow
	{
		polyCountX /= 2;
		polyCountY /= 2;
	}

	const unsigned int polyCountXPitch = polyCountX + 1; // get to same vertex on next level

	const Colour clr(255, 255, 255, 255);

	unsigned int level = 0;

	for (unsigned int p1 = 0; p1 < polyCountY - 1; ++p1)
	{
		// main quads, top to bottom
		for (unsigned int p2 = 0; p2 < polyCountX - 1; ++p2)
		{
			const unsigned int curr = level + p2;
			pSubMesh->addIndex(curr + polyCountXPitch);
			pSubMesh->addIndex(curr);
			pSubMesh->addIndex(curr + 1);
			pSubMesh->addIndex(curr + polyCountXPitch);
			pSubMesh->addIndex(curr + 1);
			pSubMesh->addIndex(curr + 1 + polyCountXPitch);
		}

		// the connectors from front to end
		pSubMesh->addIndex(level + polyCountX - 1 + polyCountXPitch);
		pSubMesh->addIndex(level + polyCountX - 1);
		pSubMesh->addIndex(level + polyCountX);

		pSubMesh->addIndex(level + polyCountX - 1 + polyCountXPitch);
		pSubMesh->addIndex(level + polyCountX);
		pSubMesh->addIndex(level + polyCountX + polyCountXPitch);
		level += polyCountXPitch;
	}

	const unsigned int polyCountSq = polyCountXPitch * polyCountY; // top point
	const unsigned int polyCountSq1 = polyCountSq + 1; // bottom point
	const unsigned int polyCountSqM1 = (polyCountY - 1) * polyCountXPitch; // last row's first vertex

	for (unsigned int p2 = 0; p2 < polyCountX - 1; ++p2)
	{
		// create triangles which are at the top of the sphere

		pSubMesh->addIndex(polyCountSq);
		pSubMesh->addIndex(p2 + 1);
		pSubMesh->addIndex(p2);

		// create triangles which are at the bottom of the sphere

		pSubMesh->addIndex(polyCountSqM1 + p2);
		pSubMesh->addIndex(polyCountSqM1 + p2 + 1);
		pSubMesh->addIndex(polyCountSq1);
	}

	// create final triangle which is at the top of the sphere

	pSubMesh->addIndex(polyCountSq);
	pSubMesh->addIndex(polyCountX);
	pSubMesh->addIndex(polyCountX - 1);

	// create final triangle which is at the bottom of the sphere

	pSubMesh->addIndex(polyCountSqM1 + polyCountX - 1);
	pSubMesh->addIndex(polyCountSqM1);
	pSubMesh->addIndex(polyCountSq1);

	// calculate the angle which separates all points in a circle
	const double angleX = 2 * VAYO_MATH_PI / polyCountX;
	const double angleY = VAYO_MATH_PI / polyCountY;

	unsigned int i = 0;
	double axz;

	// we don't start at 0.

	double ay = 0;

	pSubMesh->resizeList((polyCountXPitch * polyCountY) + 2, -1);
	for (unsigned int y = 0; y < polyCountY; ++y)
	{
		ay += angleY;
		const double sinay = sin(ay);
		axz = 0;

		// calculate the necessary vertices without the doubled one
		for (unsigned int xz = 0; xz < polyCountX; ++xz)
		{
			// calculate points position
			const Vector3df pos(static_cast<float>(radius * cos(axz) * sinay),
				static_cast<float>(radius * cos(ay)),
				static_cast<float>(radius * sin(axz) * sinay));
			// for spheres the normal is the position
			Vector3df normal(pos);
			normal.normalize();

			// calculate texture coordinates via sphere mapping
			// tu is the same on each level, so only calculate once
			float tu = 0.5f;
			if (y == 0)
			{
				if (normal._y != -1.0f && normal._y != 1.0f)
					tu = static_cast<float>(acos(clamp_(normal._x / sinay, -1.0, 1.0)) * 0.5 * VAYO_MATH_RECIPROCAL_PI);
				if (normal._z < 0.0f)
					tu = 1 - tu;
			}
			else
				tu = pSubMesh->getVertices()[i - polyCountXPitch]._texCoord._x;
			pSubMesh->setVertex(Vertex(pos._x, pos._y, pos._z,
				normal._x, normal._y, normal._z, clr,
				tu, static_cast<float>(ay*VAYO_MATH_RECIPROCAL_PI)), i);
			++i;
			axz += angleX;
		}
		// This is the doubled vertex on the initial position
		pSubMesh->setVertex(Vertex(pSubMesh->getVertices()[i - polyCountX]), i);
		pSubMesh->getVertices()[i]._texCoord._x = 1.0f;
		++i;
	}

	// the vertex at the top of the sphere
	pSubMesh->setVertex(Vertex(0.0f, radius, 0.0f, 0.0f, 1.0f, 0.0f, clr, 0.5f, 0.0f), i);

	// the vertex at the bottom of the sphere
	++i;
	pSubMesh->setVertex(Vertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, clr, 0.5f, 1.0f), i);

	pSubMesh->setDirty();
	_meshPool[pMesh->getName()] = pMesh;
	return pMesh;
}

MeshPtr MeshManager::createCylinderMesh(float radius, float length, unsigned int tesselation,
	const Colour& color /*= 0xffffffff*/, bool closeTop /*= true*/, float oblique /*= 0.0f*/,
	const wstring& materialName /*= L""*/)
{
	return createCylinderMesh(L"", radius, length, tesselation, color, closeTop, oblique, materialName);
}

MeshPtr MeshManager::createCylinderMesh(const wstring& name, float radius, float length,
	unsigned int tesselation, const Colour& color /*= 0xffffffff*/, bool closeTop /*= true*/,
	float oblique /*= 0.0f*/, const wstring& materialName /*= L""*/)
{
	MeshPtr pMesh(new Mesh(name));
	SubMesh* pSubMesh = pMesh->createSubMesh(EPT_TRIANGLES);
	pSubMesh->setMaterialName(materialName);
	pSubMesh->setHardwareMappingHint(EHM_STATIC);

	const float recTesselation = 1.0f / (float)tesselation;
	const float recTesselationHalf = recTesselation * 0.5f;
	const float angleStep = (VAYO_MATH_PI * 2.f) * recTesselation;
	const float angleStepHalf = angleStep*0.5f;

	unsigned int i;
	Vertex v;
	v._color = color;
	pSubMesh->reserveList(tesselation * 4 + 4 + (closeTop ? 2 : 1), (tesselation * 2 + 1)*(closeTop ? 12 : 9));
	float tcx = 0.f;
	for (i = 0; i <= tesselation; ++i)
	{
		const float angle = angleStep * i;
		v._position._x = radius * cosf(angle);
		v._position._y = 0.f;
		v._position._z = radius * sinf(angle);
		v._normal = v._position;
		v._normal.normalize();
		v._texCoord._x = tcx;
		v._texCoord._y = 0.f;
		pSubMesh->addVertex(v);

		v._position._x += oblique;
		v._position._y = length;
		v._normal = v._position;
		v._normal.normalize();
		v._texCoord._y = 1.f;
		pSubMesh->addVertex(v);

		v._position._x = radius * cosf(angle + angleStepHalf);
		v._position._y = 0.f;
		v._position._z = radius * sinf(angle + angleStepHalf);
		v._normal = v._position;
		v._normal.normalize();
		v._texCoord._x = tcx + recTesselationHalf;
		v._texCoord._y = 0.f;
		pSubMesh->addVertex(v);

		v._position._x += oblique;
		v._position._y = length;
		v._normal = v._position;
		v._normal.normalize();
		v._texCoord._y = 1.f;
		pSubMesh->addVertex(v);
		tcx += recTesselation;
	}

	// indices for the main hull part
	const unsigned int nonWrappedSize = tesselation * 4;
	for (i = 0; i != nonWrappedSize; i += 2)
	{
		pSubMesh->addIndex(i + 2);
		pSubMesh->addIndex(i + 0);
		pSubMesh->addIndex(i + 1);

		pSubMesh->addIndex(i + 2);
		pSubMesh->addIndex(i + 1);
		pSubMesh->addIndex(i + 3);
	}

	// two closing quads between end and start
	pSubMesh->addIndex(0);
	pSubMesh->addIndex(i + 0);
	pSubMesh->addIndex(i + 1);

	pSubMesh->addIndex(0);
	pSubMesh->addIndex(i + 1);
	pSubMesh->addIndex(1);

	// close down
	v._position._x = 0.f;
	v._position._y = 0.f;
	v._position._z = 0.f;
	v._normal._x = 0.f;
	v._normal._y = -1.f;
	v._normal._z = 0.f;
	v._texCoord._x = 1.f;
	v._texCoord._y = 1.f;
	pSubMesh->addVertex(v);

	unsigned int index = pSubMesh->getVertexCount() - 1;

	for (i = 0; i != nonWrappedSize; i += 2)
	{
		pSubMesh->addIndex(index);
		pSubMesh->addIndex(i + 0);
		pSubMesh->addIndex(i + 2);
	}

	pSubMesh->addIndex(index);
	pSubMesh->addIndex(i + 0);
	pSubMesh->addIndex(0);

	if (closeTop)
	{
		// close top
		v._position._x = oblique;
		v._position._y = length;
		v._position._z = 0.f;
		v._normal._x = 0.f;
		v._normal._y = 1.f;
		v._normal._z = 0.f;
		v._texCoord._x = 0.f;
		v._texCoord._y = 0.f;
		pSubMesh->addVertex(v);

		index = pSubMesh->getVertexCount() - 1;

		for (i = 0; i != nonWrappedSize; i += 2)
		{
			pSubMesh->addIndex(i + 1);
			pSubMesh->addIndex(index);
			pSubMesh->addIndex(i + 3);
		}

		pSubMesh->addIndex(i + 1);
		pSubMesh->addIndex(index);
		pSubMesh->addIndex(1);
	}

	pSubMesh->setDirty();
	_meshPool[pMesh->getName()] = pMesh;
	return pMesh;
}

MeshPtr MeshManager::createConeMesh(float radius, float length, unsigned int tesselation,
	const Colour& colorTop /*= 0xffffffff*/, const Colour& colorBottom /*= 0xffffffff*/,
	float oblique /*= 0.0f*/, const wstring& materialName /*= L""*/)
{
	return createConeMesh(L"", radius, length, tesselation, colorTop, colorBottom, oblique, materialName);
}

MeshPtr MeshManager::createConeMesh(const wstring& name, float radius, float length,
	unsigned int tesselation, const Colour& colorTop /*= 0xffffffff*/,
	const Colour& colorBottom /*= 0xffffffff*/, float oblique /*= 0.0f*/,
	const wstring& materialName /*= L""*/)
{
	MeshPtr pMesh(new Mesh(name));
	SubMesh* pSubMesh = pMesh->createSubMesh(EPT_TRIANGLES);
	pSubMesh->setMaterialName(materialName);
	pSubMesh->setHardwareMappingHint(EHM_STATIC);

	const float angleStep = (VAYO_MATH_PI * 2.f) / tesselation;
	const float angleStepHalf = angleStep*0.5f;

	Vertex v;
	unsigned int i;

	v._color = colorTop;
	for (i = 0; i != tesselation; ++i)
	{
		float angle = angleStep * float(i);

		v._position._x = radius * cosf(angle);
		v._position._y = 0.f;
		v._position._z = radius * sinf(angle);
		v._normal = v._position;
		v._normal.normalize();
		pSubMesh->addVertex(v);

		angle += angleStepHalf;
		v._position._x = radius * cosf(angle);
		v._position._y = 0.f;
		v._position._z = radius * sinf(angle);
		v._normal = v._position;
		v._normal.normalize();
		pSubMesh->addVertex(v);
	}
	const unsigned int nonWrappedSize = pSubMesh->getVertexCount() - 1;

	// close top
	v._position._x = oblique;
	v._position._y = length;
	v._position._z = 0.f;
	v._normal._x = 0.f;
	v._normal._y = 1.f;
	v._normal._z = 0.f;
	pSubMesh->addVertex(v);

	unsigned int index = pSubMesh->getVertexCount() - 1;

	for (i = 0; i != nonWrappedSize; i += 1)
	{
		pSubMesh->addIndex(i + 0);
		pSubMesh->addIndex(index);
		pSubMesh->addIndex(i + 1);
	}

	pSubMesh->addIndex(i + 0);
	pSubMesh->addIndex(index);
	pSubMesh->addIndex(0);

	// close down
	v._color = colorBottom;
	v._position._x = 0.f;
	v._position._y = 0.f;
	v._position._z = 0.f;
	v._normal._x = 0.f;
	v._normal._y = -1.f;
	v._normal._z = 0.f;
	pSubMesh->addVertex(v);

	index = pSubMesh->getVertexCount() - 1;

	for (i = 0; i != nonWrappedSize; i += 1)
	{
		pSubMesh->addIndex(index);
		pSubMesh->addIndex(i + 0);
		pSubMesh->addIndex(i + 1);
	}

	pSubMesh->addIndex(index);
	pSubMesh->addIndex(i + 0);
	pSubMesh->addIndex(0);

	pSubMesh->setDirty();
	_meshPool[pMesh->getName()] = pMesh;
	return pMesh;
}

NS_VAYO3D_END
