/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 图形队列
\*************************************************************************/
#ifndef __VAYO2D_GRAPHIC_QUEUE_H__
#define __VAYO2D_GRAPHIC_QUEUE_H__

#include "Vayo2dSupport.h"
#include "math/VayoMatrix3x3.h"
NS_VAYO2D_BEGIN

enum EGraphicQueueID
{
	EGQ_BACKGROUND = 0,
	EGQ_MAP_EARLY = 5,
	EGQ_MAIN_BODY = 50,
	EGQ_MAP_LATE = 80,
	EGQ_WIRE_BOUNDING_AREA = 90,
	EGQ_OVERLAY = 95,
	EGQ_COUNT = 100
};

class _Vayo2dExport GraphicQueue
{
public:
	GraphicQueue();
	void update(Matrix3x3& curTransform, bool execTransform = true);
	void clear();
	void addGraphics(Graphics* pGraph);

private:
	typedef struct tagOrderedGraph
	{
		tagOrderedGraph(Graphics* g);
		bool operator<(const tagOrderedGraph& other) const
		{
			return Order < other.Order;
		}

		Graphics* Graph;
	private:
		unsigned int Order;
	} OrderedGraph;

private:
	Renderer*            _renderer;
	vector<Graphics*>    _basicGraphs;
	vector<OrderedGraph> _orderedGraphs;
};

class _Vayo2dExport GraphicQueueGroup
{
public:
	GraphicQueueGroup(LayerManager* relatedLayerMgr);
	~GraphicQueueGroup();
	void update();
	void clear(unsigned int queueID);
	void addGraphics(Graphics* pGraph);
	void addGraphics(Graphics* pGraph, unsigned int queueID);

private:
	GraphicQueue& getQueue(unsigned int queueID);

private:
	Renderer*     _renderer;
	LayerManager* _relatedLayerMgr;
	Matrix3x3     _currentTransform;
	GraphicQueue  _groups[EGQ_COUNT];
};

NS_VAYO2D_END

#endif // __VAYO2D_GRAPHIC_QUEUE_H__
