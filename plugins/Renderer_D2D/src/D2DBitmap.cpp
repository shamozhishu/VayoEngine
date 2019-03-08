#include "D2DBitmap.h"

D2DBitmap::D2DBitmap(const wstring& name, D2DRenderer* renderer)
	: Picture(name)
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

D2DBitmap::~D2DBitmap()
{
}

ComPtr<ID2D1Bitmap> D2DBitmap::getBitmap() const
{
	return _bitmap;
}
