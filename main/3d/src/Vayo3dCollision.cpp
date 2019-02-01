#include "Vayo3dCollision.h"
#include "Vayo3dMesh.h"
#include "VayoLog.h"
#include "Vayo3dSceneNode.h"
#include "Vayo3dSceneManager.h"
#include "Vayo3dMovableObject.h"
#include "Vayo3dCamera.h"
#include "Vayo3dRoot.h"
#include "Vayo3dRenderSystem.h"

NS_VAYO3D_BEGIN

int SimpleTriContainer::getTriCount() const
{
	return (int)_triangles.size();
}

TriContainer::TriContainer(const wstring& name)
	: _name(name)
{
	static unsigned short idx = 0;
	if (0 == _name.compare(L""))
	{
		std::wstringstream ss;
		ss << L"TriContainer" << idx;
		++idx;
		_name = ss.str();
	}
}

SimpleTriContainer::SimpleTriContainer(MovableObject* obj)
	: TriContainer(obj->getName())
	, _targetObj(obj)
{
	_boundingBox.reset(0.0f, 0.0f, 0.0f);
	createFromMesh(obj->getMesh().get());
}

SimpleTriContainer::SimpleTriContainer(MovableObject* obj, const Aabbox3df& box)
	: TriContainer(obj->getName())
	, _targetObj(obj)
	, _boundingBox(box)
{
	createFromMesh(obj->getMesh().get());
}

SimpleTriContainer::~SimpleTriContainer()
{
}

void SimpleTriContainer::getTris(Triangle3df* triangles, int arraySize, int& outTriangleCount,
	const Matrix4x4* transform /*= NULL*/) const
{
	unsigned int cnt = _triangles.size();
	if (cnt > (unsigned int)arraySize)
		cnt = (unsigned int)arraySize;

	Matrix4x4 mat;
	if (transform)
		mat = *transform;

	if (_targetObj && _targetObj->getParentNode())
		mat *= _targetObj->getParentNode()->getAbsTransform();

	for (unsigned int i = 0; i < cnt; ++i)
	{
		mat.transformVect(_triangles[i]._pointA, triangles[i]._pointA);
		mat.transformVect(_triangles[i]._pointB, triangles[i]._pointB);
		mat.transformVect(_triangles[i]._pointC, triangles[i]._pointC);
	}

	outTriangleCount = cnt;
}

void SimpleTriContainer::getTris(Triangle3df* triangles, int arraySize, int& outTriangleCount,
	const Aabbox3df& box, const Matrix4x4* transform /*= NULL*/) const
{
	Matrix4x4 mat;
	Aabbox3df tBox(box);

	if (_targetObj && _targetObj->getParentNode())
	{
		_targetObj->getParentNode()->getAbsTransform().getInverse(mat);
		mat.transformBox(tBox);
	}
	if (transform)
		mat = *transform;
	else
		mat.makeIdentity();

	if (_targetObj->getParentNode())
		mat *= _targetObj->getParentNode()->getAbsTransform();

	outTriangleCount = 0;

	if (!tBox.intersectsWithBox(_boundingBox))
		return;

	int triangleCount = 0;
	const unsigned int cnt = _triangles.size();
	for (unsigned int i = 0; i < cnt; ++i)
	{
		if (triangleCount == arraySize)
			break;

		if (_triangles[i].isTotalOutsideBox(tBox))
			continue;

		triangles[triangleCount] = _triangles[i];
		mat.transformVect(triangles[triangleCount]._pointA);
		mat.transformVect(triangles[triangleCount]._pointB);
		mat.transformVect(triangles[triangleCount]._pointC);

		++triangleCount;
	}

	outTriangleCount = triangleCount;
}

void SimpleTriContainer::getTris(Triangle3df* triangles, int arraySize, int& outTriangleCount,
	const Line3df& line, const Matrix4x4* transform /*= NULL*/) const
{
	Aabbox3df box(line._start);
	box.addInternalPoint(line._end);
	getTris(triangles, arraySize, outTriangleCount, box, transform);
}

MovableObject* SimpleTriContainer::getObjectForTri(unsigned int triangleIndex) const
{
	return _targetObj;
}

void SimpleTriContainer::createFromMesh(const Mesh* mesh)
{
	if (NULL == mesh)
		return;

	const unsigned int cnt = mesh->getSubMeshCount();
	unsigned int totalFaceCount = 0, faceIndex = 0;

	for (unsigned int i = 0; i < cnt; ++i)
	{
		const SubMesh* pSubMesh = mesh->getSubMesh(i);
		EPrimitiveType primType = pSubMesh->getPrimType();
		if (primType != EPT_TRIANGLES && primType != EPT_TRIANGLE_STRIP && primType != EPT_TRIANGLE_FAN)
		{
			Log::wprint(ELL_WARNING, L"Mesh:[%s]包含图元类型不是三角形的子网格", mesh->getName().c_str());
			continue;
		}

		totalFaceCount += pSubMesh->getPrimCount();
	}

	_triangles.clear();
	_triangles.resize(totalFaceCount);
	_boundingBox.reset(0.0f, 0.0f, 0.0f);

	for (unsigned int i = 0; i < cnt && faceIndex < totalFaceCount; ++i)
	{
		const SubMesh* buf = mesh->getSubMesh(i);
		EPrimitiveType primType = buf->getPrimType();
		if (primType != EPT_TRIANGLES && primType != EPT_TRIANGLE_STRIP && primType != EPT_TRIANGLE_FAN)
			continue;

		const unsigned int idxCnt = buf->getIndexCount();
		const unsigned int* const indices = buf->getIndices();
		if (0 == idxCnt || NULL == indices)
			continue;

		switch (primType)
		{
		case EPT_TRIANGLES:
			for (unsigned int j = 0; j < idxCnt; j += 3)
			{
				_triangles[faceIndex] = Triangle3df(
					buf->getVertices()[indices[j + 0]]._position,
					buf->getVertices()[indices[j + 1]]._position,
					buf->getVertices()[indices[j + 2]]._position);
				const Triangle3df& tri = _triangles[faceIndex++];
				_boundingBox.addInternalPoint(tri._pointA);
				_boundingBox.addInternalPoint(tri._pointB);
				_boundingBox.addInternalPoint(tri._pointC);
			}
			break;
		case EPT_TRIANGLE_STRIP:
			for (unsigned int j = 0; j < idxCnt - 2; ++j)
			{
				_triangles[faceIndex] = Triangle3df(
					buf->getVertices()[indices[j + 0]]._position,
					buf->getVertices()[indices[j + 1]]._position,
					buf->getVertices()[indices[j + 2]]._position);
				const Triangle3df& tri = _triangles[faceIndex++];
				_boundingBox.addInternalPoint(tri._pointA);
				_boundingBox.addInternalPoint(tri._pointB);
				_boundingBox.addInternalPoint(tri._pointC);
			}
			break;
		case EPT_TRIANGLE_FAN:
			for (unsigned int j = 1; j < idxCnt - 1; ++j)
			{
				_triangles[faceIndex] = Triangle3df(
					buf->getVertices()[indices[0]]._position,
					buf->getVertices()[indices[j]]._position,
					buf->getVertices()[indices[j + 1]]._position);
				const Triangle3df& tri = _triangles[faceIndex++];
				_boundingBox.addInternalPoint(tri._pointA);
				_boundingBox.addInternalPoint(tri._pointB);
				_boundingBox.addInternalPoint(tri._pointC);
			}
			break;
		default:
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
CollisionDetector::CollisionDetector(SceneManager* sceneMgr)
	: _sceneMgr(sceneMgr)
{
}

CollisionDetector::~CollisionDetector()
{
	destroyAllTriContainers();
}

void CollisionDetector::destroyTriContainer(const wstring& name)
{
	map<wstring, TriContainer*>::iterator it = _triContainersPool.find(name);
	if (it != _triContainersPool.end())
	{
		delete it->second;
		_triContainersPool.erase(it);
	}
}

void CollisionDetector::destroyTriContainer(TriContainer* triContainer)
{
	if (triContainer)
		destroyTriContainer(triContainer->getName());
}

void CollisionDetector::destroyAllTriContainers()
{
	map<wstring, TriContainer*>::iterator it = _triContainersPool.begin();
	for (; it != _triContainersPool.end(); ++it)
		delete it->second;
	_triContainersPool.clear();
}

bool CollisionDetector::getCollisionPoint(const Line3df& ray, TriContainer* container,
	Vector3df& outCollisionPoint, Triangle3df& outTriangle, MovableObject*& outObj)
{
	if (!container)
		return false;

	int totalcnt = container->getTriCount();
	if (totalcnt <= 0)
		return false;

	_trianglesBuffer.clear();
	_trianglesBuffer.resize(totalcnt);

	int cnt = 0;
	container->getTris(&_trianglesBuffer[0], totalcnt, cnt, ray);

	const Vector3df linevect = ray.getVector().normalize();
	Vector3df intersection;
	float nearest = FLT_MAX;
	bool found = false;
	const float raylength = ray.getLengthSQ();

	const float minX = min_(ray._start._x, ray._end._x);
	const float maxX = max_(ray._start._x, ray._end._x);
	const float minY = min_(ray._start._y, ray._end._y);
	const float maxY = max_(ray._start._y, ray._end._y);
	const float minZ = min_(ray._start._z, ray._end._z);
	const float maxZ = max_(ray._start._z, ray._end._z);

	for (int i = 0; i < cnt; ++i)
	{
		const Triangle3df& triangle = _trianglesBuffer[i];

		if (minX > triangle._pointA._x && minX > triangle._pointB._x && minX > triangle._pointC._x)
			continue;
		if (maxX < triangle._pointA._x && maxX < triangle._pointB._x && maxX < triangle._pointC._x)
			continue;
		if (minY > triangle._pointA._y && minY > triangle._pointB._y && minY > triangle._pointC._y)
			continue;
		if (maxY < triangle._pointA._y && maxY < triangle._pointB._y && maxY < triangle._pointC._y)
			continue;
		if (minZ > triangle._pointA._z && minZ > triangle._pointB._z && minZ > triangle._pointC._z)
			continue;
		if (maxZ < triangle._pointA._z && maxZ < triangle._pointB._z && maxZ < triangle._pointC._z)
			continue;

		if (triangle.getIntersectionWithLine(ray._start, linevect, intersection))
		{
			const float tmp = intersection.getDistanceFromSQ(ray._start);
			const float tmp2 = intersection.getDistanceFromSQ(ray._end);

			if (tmp < raylength && tmp2 < raylength && tmp < nearest)
			{
				nearest = tmp;
				outTriangle = triangle;
				outCollisionPoint = intersection;
				outObj = container->getObjectForTri(i);
				found = true;
			}
		}
	}

	return found;
}

Vector3df CollisionDetector::getCollisionResultPosition(TriContainer* container,
	const Vector3df& ellipsoidPosition, const Vector3df& ellipsoidRadius,
	const Vector3df& ellipsoidDirectionAndSpeed, Triangle3df& triout,
	Vector3df& hitPosition, bool& outFalling, MovableObject*& outObj,
	float slidingSpeed /*= 0.0005f*/, const Vector3df& gravityDirectionAndSpeed /*= Vector3df::Origin*/)
{
	return collideEllipsoidWithWorld(container, ellipsoidPosition,
		ellipsoidRadius, ellipsoidDirectionAndSpeed, slidingSpeed,
		gravityDirectionAndSpeed, triout, hitPosition, outFalling, outObj);
}

Line3df CollisionDetector::getRayFromScreenCoordinates(const Position2di& pos, Camera* camera /*= NULL*/)
{
	Line3df ln(0, 0, 0, 0, 0, 0);

	if (!_sceneMgr)
		return ln;

	if (!camera)
		camera = _sceneMgr->getActiveCamera();

	if (!camera)
		return ln;

	const Frustum& furstum = camera->getViewFrustum();

	Vector3df farLeftUp = furstum.getFarLeftUp();
	Vector3df lefttoright = furstum.getFarRightUp() - farLeftUp;
	Vector3df uptodown = furstum.getFarLeftDown() - farLeftUp;

	const Recti& viewPort = Root::getSingleton().getActiveRenderer()->getViewPort();
	Dimension2di screenSize(viewPort.getWidth(), viewPort.getHeight());

	float dx = pos._x / (float)screenSize._width;
	float dy = pos._y / (float)screenSize._height;

	if (camera->isOrthogonal())
		ln._start = furstum._cameraPosition + (lefttoright * (dx - 0.5f)) + (uptodown * (dy - 0.5f));
	else
		ln._start = furstum._cameraPosition;

	ln._end = farLeftUp + (lefttoright * dx) + (uptodown * dy);

	return ln;
}

Position2di CollisionDetector::getScreenCoordinatesFrom3DPosition(const Vector3df& pos,
	Camera* camera /*= NULL*/, bool useViewPort /*= false*/)
{
	RenderSystem* renderer = Root::getSingleton().getActiveRenderer();
	if (!_sceneMgr || !renderer)
		return Position2di(-1000, -1000);

	if (!camera)
		camera = _sceneMgr->getActiveCamera();

	if (!camera)
		return Position2di(-1000, -1000);

	Dimension2di dim;
	if (useViewPort)
		dim.set(renderer->getViewPort().getWidth(), renderer->getViewPort().getHeight());
	else
		dim = (renderer->getCurRenderTargetSize());

	dim._width /= 2;
	dim._height /= 2;

	Matrix4x4 trans = camera->getProj();
	trans *= camera->getView();

	float transformedPos[4] = { pos._x, pos._y, pos._z, 1.0f };
	trans.multiplyWith1x4Matrix(transformedPos);

	if (transformedPos[3] < 0.0f)
		return Position2di(-10000, -10000);

	const float zDiv = isZero(transformedPos[3]) ? 1.0f : 1.0f / transformedPos[3];

	return Position2di(
		dim._width + (int)round_(dim._width * (transformedPos[0] * zDiv)),
		dim._height - (int)round_(dim._height * (transformedPos[1] * zDiv)));
}

MovableObject* CollisionDetector::getObjectFromScreenCoordinatesAABB(const Position2di& pos,
	int idBitMask /*= ESTATE_01*/, SceneNode* root /*= NULL*/)
{
	const Line3df ln = getRayFromScreenCoordinates(pos, 0);
	if (ln._start == ln._end)
		return NULL;
	return getObjectFromRayAABB(ln, idBitMask, root);
}

MovableObject* CollisionDetector::getObjectFromRayAABB(const Line3df& ray, int idBitMask /*= ESTATE_01*/,  SceneNode* root /*= NULL*/)
{
	MovableObject* best = 0;
	float dist = FLT_MAX;
	Line3df truncatableRay(ray);
	getPickedObjectAABB((root == NULL) ? _sceneMgr->getRootSceneNode() : root, truncatableRay, idBitMask, dist, best);
	return best;
}

MovableObject* CollisionDetector::getObjectFromCameraAABB(Camera* camera, int idBitMask /*= ESTATE_01*/)
{
	if (!camera)
		return NULL;
	Vector3df theStart, theEnd;
	camera->getWorldPos(theStart);
	camera->getWorldLook(theEnd);
	theEnd = theStart + (theEnd * camera->getFarZ());
	return getObjectFromRayAABB(Line3df(theStart, theEnd), idBitMask);
}

MovableObject* CollisionDetector::getObjectAndCollisionPointFromRay(Line3df ray, Vector3df& outCollisionPoint,
	Triangle3df& outTriangle, int idBitMask /*= ESTATE_01*/, SceneNode* collisionRootNode /*= NULL*/)
{
	MovableObject* bestObj = 0;
	float bestDistanceSquared = FLT_MAX;

	if (NULL == collisionRootNode)
		collisionRootNode = _sceneMgr->getRootSceneNode();

	// We don't try to do anything too clever, like sorting the candidate
	// nodes by distance to bounding-box. In the example below, we could do the
	// triangle collision check with node A first, but we'd have to check node B
	// anyway, as the actual collision point could be (and is) closer than the
	// collision point in node A.
	//
	//    ray end
	//       |
	//   AAAAAAAAAA
	//   A   |
	//   A   |  B
	//   A   |  B
	//   A  BBBBB
	//   A   |
	//   A   |
	//       |
	//       |
	//    ray start
	//
	// We therefore have to do a full BB and triangle collision on every scene
	// node in order to find the nearest collision point, so sorting them by
	// bounding box would be pointless.

	getPickedObjectFromAABBAndTriContainer(collisionRootNode, ray, idBitMask,
		bestDistanceSquared, bestObj, outCollisionPoint, outTriangle);
	return bestObj;
}

void CollisionDetector::getPickedObjectAABB(SceneNode* root, Line3df& ray, int bits, float& outBestDistance, MovableObject*& outBestObj)
{
	const list<Node*>& children = root->getChildren();
	const Vector3df rayVector = ray.getVector().normalize();

	list<Node*>::const_iterator it = children.cbegin();
	for (; it != children.cend(); ++it)
	{
		SceneNode* pNode = dynamic_cast<SceneNode*>(*it);
		if (NULL == pNode || !pNode->isCanVisit())
			continue;

		const map<wstring, MovableObject*>& objs = pNode->getObjects();
		map<wstring, MovableObject*>::const_iterator cit = objs.cbegin();
		for (; cit != objs.cend(); ++cit)
		{
			MovableObject* current = cit->second;
			if (current && current->isVisible())
			{
				if (bits != 0 && (current->getCollideMask().checkState(bits)))
				{
					// get world to object space transform
					Matrix4x4 worldToObject;
					if (!pNode->getAbsTransform().getInverse(worldToObject))
						continue;

					// transform vector from world space to object space
					Line3df objectRay(ray);
					worldToObject.transformVect(objectRay._start);
					worldToObject.transformVect(objectRay._end);

					const Aabbox3df& objectBox = current->getLocalAABB();

					// Do the initial intersection test in object space, since the
					// object space box test is more accurate.
					if (objectBox.isPointInside(objectRay._start))
					{
						// use fast bbox intersection to find distance to hitpoint
						// algorithm from Kay et al., code from gamedev.net
						const Vector3df dir = (objectRay._end - objectRay._start).normalize();
						const Vector3df minDist = (objectBox._minEdge - objectRay._start) / dir;
						const Vector3df maxDist = (objectBox._maxEdge - objectRay._start) / dir;
						const Vector3df realMin(min_(minDist._x, maxDist._x), min_(minDist._y, maxDist._y), min_(minDist._z, maxDist._z));
						const Vector3df realMax(max_(minDist._x, maxDist._x), max_(minDist._y, maxDist._y), max_(minDist._z, maxDist._z));

						const float minmax = min_(realMax._x, realMax._y, realMax._z);
						// nearest distance to intersection
						const float maxmin = max_(realMin._x, realMin._y, realMin._z);

						const float toIntersectionSq = (maxmin>0 ? maxmin*maxmin : minmax*minmax);
						if (toIntersectionSq < outBestDistance)
						{
							outBestDistance = toIntersectionSq;
							outBestObj = current;

							// And we can truncate the ray to stop us hitting further nodes.
							ray._end = ray._start + (rayVector * sqrtf(toIntersectionSq));
						}
					}
					else if (objectBox.intersectsWithLine(objectRay))
					{
						// Now transform into world space, since we need to use world space
						// scales and distances.
						Aabbox3df worldBox(objectBox);
						pNode->getAbsTransform().transformBox(worldBox);

						Vector3df edges[8];
						worldBox.getEdges(edges);

						/* We need to check against each of 6 faces, composed of these corners:
						  /3--------/7
						 / |       / |
						/  |      /  |
						1---------5  |
						|  2- - - | -6
						| /       |  /
						|/        | /
						0---------4/

						Note that we define them as opposite pairs of faces.
						*/
						static const int faceEdges[6][3] =
						{
							{ 0, 1, 5 }, // Front
							{ 6, 7, 3 }, // Back
							{ 2, 3, 1 }, // Left
							{ 4, 5, 7 }, // Right
							{ 1, 3, 7 }, // Top
							{ 2, 0, 4 }  // Bottom
						};

						Vector3df intersection;
						Plane3df facePlane;
						float bestDistToBoxBorder = FLT_MAX;
						float bestToIntersectionSq = FLT_MAX;

						for (int face = 0; face < 6; ++face)
						{
							facePlane.setPlane(edges[faceEdges[face][0]], edges[faceEdges[face][1]], edges[faceEdges[face][2]]);

							// Only consider lines that might be entering through this face, since we
							// already know that the start point is outside the box.
							if (facePlane.classifyPointRelation(ray._start) != EIR_FRONT)
								continue;

							// Don't bother using a limited ray, since we already know that it should be long
							// enough to intersect with the box.
							if (facePlane.getIntersectionWithLine(ray._start, rayVector, intersection))
							{
								const float toIntersectionSq = ray._start.getDistanceFromSQ(intersection);
								if (toIntersectionSq < outBestDistance)
								{
									// We have to check that the intersection with this plane is actually
									// on the box, so need to go back to object space again.
									worldToObject.transformVect(intersection);

									// find the closest point on the box borders. Have to do this as exact checks will fail due to floating point problems.
									float distToBorder = max_(min_(abs_(objectBox._minEdge._x - intersection._x), abs_(objectBox._maxEdge._x - intersection._x)),
										min_(abs_(objectBox._minEdge._y - intersection._y), abs_(objectBox._maxEdge._y - intersection._y)),
										min_(abs_(objectBox._minEdge._z - intersection._z), abs_(objectBox._maxEdge._z - intersection._z)));
									if (distToBorder < bestDistToBoxBorder)
									{
										bestDistToBoxBorder = distToBorder;
										bestToIntersectionSq = toIntersectionSq;
									}
								}
							}

							// If the ray could be entering through the first face of a pair, then it can't
							// also be entering through the opposite face, and so we can skip that face.
							if (!(face & 0x01))
								++face;
						}

						if (bestDistToBoxBorder < FLT_MAX)
						{
							outBestDistance = bestToIntersectionSq;
							outBestObj = current;

							// If we got a hit, we can now truncate the ray to stop us hitting further nodes.
							ray._end = ray._start + (rayVector * sqrtf(outBestDistance));
						}
					}
				}
			}
		}

		// Only check the children if this node is visible.
		getPickedObjectAABB(pNode, ray, bits, outBestDistance, outBestObj);
	}
}

void CollisionDetector::getPickedObjectFromAABBAndTriContainer(SceneNode* root, Line3df& ray,
	int bits, float& outBestDistanceSquared, MovableObject*& outBestObj,
	Vector3df& outBestCollisionPoint,Triangle3df& outBestTriangle)
{
	const list<Node*>& children = root->getChildren();
	list<Node*>::const_iterator it = children.cbegin();
	for (; it != children.end(); ++it)
	{
		SceneNode* pNode = dynamic_cast<SceneNode*>(*it);
		if (NULL == pNode || !pNode->isCanVisit())
			continue;

		const map<wstring, MovableObject*>& objs = pNode->getObjects();
		map<wstring, MovableObject*>::const_iterator cit = objs.cbegin();
		for (; cit != objs.cend(); ++cit)
		{
			MovableObject* current = cit->second;
			TriContainer * container = current->getTriContainer();

			if (container && current->isVisible() &&
				(bits != 0 && (current->getCollideMask().checkState(bits))))
			{
				// get world to object space transform
				Matrix4x4 mat;
				if (!pNode->getAbsTransform().getInverse(mat))
					continue;

				// transform vector from world space to object space
				Line3df line(ray);
				mat.transformVect(line._start);
				mat.transformVect(line._end);

				const Aabbox3df& box = current->getLocalAABB();

				Vector3df candidateCollisionPoint;
				Triangle3df candidateTriangle;

				// do intersection test in object space
				MovableObject* hitObject = NULL;
				if (box.intersectsWithLine(line) &&
					getCollisionPoint(ray, container, candidateCollisionPoint, candidateTriangle, hitObject))
				{
					const float distanceSquared = (candidateCollisionPoint - ray._start).getLengthSQ();

					if (distanceSquared < outBestDistanceSquared)
					{
						outBestDistanceSquared = distanceSquared;
						outBestObj = current;
						outBestCollisionPoint = candidateCollisionPoint;
						outBestTriangle = candidateTriangle;
						const Vector3df rayVector = ray.getVector().normalize();
						ray._end = ray._start + (rayVector * sqrtf(distanceSquared));
					}
				}
			}

			getPickedObjectFromAABBAndTriContainer(pNode, ray, bits, outBestDistanceSquared, outBestObj, outBestCollisionPoint, outBestTriangle);
		}
	}
}

bool CollisionDetector::testTriangleIntersection(CollisionData* colData, const Triangle3df& triangle)
{
	const Plane3df trianglePlane = triangle.getPlane();

	// only check front facing polygons
	if (!trianglePlane.isFrontFacing(colData->normalizedVelocity))
		return false;

	// get interval of plane intersection

	float t1, t0;
	bool embeddedInPlane = false;

	// calculate signed distance from sphere position to triangle plane
	float signedDistToTrianglePlane = trianglePlane.getDistanceTo(
		colData->basePoint);

	float normalDotVelocity =
		trianglePlane._normal.dotProduct(colData->velocity);

	if (isZero(normalDotVelocity))
	{
		// sphere is traveling parallel to plane

		if (fabs(signedDistToTrianglePlane) >= 1.0f)
			return false; // no collision possible
		else
		{
			// sphere is embedded in plane
			embeddedInPlane = true;
			t0 = 0.0;
			t1 = 1.0;
		}
	}
	else
	{
		normalDotVelocity = 1.0f / normalDotVelocity;

		// N.D is not 0. Calculate intersection interval
		t0 = (-1.f - signedDistToTrianglePlane) * normalDotVelocity;
		t1 = (1.f - signedDistToTrianglePlane) * normalDotVelocity;

		// Swap so t0 < t1
		if (t0 > t1) { float tmp = t1; t1 = t0; t0 = tmp; }

		// check if at least one value is within the range
		if (t0 > 1.0f || t1 < 0.0f)
			return false; // both t values are outside 1 and 0, no collision possible

		// clamp to 0 and 1
		t0 = clamp_(t0, 0.f, 1.f);
		t1 = clamp_(t1, 0.f, 1.f);
	}

	// at this point we have t0 and t1, if there is any intersection, it
	// is between this interval
	Vector3df collisionPoint;
	bool foundCollision = false;
	float t = 1.0f;

	// first check the easy case: Collision within the triangle;
	// if this happens, it must be at t0 and this is when the sphere
	// rests on the front side of the triangle plane. This can only happen
	// if the sphere is not embedded in the triangle plane.

	if (!embeddedInPlane)
	{
		Vector3df planeIntersectionPoint =
			(colData->basePoint - trianglePlane._normal)
			+ (colData->velocity * t0);

		if (triangle.isPointInside(planeIntersectionPoint))
		{
			foundCollision = true;
			t = t0;
			collisionPoint = planeIntersectionPoint;
		}
	}

	// if we havent found a collision already we will have to sweep
	// the sphere against points and edges of the triangle. Note: A
	// collision inside the triangle will always happen before a
	// vertex or edge collision.

	if (!foundCollision)
	{
		Vector3df velocity = colData->velocity;
		Vector3df base = colData->basePoint;

		float velocitySqaredLength = velocity.getLengthSQ();
		float a, b, c;
		float newT;

		// for each edge or vertex a quadratic equation has to be solved:
		// a*t^2 + b*t + c = 0. We calculate a,b, and c for each test.

		// check against points
		a = velocitySqaredLength;

		// p1
		b = 2.0f * (velocity.dotProduct(base - triangle._pointA));
		c = (triangle._pointA - base).getLengthSQ() - 1.f;
		if (getLowestRoot(a, b, c, t, &newT))
		{
			t = newT;
			foundCollision = true;
			collisionPoint = triangle._pointA;
		}

		// p2
		if (!foundCollision)
		{
			b = 2.0f * (velocity.dotProduct(base - triangle._pointB));
			c = (triangle._pointB - base).getLengthSQ() - 1.f;
			if (getLowestRoot(a, b, c, t, &newT))
			{
				t = newT;
				foundCollision = true;
				collisionPoint = triangle._pointB;
			}
		}

		// p3
		if (!foundCollision)
		{
			b = 2.0f * (velocity.dotProduct(base - triangle._pointC));
			c = (triangle._pointC - base).getLengthSQ() - 1.f;
			if (getLowestRoot(a, b, c, t, &newT))
			{
				t = newT;
				foundCollision = true;
				collisionPoint = triangle._pointC;
			}
		}

		// check against edges:

		// p1 --- p2
		Vector3df edge = triangle._pointB - triangle._pointA;
		Vector3df baseToVertex = triangle._pointA - base;
		float edgeSqaredLength = edge.getLengthSQ();
		float edgeDotVelocity = edge.dotProduct(velocity);
		float edgeDotBaseToVertex = edge.dotProduct(baseToVertex);

		// calculate parameters for equation
		a = edgeSqaredLength* -velocitySqaredLength +
			edgeDotVelocity*edgeDotVelocity;
		b = edgeSqaredLength* (2.f *velocity.dotProduct(baseToVertex)) -
			2.0f*edgeDotVelocity*edgeDotBaseToVertex;
		c = edgeSqaredLength* (1.f - baseToVertex.getLengthSQ()) +
			edgeDotBaseToVertex*edgeDotBaseToVertex;

		// does the swept sphere collide against infinite edge?
		if (getLowestRoot(a, b, c, t, &newT))
		{
			float f = (edgeDotVelocity*newT - edgeDotBaseToVertex) / edgeSqaredLength;
			if (f >= 0.0f && f <= 1.0f)
			{
				// intersection took place within segment
				t = newT;
				foundCollision = true;
				collisionPoint = triangle._pointA + (edge*f);
			}
		}

		// p2 --- p3
		edge = triangle._pointC - triangle._pointB;
		baseToVertex = triangle._pointB - base;
		edgeSqaredLength = edge.getLengthSQ();
		edgeDotVelocity = edge.dotProduct(velocity);
		edgeDotBaseToVertex = edge.dotProduct(baseToVertex);

		// calculate parameters for equation
		a = edgeSqaredLength* -velocitySqaredLength +
			edgeDotVelocity*edgeDotVelocity;
		b = edgeSqaredLength* (2 * velocity.dotProduct(baseToVertex)) -
			2.0f*edgeDotVelocity*edgeDotBaseToVertex;
		c = edgeSqaredLength* (1 - baseToVertex.getLengthSQ()) +
			edgeDotBaseToVertex*edgeDotBaseToVertex;

		// does the swept sphere collide against infinite edge?
		if (getLowestRoot(a, b, c, t, &newT))
		{
			float f = (edgeDotVelocity*newT - edgeDotBaseToVertex) /
				edgeSqaredLength;
			if (f >= 0.0f && f <= 1.0f)
			{
				// intersection took place within segment
				t = newT;
				foundCollision = true;
				collisionPoint = triangle._pointB + (edge*f);
			}
		}

		// p3 --- p1
		edge = triangle._pointA - triangle._pointC;
		baseToVertex = triangle._pointC - base;
		edgeSqaredLength = edge.getLengthSQ();
		edgeDotVelocity = edge.dotProduct(velocity);
		edgeDotBaseToVertex = edge.dotProduct(baseToVertex);

		// calculate parameters for equation
		a = edgeSqaredLength* -velocitySqaredLength +
			edgeDotVelocity*edgeDotVelocity;
		b = edgeSqaredLength* (2 * velocity.dotProduct(baseToVertex)) -
			2.0f*edgeDotVelocity*edgeDotBaseToVertex;
		c = edgeSqaredLength* (1 - baseToVertex.getLengthSQ()) +
			edgeDotBaseToVertex*edgeDotBaseToVertex;

		// does the swept sphere collide against infinite edge?
		if (getLowestRoot(a, b, c, t, &newT))
		{
			float f = (edgeDotVelocity*newT - edgeDotBaseToVertex) /
				edgeSqaredLength;
			if (f >= 0.0f && f <= 1.0f)
			{
				// intersection took place within segment
				t = newT;
				foundCollision = true;
				collisionPoint = triangle._pointC + (edge*f);
			}
		}
	} // end no collision found

	// set result:
	if (foundCollision)
	{
		// distance to collision is t
		float distToCollision = t*colData->velocity.getLength();

		// does this triangle qualify for closest hit?
		if (!colData->foundCollision ||
			distToCollision < colData->nearestDistance)
		{
			colData->nearestDistance = distToCollision;
			colData->intersectionPoint = collisionPoint;
			colData->foundCollision = true;
			colData->intersectionTriangle = triangle;
			++colData->triangleHits;
			return true;
		}
	} // end found collision

	return false;
}

Vector3df CollisionDetector::collideEllipsoidWithWorld(TriContainer* container, const Vector3df &position,
	const Vector3df& radius, const Vector3df& velocity, float slidingSpeed, const Vector3df& gravity,
	Triangle3df& triout, Vector3df& hitPosition, bool& outFalling, MovableObject*& outObj)
{
	if (!container || isZero(radius._x) || isZero(radius._y) || isZero(radius._z))
		return position;

	// This code is based on the paper "Improved Collision detection and Response"
	// by Kasper Fauerby, but some parts are modified.

	CollisionData colData;
	colData.R3Position = position;
	colData.R3Velocity = velocity;
	colData.eRadius = radius;
	colData.nearestDistance = FLT_MAX;
	colData.container = container;
	colData.slidingSpeed = slidingSpeed;
	colData.triangleHits = 0;
	colData.triangleIndex = -1;

	Vector3df eSpacePosition = colData.R3Position / colData.eRadius;
	Vector3df eSpaceVelocity = colData.R3Velocity / colData.eRadius;

	// iterate until we have our final position

	Vector3df finalPos = collideWithWorld(0, colData, eSpacePosition, eSpaceVelocity);
	outFalling = false;

	// add gravity

	if (gravity != Vector3df::Origin)
	{
		colData.R3Position = finalPos * colData.eRadius;
		colData.R3Velocity = gravity;
		colData.triangleHits = 0;

		eSpaceVelocity = gravity / colData.eRadius;

		finalPos = collideWithWorld(0, colData,
			finalPos, eSpaceVelocity);

		outFalling = (colData.triangleHits == 0);
	}

	if (colData.triangleHits)
	{
		triout = colData.intersectionTriangle;
		triout._pointA *= colData.eRadius;
		triout._pointB *= colData.eRadius;
		triout._pointC *= colData.eRadius;
		outObj = container->getObjectForTri(colData.triangleIndex);
	}

	finalPos *= colData.eRadius;
	hitPosition = colData.intersectionPoint * colData.eRadius;
	return finalPos;
}

Vector3df CollisionDetector::collideWithWorld(int recursionDepth, CollisionData& colData, Vector3df pos, Vector3df vel)
{
	float veryCloseDistance = colData.slidingSpeed;

	if (recursionDepth > 5)
		return pos;

	colData.velocity = vel;
	colData.normalizedVelocity = vel;
	colData.normalizedVelocity.normalize();
	colData.basePoint = pos;
	colData.foundCollision = false;
	colData.nearestDistance = FLT_MAX;

	//------------------ collide with world

	// get all triangles with which we might collide
	Aabbox3df box(colData.R3Position);
	box.addInternalPoint(colData.R3Position + colData.R3Velocity);
	box._minEdge -= colData.eRadius;
	box._maxEdge += colData.eRadius;

	int totalTriangleCnt = colData.container->getTriCount();
	_trianglesBuffer.clear();
	_trianglesBuffer.resize(totalTriangleCnt);

	Matrix4x4 scaleMatrix;
	scaleMatrix.setScale(Vector3df(1.0f / colData.eRadius._x, 1.0f / colData.eRadius._y, 1.0f / colData.eRadius._z));
	int triangleCnt = 0;
	colData.container->getTris(&_trianglesBuffer[0], totalTriangleCnt, triangleCnt, box, &scaleMatrix);

	for (int i = 0; i < triangleCnt; ++i)
		if (testTriangleIntersection(&colData, _trianglesBuffer[i]))
			colData.triangleIndex = i;

	//---------------- end collide with world

	if (!colData.foundCollision)
		return pos + vel;

	// original destination point
	const Vector3df destinationPoint = pos + vel;
	Vector3df newBasePoint = pos;

	// only update if we are not already very close
	// and if so only move very close to intersection, not to the
	// exact point
	if (colData.nearestDistance >= veryCloseDistance)
	{
		Vector3df v = vel;
		v.setLength(colData.nearestDistance - veryCloseDistance);
		newBasePoint = colData.basePoint + v;

		v.normalize();
		colData.intersectionPoint -= (v * veryCloseDistance);
	}

	// calculate sliding plane

	const Vector3df slidePlaneOrigin = colData.intersectionPoint;
	const Vector3df slidePlaneNormal = (newBasePoint - colData.intersectionPoint).normalize();
	Plane3df slidingPlane(slidePlaneOrigin, slidePlaneNormal);
	Vector3df newDestinationPoint = destinationPoint - (slidePlaneNormal * slidingPlane.getDistanceTo(destinationPoint));

	// generate slide vector

	const Vector3df newVelocityVector = newDestinationPoint -
		colData.intersectionPoint;

	if (newVelocityVector.getLength() < veryCloseDistance)
		return newBasePoint;

	return collideWithWorld(recursionDepth + 1, colData, newBasePoint, newVelocityVector);
}

bool CollisionDetector::getLowestRoot(float a, float b, float c, float maxR, float* root)
{
	// check if solution exists
	const float determinant = b*b - 4.0f*a*c;

	// if determinant is negative, no solution
	if (determinant < 0.0f || a == 0.0f)
		return false;

	// calculate two roots: (if det==0 then x1==x2
	// but lets disregard that slight optimization)

	const float sqrtD = sqrtf(determinant);
	const float invDA = 1.0f / (2 * a);
	float r1 = (-b - sqrtD) * invDA;
	float r2 = (-b + sqrtD) * invDA;

	// sort so x1 <= x2
	if (r1 > r2)
		std::swap(r1, r2);

	// get lowest root
	if (r1 > 0 && r1 < maxR)
	{
		*root = r1;
		return true;
	}

	// its possible that we want x2, this can happen if x1 < 0
	if (r2 > 0 && r2 < maxR)
	{
		*root = r2;
		return true;
	}

	return false;
}

NS_VAYO3D_END
