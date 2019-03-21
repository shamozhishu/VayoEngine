#include "Vayo2dSurface.h"
#include "Vayo2dLayerManager.h"
#include "Vayo2dPictureManager.h"
#include "Vayo2dPivot.h"
#include "Vayo2dRenderer.h"

NS_VAYO2D_BEGIN

void Surface::Section::render()
{
	Renderer* pRenderer = Pivot::getSingleton().getActiveRenderer();
	pRenderer->drawPicture(getFeature()->_picture, getPos());
}

Reflex<Surface, const wstring&, LayerManager*> Surface::_dynReflex;
Surface::Surface(const wstring& name, LayerManager* oriLayerMgr)
	: Body(name, oriLayerMgr)
{
}

Surface::~Surface()
{
	eraseSections();
}

void Surface::update(float dt)
{
	map<wstring, Section*>::iterator it = _sections.begin();
	for (; it != _sections.end(); ++it)
	{
		Pivot::getSingleton().getCurLayerMgr()->registerForRendering(it->second, getQueueID());
	}
}

Surface::Section* Surface::getSection(const wstring& secName)
{
	map<wstring, Section*>::iterator it = _sections.find(secName);
	if (it != _sections.end())
		return it->second;
	Surface::Section* pSection = new Surface::Section();
	pSection->_name = secName;
	_sections.insert(make_pair(secName, pSection));
	return pSection;
}

void Surface::eraseSection(const wstring& secName)
{
	map<wstring, Section*>::iterator it = _sections.find(secName);
	if (it != _sections.end())
	{
		SAFE_DELETE(it->second);
		_sections.erase(it);
	}
}

void Surface::eraseSection(Surface::Section* pSection)
{
	if (pSection)
		eraseSection(pSection->_name);
}

void Surface::eraseSections()
{
	map<wstring, Section*>::iterator it = _sections.begin();
	for (; it != _sections.end(); ++it)
	{
		SAFE_DELETE(it->second);
	}
	_sections.clear();
}

NS_VAYO2D_END
