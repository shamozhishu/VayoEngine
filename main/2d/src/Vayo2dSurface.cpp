#include "Vayo2dSurface.h"
#include "Vayo2dLayerManager.h"
#include "Vayo2dPictureManager.h"
#include "Vayo2dPivot.h"
#include "Vayo2dRenderer.h"

NS_VAYO2D_BEGIN

void Surface::Section::render()
{
	Renderer* pRenderer = Pivot::getSingleton().getActiveRenderer();
	pRenderer->drawBitmap(_feature->_picture, getPos());
}

Surface::Section::~Section()
{

}

//////////////////////////////////////////////////////////////////////////
Surface::Block::~Block()
{
	Pivot::getSingleton().getPictureManager()->removePicture(_pic);
}

//////////////////////////////////////////////////////////////////////////
Surface::Surface(const wstring& name, LayerManager* oriLayerMgr)
	: Body(name, oriLayerMgr)
	, _needUpdate(true)
	, _blockRows(1)
	, _blockCols(1)
{
}

Surface::~Surface()
{
	eraseSections();
}

void Surface::update(float dt)
{
	if (_needUpdate)
	{
		_needUpdate = false;
		wstringstream ss;
		PictureManager* pPicMgr = Pivot::getSingleton().getPictureManager();

		map<wstring, Section*>::iterator it = _sections.begin();
		for (; it != _sections.end(); ++it)
		{
			Section* pSec = it->second;
			pSec->_blockSet.resize(_blockRows, _blockCols);
			Table<Block>::iter it = pSec->_blockSet.begin();
			for (int i = 0; it != pSec->_blockSet.end(); ++it, ++i)
			{
				ss.str(L"");
				ss << pSec->_name << L"&" << i % _blockRows << L"&" << i / _blockCols;
				(*it)._pic = pPicMgr->getPicture(ss.str());
			}
		}
	}

	map<wstring, Section*>::iterator it = _sections.begin();
	for (; it != _sections.end(); ++it)
	{
		Pivot::getSingleton().getCurLayerMgr()->registerForRendering(it->second, getQueueID());
	}
}

void Surface::blockSplit(int rows, int cols, Rectf scope, Dimension2di maxSize)
{
	if (rows > 0 && cols > 0 && (_blockRows != rows || _blockCols != cols))
	{
		_blockRows = rows;
		_blockCols = cols;
		_scope = scope;
		_maxSize = maxSize;
		_needUpdate = true;
	}
}

bool Surface::frameStarted(float dt)
{
	Renderer* pRenderer = Pivot::getSingleton().getActiveRenderer();
	if (pRenderer->isDeviceLost())
		_needUpdate = true;
	return true;
}

bool Surface::frameRendering(float dt)
{
	return true;
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
