#include "Vayo2dPivot.h"

NS_VAYO2D_BEGIN

bool Pivot::openUI(Device* dev /*= NULL*/)
{
	return false;
}

void Pivot::closeUI(Device* dev /*= NULL*/)
{
}

Pivot::Pivot()
{
}

Pivot::~Pivot()
{
}

bool Pivot::launch(Core::Config* config)
{
	do 
	{
		IF_FALSE_BREAK(Core::launch(config));
		_isLaunched = true;
		return true;
	} while (0);
	_isLaunched = false;
	return false;
}

void Pivot::resize(Device* dev /*= NULL*/)
{

}

void Pivot::activate(Device* dev /*= NULL*/)
{

}

bool Pivot::renderOneFrame(Device* renderWnd /*= NULL*/)
{
	return true;
}

void Pivot::bootFrame(Device* dev, const wstring& scenename /*= L""*/, const wstring& userEvtID /*= L""*/)
{

}

bool Pivot::setPixelFormat(Device* dev /*= NULL*/)
{
	return true;
}

NS_VAYO2D_END
