/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 碰撞检测
\*************************************************************************/
#ifndef __VAYO_COLLISION_H__
#define __VAYO_COLLISION_H__

#include "VayoSupport.h"
#include "VayoBitState.h"
#include "VayoVector2d.h"
#include "VayoTriangle3d.h"
#include "VayoMatrix4x4.h"
NS_VAYO_BEGIN

// 三角形容器抽象类,用于实现与三角形相关的碰撞检测,目前只支持图元类型为三角形的网格
class _VayoExport TriContainer
{
public:
	TriContainer(const wstring& name);
	virtual ~TriContainer() {}
	virtual int getTriCount() const = 0;
	virtual void getTris(Triangle3df* triangles, int arraySize, int& outTriangleCount, const Matrix4x4* transform = NULL) const = 0;
	virtual void getTris(Triangle3df* triangles, int arraySize, int& outTriangleCount, const Aabbox3df& box, const Matrix4x4* transform = NULL) const = 0;
	virtual void getTris(Triangle3df* triangles, int arraySize, int& outTriangleCount, const Line3df& line, const Matrix4x4* transform = NULL) const = 0;
	virtual MovableObject* getObjectForTri(unsigned int triangleIndex) const = 0;
	PROPERTY_R_REF(wstring, _name, Name)
};

// 一个简单的三角形容器实现类
class _VayoExport SimpleTriContainer : public TriContainer
{
public:
	SimpleTriContainer(MovableObject* obj);
	SimpleTriContainer(MovableObject* obj, const Aabbox3df& box);
	~SimpleTriContainer();
	int getTriCount() const;
	void getTris(Triangle3df* triangles, int arraySize, int& outTriangleCount, const Matrix4x4* transform = NULL) const;
	void getTris(Triangle3df* triangles, int arraySize, int& outTriangleCount, const Aabbox3df& box, const Matrix4x4* transform = NULL) const;
	void getTris(Triangle3df* triangles, int arraySize, int& outTriangleCount, const Line3df& line, const Matrix4x4* transform = NULL) const;
	MovableObject* getObjectForTri(unsigned int triangleIndex) const;

protected:
	virtual void createFromMesh(const Mesh* mesh);
	MovableObject*              _targetObj;
	mutable vector<Triangle3df> _triangles;
	mutable Aabbox3df           _boundingBox;
};

// 碰撞检测器
class _VayoExport CollisionDetector
{
public:
	CollisionDetector(SceneManager* sceneMgr);
	~CollisionDetector();
	template<typename T> T* createTriContainer(const wstring& name = L"");
	template<typename T> T* findTriContainer(const wstring& name);
	void destroyTriContainer(const wstring& name);
	void destroyTriContainer(TriContainer* triContainer);
	void destroyAllTriContainers();

public:
	// Finds the collision point of a line and lots of triangles, if there is one.
	bool getCollisionPoint(const Line3df& ray,
		TriContainer* container, Vector3df& outCollisionPoint,
		Triangle3df& outTriangle, MovableObject*& outObj);

	// Collides a moving ellipsoid with a 3d world with gravity and returns
	// the resulting new position of the ellipsoid.
	Vector3df getCollisionResultPosition(TriContainer* container,
		const Vector3df& ellipsoidPosition,
		const Vector3df& ellipsoidRadius,
		const Vector3df& ellipsoidDirectionAndSpeed,
		Triangle3df& triout,
		Vector3df& hitPosition,
		bool& outFalling,
		MovableObject*& outObj,
		float slidingSpeed = 0.0005f,
		const Vector3df& gravityDirectionAndSpeed = Vector3df::Origin);

	// Returns a 3d ray which would go through the 2d screen coodinates.
	Line3df getRayFromScreenCoordinates(const Position2di& pos, Camera* camera = NULL);
	// Calculates 2d screen position from a 3d position.
	Position2di getScreenCoordinatesFrom3DPosition(const Vector3df& pos, Camera* camera = NULL, bool useViewPort = false);
	// Returns the movable object, which is currently visible at the given
	// screen coordinates, viewed from the currently active camera.
	MovableObject* getObjectFromScreenCoordinatesAABB(const Position2di& pos, int idBitMask = ESTATE_01, SceneNode* root = NULL);
	// Returns the nearest movable object which collides with a 3d ray and
	// whose id matches a bitmask.
	MovableObject* getObjectFromRayAABB(const Line3df& ray, int idBitMask = ESTATE_01, SceneNode* root = NULL);
	// Returns the movable object, at which the overgiven camera is looking at and
	// which id matches the bitmask.
	MovableObject* getObjectFromCameraAABB(Camera* camera, int idBitMask = ESTATE_01);

	// Gets the movable object and nearest collision point for a ray based on
	// the objects' id bitmasks, bounding boxes and triangle containers.
	MovableObject* getObjectAndCollisionPointFromRay(Line3df ray, Vector3df& outCollisionPoint,
		Triangle3df& outTriangle, int idBitMask = ESTATE_01, SceneNode* collisionRootNode = NULL);

private:
	// recursive method for going through all scene nodes.
	void getPickedObjectAABB(SceneNode* root, Line3df& ray, int bits, float& outBestDistance, MovableObject*& outBestObj);
	void getPickedObjectFromAABBAndTriContainer(SceneNode* root, Line3df& ray, int bits, float& outBestDistanceSquared,
		MovableObject*& outBestObj, Vector3df& outBestCollisionPoint, Triangle3df& outBestTriangle);

	typedef struct tagCollisionData
	{
		Vector3df eRadius;

		Vector3df R3Velocity;
		Vector3df R3Position;

		Vector3df velocity;
		Vector3df normalizedVelocity;
		Vector3df basePoint;

		bool foundCollision;
		float nearestDistance;
		Vector3df intersectionPoint;

		Triangle3df intersectionTriangle;
		int triangleIndex;
		int triangleHits;

		float slidingSpeed;

		TriContainer* container;
	} CollisionData;

	// Tests the current collision data against an individual triangle.
	/**
	\param colData: the collision data.
	\param triangle: the triangle to test against.
	\return true if the triangle is hit(and is the closest hit), false otherwise. */
	bool testTriangleIntersection(CollisionData* colData, const Triangle3df& triangle);

	// recursive method for doing collision response.
	Vector3df collideEllipsoidWithWorld(TriContainer* container, const Vector3df &position,
		const Vector3df& radius, const Vector3df& velocity, float slidingSpeed,
		const Vector3df& gravity, Triangle3df& triout, Vector3df& hitPosition,
		bool& outFalling, MovableObject*& outObj);

	Vector3df collideWithWorld(int recursionDepth, CollisionData& colData, Vector3df pos, Vector3df vel);
	inline bool getLowestRoot(float a, float b, float c, float maxR, float* root);

private:
	SceneManager* _sceneMgr;
	vector<Triangle3df> _trianglesBuffer;
	map<wstring, TriContainer*> _triContainersPool;
	DISALLOW_COPY_AND_ASSIGN(CollisionDetector)
};

template<typename T>
T* CollisionDetector::createTriContainer(const wstring& name /*= L""*/)
{
	T* ret = new T(name);
	TriContainer* pTri = dynamic_cast<TriContainer*>(ret);
	if (NULL == pTri)
	{
		SAFE_DELETE(ret);
		return NULL;
	}

	TriContainer* pFindedTri = findTriContainer<TriContainer>(pTri->getName());
	if (pFindedTri)
	{
		SAFE_DELETE(pFindedTri);
	}

	_triContainersPool[pTri->getName()] = pTri;
	return ret;
}

template<typename T>
T* CollisionDetector::findTriContainer(const wstring& name)
{
	T* ret = NULL;
	map<wstring, TriContainer*>::iterator it = _triContainersPool.find(name);
	if (it != _triContainersPool.end())
		ret = dynamic_cast<T*>(it->second);
	return ret;
}

NS_VAYO_END

#endif // __VAYO_COLLISION_H__
