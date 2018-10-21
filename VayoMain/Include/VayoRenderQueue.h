/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* 渲染队列
\*************************************************************************/
#ifndef __VAYO_RENDER_QUEUE_H__
#define __VAYO_RENDER_QUEUE_H__

#include "VayoSupport.h"
#include "VayoVector3d.h"
#include "VayoMatrix4x4.h"
NS_VAYO_BEGIN

enum ERenderQueueID
{
	ERQ_BACKGROUND = 0,
	ERQ_SKIES_EARLY = 5,
	ERQ_MAIN_SOLID = 50,
	ERQ_SKIES_LATE = 80,
	ERQ_WIRE_BOUNDING_BOX = 90,
	ERQ_OVERLAY = 95,
	ERQ_COUNT = 100
};

class RenderQueue
{
public:
	RenderQueue();
	void update(Matrix4x4& curTransform, bool execTransform = true);
	void clear();
	void addRenderable(Renderable* pRend);

private:
	typedef struct tagOrderedSolid
	{
		tagOrderedSolid(Renderable* r);
		bool operator<(const tagOrderedSolid& other) const
		{
			return Prior < other.Prior;
		}

		Renderable* Rend;
	private:
		unsigned int Prior;
	} OrderedSolid;

	typedef struct tagTransparentSolid
	{
		tagTransparentSolid(Renderable* r, SceneManager* relatedSceneMgr);
		bool operator<(const tagTransparentSolid& other) const
		{
			return other.Distance < Distance;
		}

		Renderable* Rend;
	private:
		float Distance;
	} TransparentSolid;

private:
	RenderSystem*                     _renderer;
	vector<Renderable*>            _basicSolids;
	vector<OrderedSolid>         _orderedSolids;
	vector<TransparentSolid> _transparentSolids;
};

class RenderQueueGroup
{
public:
	RenderQueueGroup(SceneManager* relatedSceneMgr);
	~RenderQueueGroup();
	void update();
	void clear(unsigned int queueID);
	void addRenderable(Renderable* pRend);
	void addRenderable(Renderable* pRend, unsigned int queueID);

private:
	RenderQueue& getQueue(unsigned int queueID);

private:
	RenderSystem*        _renderer;
	SceneManager* _relatedSceneMgr;
	Matrix4x4    _currentTransform;
	RenderQueue _groups[ERQ_COUNT];
};

NS_VAYO_END

#endif // __VAYO_RENDER_QUEUE_H__
