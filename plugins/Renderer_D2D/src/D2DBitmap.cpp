#include "D2DBitmap.h"
#include "D2DRenderer.h"
#include "VayoCore.h"
#include "VayoConfigManager.h"

D2DBitmap::D2DBitmap(const wstring& filename, D2DRenderer* renderer)
	: Picture(filename)
	, _renderer(renderer)
{
	if (0 == _filename.compare(L""))
	{
		_filename = ConfigManager::getSingleton().getConfig()._2d.picturesPath;
		_filename += L"default.png";
	}
}

void D2DBitmap::destroy()
{
	_wicBitmap.Reset();
	_wicConverter.Reset();
	for (int i = EDID_AUX_DEVICE0; i < EDID_DEVICE_COUNT; ++i)
		_bitmap[i].Reset();
}

void D2DBitmap::cleanPic()
{
	_bitmap[_renderer->getActivateRTID()].Reset();
}

bool D2DBitmap::isLoaded() const
{
	return _wicBitmap != nullptr && _wicConverter != nullptr;
}

Dimension2df D2DBitmap::getPicSize() const
{
	UINT width = 0, height = 0;
	if (_wicConverter)
		_wicConverter->GetSize(&width, &height);
	return Dimension2df(width, height);
}

ComPtr<ID2D1Bitmap> D2DBitmap::getBitmap(EDeviceID devid /*= EDID_CURRENT_DEV*/)
{
	if (devid == EDID_CURRENT_DEV)
	{
		Device* dev = Core::getCore().getActiveDevice();
		if (dev)
			devid = dev->getDeviceID();
		else
			devid = EDID_MAIN_DEVICE;
	}

	if (!_bitmap[devid])
	{
		ComPtr<ID2D1HwndRenderTarget> pRT = _renderer->getHwndRT(devid);
		if (pRT)
		{
			HRESULT hr = S_OK;
			if (_wicConverter)
				hr = pRT->CreateBitmapFromWicBitmap(_wicConverter.Get(), nullptr, &_bitmap[devid]);
			else
			{
				ComPtr<ID2D1BitmapRenderTarget> pBitmapRT = _renderer->getBitmapRT(devid);
				if (pBitmapRT)
					hr = pBitmapRT->GetBitmap(&_bitmap[devid]);
			}
			printComError(hr);
		}
	}
	return _bitmap[devid];
}
