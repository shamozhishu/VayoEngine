#include "VayoRenderQueue.h"
#include "VayoRenderable.h"
#include "VayoRenderSystem.h"
#include "VayoSceneManager.h"
#include "VayoCamera.h"
#include "VayoRoot.h"

NS_VAYO_BEGIN

void RenderQueue::clear()
{
	_basicSolids.clear();
	_orderedSolids.clear();
	_transparentSolids.clear();
}

RenderQueue::RenderQueue()
	: _renderer(Root::getSingleton().getActiveRenderer())
{
}

void RenderQueue::update(Matrix4x4& curTransform, bool execTransform /*= true*/)
{
	Renderable* pCurrentRenderable = NULL;
	Matrix4x4 tempMat;

	if (_orderedSolids.size() > 0)
	{
		std::sort(_orderedSolids.begin(), _orderedSolids.end());
		vector<OrderedSolid>::const_iterator cit = _orderedSolids.cbegin();
		for (; cit != _orderedSolids.cend(); ++cit)
		{
			pCurrentRenderable = (*cit).Rend;

			if (execTransform)
			{
				pCurrentRenderable->getWorldTransform(tempMat);
				if (tempMat != curTransform)
				{
					curTransform = tempMat;
					_renderer->setTransform(ETS_WORLD, curTransform);
				}
			}
			
			pCurrentRenderable->render();
		}
		_orderedSolids.clear();
	}

	if (_basicSolids.size() > 0)
	{
		vector<Renderable*>::const_iterator cit = _basicSolids.cbegin();
		for (; cit != _basicSolids.cend(); ++cit)
		{
			pCurrentRenderable = (*cit);

			if (execTransform)
			{
				pCurrentRenderable->getWorldTransform(tempMat);
				if (tempMat != curTransform)
				{
					curTransform = tempMat;
					_renderer->setTransform(ETS_WORLD, curTransform);
				}
			}
			
			pCurrentRenderable->render();
		}
		_basicSolids.clear();
	}

	if (_transparentSolids.size() > 0)
	{
		std::sort(_transparentSolids.begin(), _transparentSolids.end());
		vector<TransparentSolid>::const_iterator cit = _transparentSolids.cbegin();
		for (; cit != _transparentSolids.cend(); ++cit)
		{
			pCurrentRenderable = (*cit).Rend;
			
			if (execTransform)
			{
				pCurrentRenderable->getWorldTransform(tempMat);
				if (tempMat != curTransform)
				{
					curTransform = tempMat;
					_renderer->setTransform(ETS_WORLD, curTransform);
				}
			}

			pCurrentRenderable->render();
		}
		_transparentSolids.clear();
	}
}

void RenderQueue::addRenderable(Renderable* pRend)
{
	if (NULL == pRend)
		return;

	MaterialRenderer* pMaterialRenderer = _renderer->getMaterialRenderer(pRend->getMaterial()->_materialType);
	if (pMaterialRenderer && pMaterialRenderer->isTransparent())
		_transparentSolids.push_back(TransparentSolid(pRend, Root::getSingleton().getCurSceneMgr()));
	else if (pRend->getRenderPriority() != (unsigned int)-1)
		_orderedSolids.push_back(pRend);
	else
		_basicSolids.push_back(pRend);
}

RenderQueue::tagOrderedSolid::tagOrderedSolid(Renderable* r)
	: Rend(r)
	, Prior(r->getRenderPriority())
{
}

RenderQueue::tagTransparentSolid::tagTransparentSolid(Renderable* r, SceneManager* relatedSceneMgr)
	: Rend(r)
	, Distance(0.0f)
{
	Camera* cam = relatedSceneMgr->getActiveCamera();
	if (cam)
	{
		Vector3df cameraPos = cam->getWorldPos();
		Matrix4x4 mat;
		Rend->getWorldTransform(mat);
		Distance = mat.getTranslation().getDistanceFromSQ(cameraPos);
	}
}

RenderQueueGroup::RenderQueueGroup(SceneManager* relatedSceneMgr)
	: _relatedSceneMgr(relatedSceneMgr)
	, _renderer(Root::getSingleton().getActiveRenderer())
{
}

RenderQueueGroup::~RenderQueueGroup()
{
}

void RenderQueueGroup::update()
{
	_currentTransform.makeIdentity();

	for (unsigned int queueID = 0; queueID < ERQ_COUNT; ++queueID)
	{
		bool exeTransform = true;

		if (queueID == ERQ_WIRE_BOUNDING_BOX)
		{
			_currentTransform.makeIdentity();
			_renderer->setTransform(ETS_WORLD, _currentTransform);
			exeTransform = false;
		}

		_groups[queueID].update(_currentTransform, exeTransform);
	}
}

void RenderQueueGroup::clear(unsigned int queueID)
{
	if (queueID >= ERQ_COUNT)
		return;
	_groups[queueID].clear();
}

void RenderQueueGroup::addRenderable(Renderable* pRend)
{
	_groups[ERQ_MAIN_SOLID].addRenderable(pRend);
}

void RenderQueueGroup::addRenderable(Renderable* pRend, unsigned int queueID)
{
	RenderQueue& renderQueue = getQueue(queueID);
	renderQueue.addRenderable(pRend);
}

RenderQueue& RenderQueueGroup::getQueue(unsigned int queueID)
{
	if (queueID >= ERQ_COUNT)
		queueID = ERQ_MAIN_SOLID;
	return _groups[queueID];
}

NS_VAYO_END
