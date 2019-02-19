#include "D2DSurface.h"

D2DSurface::D2DSurface(const wstring& name, D2DRenderer* renderer)
	: Surface(name)
	, _renderer(renderer)
{
	static unsigned short idx = 0;
	if (0 == _name.compare(L""))
	{
		std::wstringstream ss;
		ss << L"Direct2D_Bitmap" << idx;
		++idx;
		_name = ss.str();
	}
}

D2DSurface::~D2DSurface()
{
}
