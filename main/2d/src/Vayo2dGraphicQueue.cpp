#include "Vayo2dGraphicQueue.h"
#include "Vayo2dPivot.h"
#include "Vayo2dGraphics.h"
#include "Vayo2dRenderer.h"

NS_VAYO2D_BEGIN

void GraphicQueue::clear()
{
	_basicGraphs.clear();
	_orderedGraphs.clear();
}

GraphicQueue::GraphicQueue()
	: _renderer(Pivot::getSingleton().getActiveRenderer())
{
}

void GraphicQueue::update(Matrix3x3& curTransform, bool execTransform /*= true*/)
{
	Graphics* pCurrentGraphics = NULL;
	Matrix3x3 tempMat;

	if (_orderedGraphs.size() > 0)
	{
		std::sort(_orderedGraphs.begin(), _orderedGraphs.end());
		vector<OrderedGraph>::const_iterator cit = _orderedGraphs.cbegin();
		for (; cit != _orderedGraphs.cend(); ++cit)
		{
			pCurrentGraphics = (*cit).Graph;

			if (execTransform)
			{
				pCurrentGraphics->getWorldTransform(tempMat);
				if (tempMat != curTransform)
				{
					curTransform = tempMat;
					_renderer->setTransform(ETK_WORLD, curTransform);
				}
			}

			pCurrentGraphics->render();
		}
		_orderedGraphs.clear();
	}

	if (_basicGraphs.size() > 0)
	{
		vector<Graphics*>::const_reverse_iterator cit = _basicGraphs.crbegin();
		for (; cit != _basicGraphs.crend(); ++cit)
		{
			pCurrentGraphics = (*cit);

			if (execTransform)
			{
				pCurrentGraphics->getWorldTransform(tempMat);
				if (tempMat != curTransform)
				{
					curTransform = tempMat;
					_renderer->setTransform(ETK_WORLD, curTransform);
				}
			}

			pCurrentGraphics->render();
		}
		_basicGraphs.clear();
	}
}

void GraphicQueue::addGraphics(Graphics* pGraph)
{
	if (NULL == pGraph)
		return;

	if (pGraph->getZOrder() != (unsigned int)-1)
		_orderedGraphs.push_back(pGraph);
	else
		_basicGraphs.push_back(pGraph);
}

GraphicQueue::tagOrderedGraph::tagOrderedGraph(Graphics* g)
	: Graph(g)
	, Order(g->getZOrder())
{
}

GraphicQueueGroup::GraphicQueueGroup(LayerManager* relatedLayerMgr)
	: _relatedLayerMgr(relatedLayerMgr)
	, _renderer(Pivot::getSingleton().getActiveRenderer())
{
}

GraphicQueueGroup::~GraphicQueueGroup()
{
}

void GraphicQueueGroup::update()
{
	_currentTransform.makeIdentity();

	for (unsigned int queueID = 0; queueID < EGQ_COUNT; ++queueID)
	{
		bool exeTransform = true;

		if (queueID == EGQ_WIRE_BOUNDING_RECT)
		{
			_currentTransform.makeIdentity();
			_renderer->setTransform(ETK_WORLD, _currentTransform);
			exeTransform = false;
		}

		_groups[queueID].update(_currentTransform, exeTransform);
	}
}

void GraphicQueueGroup::clear(unsigned int queueID)
{
	if (queueID >= EGQ_COUNT)
		return;
	_groups[queueID].clear();
}

void GraphicQueueGroup::addGraphics(Graphics* pGraph)
{
	_groups[EGQ_MAIN_BODY].addGraphics(pGraph);
}

void GraphicQueueGroup::addGraphics(Graphics* pGraph, unsigned int queueID)
{
	GraphicQueue& zorderQueue = getQueue(queueID);
	zorderQueue.addGraphics(pGraph);
}

GraphicQueue& GraphicQueueGroup::getQueue(unsigned int queueID)
{
	if (queueID >= EGQ_COUNT)
		queueID = EGQ_MAIN_BODY;
	return _groups[queueID];
}

NS_VAYO2D_END
