#include "D2DRenderer.h"
#include "VayoLog.h"
#include "Vayo2dPivot.h"
#include "D2DBitmap.h"
#include "D2DPaintbrush.h"
#include "D2DGeometry.h"

D2DRenderer::D2DRenderer(const wstring& name)
	: Renderer(name)
	, _activateRTID(EDID_MAIN_DEVICE)
	, _d2dFactory(nullptr)
	, _wicFactory(nullptr)
	, _dwriteFactory(nullptr)
	, _activateRT(nullptr)
{
}

D2DRenderer::~D2DRenderer()
{
	for (unsigned int i = EDID_AUX_DEVICE0; i < EDID_DEVICE_COUNT; ++i)
		discardDeviceResources(i);
	destroyAllGeometries();
	_dwriteFactory.Reset();
	_wicFactory.Reset();
	_d2dFactory.Reset();
}

PicturePtr D2DRenderer::createPicture(const wstring& name)
{
	return new D2DBitmap(name, this);
}

Geometry* D2DRenderer::createGeometry(const wstring& name /*= L""*/)
{
	Geometry* geom = new D2DGeometry(name, this);
	map<wstring, Geometry*>::iterator it = _geometries.find(geom->getName());
	if (it != _geometries.end())
	{
		SAFE_DELETE(it->second);
		_geometries.erase(it);
	}
	_geometries[geom->getName()] = geom;
	return geom;
}

PaintbrushPtr D2DRenderer::createPaintbrush(ERenderTarget rt, unsigned int devid)
{
	return new D2DPaintbrush(rt, devid, this);
}

bool D2DRenderer::init()
{
	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), &_d2dFactory);
	if (FAILED(hr))
	{
		Log::wprint(ELL_ERROR, L"创建D2D工厂失败");
		return false;
	}

	hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_wicFactory));
	if (FAILED(hr))
	{
		Log::wprint(ELL_ERROR, L"创建WIC工厂失败");
		return false;
	}

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(_dwriteFactory), &_dwriteFactory);
	if (FAILED(hr))
	{
		Log::wprint(ELL_ERROR, L"创建DirectWrite工厂失败");
		return false;
	}

	return setRenderTarget(ERT_WINDOW);
}

bool D2DRenderer::beginDraw(Device* drawWnd)
{
	if (_deviceLost[_activateRTID] && drawWnd->getDeviceID() == _activateRTID)
	{
		if (rebuildDeviceResources())
			_deviceLost[_activateRTID] = false;
	}

	if (!_activateRT)
		return false;

	if (_activateRT == _hwndRT[_activateRTID] && (_hwndRT[_activateRTID]->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
		return false;

	_activateRT->BeginDraw();
	_activateRT->Clear(D2D1::ColorF(drawWnd->getAttrib().BgClearColor));
	return true;
}

bool D2DRenderer::endDraw()
{
	if (!_activateRT)
		return false;

	HRESULT hr = _activateRT->EndDraw();
	if (hr == D2DERR_RECREATE_TARGET)
	{
		hr = S_OK;
		_deviceLost[_activateRTID] = true;
		discardDeviceResources(_activateRTID);
	}

	return SUCCEEDED(hr);
}

void D2DRenderer::resize(unsigned int width, unsigned int height)
{
	ComPtr<ID2D1HwndRenderTarget> pHwndRT = getHwndRT(_activateRTID);
	if (pHwndRT)
		pHwndRT->Resize(D2D1::SizeU(width, height));
}

void D2DRenderer::drawPoint(const Vector2df& pt)
{
	if (_activateRT && _activateRTID < EDID_DEVICE_COUNT)
	{
		D2DPaintbrush* pBrush = nullptr;
		switch (_renderTarget)
		{
		case ERT_WINDOW:
			pBrush = dynamic_cast<D2DPaintbrush*>(_hwndPaintbrushs[_activateRTID].get()); break;
		case ERT_MEMORY:
			pBrush = dynamic_cast<D2DPaintbrush*>(_bitmapPaintbrushs[_activateRTID].get()); break;
		}

		if (pBrush)
		{
			setPaintbrushRenderState(pBrush);
			_activateRT->DrawRectangle(D2D1::Rect(pt._x, pt._y, pt._x, pt._y),
				pBrush->_colorBrush.Get(), _curFeature._strokeWidth, pBrush->_strokeStyle.Get());
		}
	}
}

void D2DRenderer::drawLine(const Vector2df& startPt, const Vector2df& endPt)
{
	if (_activateRT && _activateRTID < EDID_DEVICE_COUNT)
	{
		D2DPaintbrush* pBrush = nullptr;
		switch (_renderTarget)
		{
		case ERT_WINDOW:
			pBrush = dynamic_cast<D2DPaintbrush*>(_hwndPaintbrushs[_activateRTID].get()); break;
		case ERT_MEMORY:
			pBrush = dynamic_cast<D2DPaintbrush*>(_bitmapPaintbrushs[_activateRTID].get()); break;
		}

		if (pBrush)
		{
			setPaintbrushRenderState(pBrush);
			_activateRT->DrawLine(D2D1::Point2F(startPt._x, startPt._y), D2D1::Point2F(endPt._x, endPt._y),
				pBrush->_colorBrush.Get(), _curFeature._strokeWidth, pBrush->_strokeStyle.Get());
		}
	}
}

void D2DRenderer::drawRect(const Rectf& rc)
{
	if (_activateRT && _activateRTID < EDID_DEVICE_COUNT)
	{
		D2DPaintbrush* pBrush = nullptr;
		switch (_renderTarget)
		{
		case ERT_WINDOW:
			pBrush = dynamic_cast<D2DPaintbrush*>(_hwndPaintbrushs[_activateRTID].get()); break;
		case ERT_MEMORY:
			pBrush = dynamic_cast<D2DPaintbrush*>(_bitmapPaintbrushs[_activateRTID].get()); break;
		}

		if (pBrush)
		{
			setPaintbrushRenderState(pBrush);
			if (_curFeature._fill)
				_activateRT->FillRectangle(D2D1::Rect(rc._upperLeftCorner._x, rc._upperLeftCorner._y,
					rc._lowerRightCorner._x, rc._lowerRightCorner._y), pBrush->_colorBrush.Get());
			else
				_activateRT->DrawRectangle(D2D1::Rect(rc._upperLeftCorner._x, rc._upperLeftCorner._y,
				rc._lowerRightCorner._x, rc._lowerRightCorner._y),
				pBrush->_colorBrush.Get(), _curFeature._strokeWidth, pBrush->_strokeStyle.Get());
		}
	}
}

void D2DRenderer::drawRect(float x, float y, float w, float h)
{
	drawRect(Rectf(x, y, x + w, y + h));
}

void D2DRenderer::drawEllipse(const Vector2df& center, const Vector2df& radius)
{
	if (_activateRT && _activateRTID < EDID_DEVICE_COUNT)
	{
		D2DPaintbrush* pBrush = nullptr;
		switch (_renderTarget)
		{
		case ERT_WINDOW:
			pBrush = dynamic_cast<D2DPaintbrush*>(_hwndPaintbrushs[_activateRTID].get()); break;
		case ERT_MEMORY:
			pBrush = dynamic_cast<D2DPaintbrush*>(_bitmapPaintbrushs[_activateRTID].get()); break;
		}

		if (pBrush)
		{
			setPaintbrushRenderState(pBrush);
			if (_curFeature._fill)
				_activateRT->FillEllipse(D2D1::Ellipse(D2D1::Point2F(center._x, center._y), radius._x, radius._y),
					pBrush->_colorBrush.Get());
			else
				_activateRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(center._x, center._y), radius._x, radius._y),
				pBrush->_colorBrush.Get(), _curFeature._strokeWidth, pBrush->_strokeStyle.Get());
		}
	}
}

void D2DRenderer::drawGeometry(Geometry* geometry)
{
	D2DGeometry* pD2DGeometry = dynamic_cast<D2DGeometry*>(geometry);
	if (nullptr == pD2DGeometry)
		return;

	if (_activateRT && _activateRTID < EDID_DEVICE_COUNT)
	{
		D2DPaintbrush* pBrush = nullptr;
		switch (_renderTarget)
		{
		case ERT_WINDOW:
			pBrush = dynamic_cast<D2DPaintbrush*>(_hwndPaintbrushs[_activateRTID].get()); break;
		case ERT_MEMORY:
			pBrush = dynamic_cast<D2DPaintbrush*>(_bitmapPaintbrushs[_activateRTID].get()); break;
		}

		if (pBrush)
		{
			setPaintbrushRenderState(pBrush);
			if (_curFeature._fill)
				_activateRT->FillGeometry(pD2DGeometry->getPathGeometry().Get(), pBrush->_colorBrush.Get());
			else
				_activateRT->DrawGeometry(pD2DGeometry->getPathGeometry().Get(), pBrush->_colorBrush.Get(),
					_curFeature._strokeWidth, pBrush->_strokeStyle.Get());
		}
	}
}

void D2DRenderer::drawBitmap(PicturePtr pic, const Position2df& pos)
{
	if (_activateRT && _activateRTID < EDID_DEVICE_COUNT)
	{
		D2DBitmap* pPic = nullptr;
		ID2D1Bitmap* pBitmap = nullptr;
		if (pic)
			pBitmap = ((pPic = dynamic_cast<D2DBitmap*>(pic.get())) ? pPic->getBitmap().Get() : nullptr);
		else if (_bitmapRT[_activateRTID])
			_bitmapRT[_activateRTID]->GetBitmap(&pBitmap);
		
		if (pBitmap)
		{
			D2D1_SIZE_F size = pBitmap->GetSize();
			_activateRT->DrawBitmap(pBitmap, D2D1::RectF(pos._x, pos._y, size.width, size.height));
		}
	}
}

void D2DRenderer::drawBitmap(PicturePtr pic, const Rectf& dstRect, const Rectf& srcRect)
{
	if (_activateRT && _activateRTID < EDID_DEVICE_COUNT)
	{
		D2DBitmap* pPic = nullptr;
		ID2D1Bitmap* pBitmap = nullptr;
		if (pic)
			pBitmap = ((pPic = dynamic_cast<D2DBitmap*>(pic.get())) ? pPic->getBitmap().Get() : nullptr);
		else if (_bitmapRT[_activateRTID])
			_bitmapRT[_activateRTID]->GetBitmap(&pBitmap);

		if (pBitmap)
			_activateRT->DrawBitmap(pBitmap,
				D2D1::RectF(dstRect._upperLeftCorner._x, dstRect._upperLeftCorner._y,
					dstRect._lowerRightCorner._x, dstRect._lowerRightCorner._y),
				1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
				D2D1::RectF(srcRect._upperLeftCorner._x, srcRect._upperLeftCorner._y,
					srcRect._lowerRightCorner._x, srcRect._lowerRightCorner._y));
	}
}

bool D2DRenderer::setRenderTarget(ERenderTarget rt, const Dimension2di* pSize /*= nullptr*/)
{
	Renderer::setRenderTarget(rt);
	_activateRT.Reset();
	_activateRTID = EDID_MAIN_DEVICE;
	Device* pDev = Pivot::getSingleton().getActiveDevice();
	Dimension2di size;
	switch (rt)
	{
	case ERT_WINDOW:
		if (pDev)
		{
			EDeviceID devid = pDev->getDeviceID();
			if (_hwndRT[devid])
			{
				_activateRT = _hwndRT[devid];
			}
			else
			{
				if (pSize)
					size = *pSize;
				else
					size = pDev->getScreenSize();
				_d2dFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
					D2D1::HwndRenderTargetProperties((HWND)pDev->getWndHandle(), D2D1::SizeU(size._width, size._height), D2D1_PRESENT_OPTIONS_IMMEDIATELY),
					&_hwndRT[devid]);
				_activateRT = _hwndRT[devid];
			}

			_activateRTID = devid;
			getPaintbrush(devid);
		}
		break;
	case ERT_MEMORY:
		if (pDev)
		{
			EDeviceID devid = pDev->getDeviceID();
			if (_bitmapRT[devid])
			{
				_activateRT = _bitmapRT[devid];
			}
			else
			{
				if (!_hwndRT[devid])
				{
					size = pDev->getScreenSize();
					_d2dFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
						D2D1::HwndRenderTargetProperties((HWND)pDev->getWndHandle(), D2D1::SizeU(size._width, size._height), D2D1_PRESENT_OPTIONS_IMMEDIATELY),
						&_hwndRT[devid]);
				}

				if (!_hwndRT[devid])
					return false;

				if (pSize)
					size = *pSize;
				else
					size = pDev->getScreenSize();
				_hwndRT[devid]->CreateCompatibleRenderTarget(D2D1::SizeF(size._width, size._height), &_bitmapRT[devid]);
				_activateRT = _bitmapRT[devid];
			}

			_activateRTID = devid;
			getPaintbrush(devid);
		}
		break;
	}

	return _activateRT != nullptr;
}

const Matrix3x3& D2DRenderer::getTransform(ETransformKind kind) const
{
	return _matrizes[kind];
}

void D2DRenderer::setTransform(ETransformKind kind, const Matrix3x3& mat)
{
	_matrizes[kind] = mat;
	if (_activateRT)
	{
		Matrix3x3 matrix = _matrizes[ETK_PROJECTION] * _matrizes[ETK_WORLD] * _matrizes[ETK_VIEW];
		const D2D1::Matrix3x2F transform(matrix[0], matrix[1], matrix[3], matrix[4], matrix[6], matrix[7]);
		_activateRT->SetTransform(transform);
	}
}

void D2DRenderer::setFeature(const Feature& feature)
{
	_curFeature = feature;
}

ComPtr<ID2D1HwndRenderTarget> D2DRenderer::getHwndRT(int devid) const
{
	if (devid < 0 || devid >= EDID_DEVICE_COUNT)
		return nullptr;
	return _hwndRT[devid];
}

ComPtr<ID2D1BitmapRenderTarget> D2DRenderer::getBitmapRT(int devid) const
{
	if (devid < 0 || devid >= EDID_DEVICE_COUNT)
		return nullptr;
	return _bitmapRT[devid];
}

bool D2DRenderer::rebuildDeviceResources()
{
	return setRenderTarget(_renderTarget);
}

void D2DRenderer::discardDeviceResources(unsigned int rtid)
{
	if (rtid < EDID_DEVICE_COUNT)
	{
		_bitmapPaintbrushs[rtid] = nullptr;
		_hwndPaintbrushs[rtid] = nullptr;
		if (_activateRT == _hwndRT[rtid] || _activateRT == _bitmapRT[rtid])
			_activateRT.Reset();
		_bitmapRT[rtid].Reset();
		_hwndRT[rtid].Reset();
	}
}

void D2DRenderer::setPaintbrushRenderState(D2DPaintbrush* pBrush)
{
	if (pBrush)
	{
		pBrush->onUnsetFeature();
		pBrush->onSetFeature(_curFeature, _lastFeature);
	}

	_lastFeature = _curFeature;
}
