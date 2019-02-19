#include "D2DRenderer.h"
#include "VayoLog.h"
#include "Vayo2dPivot.h"
#include "D2DSurface.h"
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
	memset(_hwndRT, 0, sizeof(_hwndRT));
	memset(_bitmapRT, 0, sizeof(_bitmapRT));
}

D2DRenderer::~D2DRenderer()
{
	discardDeviceResources();
	SAFE_RELEASE(_dwriteFactory);
	SAFE_RELEASE(_wicFactory);
	SAFE_RELEASE(_d2dFactory);
}

SurfacePtr D2DRenderer::createSurface(const wstring& name)
{
	return new D2DSurface(name, this);
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
	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory),
		reinterpret_cast<void**>(&_d2dFactory));
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

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(_dwriteFactory),
		reinterpret_cast<IUnknown **>(&_dwriteFactory));
	if (FAILED(hr))
	{
		Log::wprint(ELL_ERROR, L"创建DirectWrite工厂失败");
		return false;
	}

	return setRenderTarget(ERT_WINDOW);
}

bool D2DRenderer::beginDraw(Device* drawWnd)
{
	if (!_activateRT)
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
		discardDeviceResources();
	}

	return SUCCEEDED(hr);
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
				pBrush->_colorBrush, _curFeature._strokeWidth, pBrush->_strokeStyle);
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
				pBrush->_colorBrush, _curFeature._strokeWidth, pBrush->_strokeStyle);
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
					rc._lowerRightCorner._x, rc._lowerRightCorner._y), pBrush->_colorBrush);
			else
				_activateRT->DrawRectangle(D2D1::Rect(rc._upperLeftCorner._x, rc._upperLeftCorner._y,
				rc._lowerRightCorner._x, rc._lowerRightCorner._y),
				pBrush->_colorBrush, _curFeature._strokeWidth, pBrush->_strokeStyle);
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
					pBrush->_colorBrush);
			else
				_activateRT->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(center._x, center._y), radius._x, radius._y),
				pBrush->_colorBrush, _curFeature._strokeWidth, pBrush->_strokeStyle);
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
				_activateRT->FillGeometry(pD2DGeometry->getPathGeometry(), pBrush->_colorBrush);
			else
				_activateRT->DrawGeometry(pD2DGeometry->getPathGeometry(), pBrush->_colorBrush,
					_curFeature._strokeWidth, pBrush->_strokeStyle);
		}
	}
}

bool D2DRenderer::setRenderTarget(ERenderTarget rt)
{
	Renderer::setRenderTarget(rt);
	_activateRT = nullptr;
	_activateRTID = EDID_MAIN_DEVICE;
	Device* pDev = Pivot::getSingleton().getActiveDevice();

	switch (rt)
	{
	case ERT_WINDOW:
		if (pDev)
		{
			unsigned int devid = pDev->getDeviceID();
			if (_hwndRT[devid])
			{
				_activateRT = _hwndRT[devid];
			}
			else
			{
				Dimension2di rc = pDev->getScreenSize();
				D2D1_SIZE_U size = D2D1::SizeU(rc._width, rc._height);
				_d2dFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
					D2D1::HwndRenderTargetProperties((HWND)pDev->getWndHandle(), size, D2D1_PRESENT_OPTIONS_IMMEDIATELY),
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
			int devid = pDev->getDeviceID();
			if (_bitmapRT)
			{
				_activateRT = _bitmapRT[devid];
			}
			else if (_hwndRT[devid] && _curFeature._surface)
			{
				Dimension2df size = _curFeature._surface->getSize();
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
		Matrix3x3 matrix = _matrizes[ETK_WORLD] * _matrizes[ETK_VIEW];
		const D2D1::Matrix3x2F transform(matrix[0], matrix[1], matrix[3], matrix[4], matrix[6], matrix[7]);
		_activateRT->SetTransform(transform);
	}
}

void D2DRenderer::setFeature(const Feature& feature)
{
	_curFeature = feature;
}

ID2D1HwndRenderTarget* D2DRenderer::getHwndRT(int devid) const
{
	if (devid < 0 || devid >= EDID_DEVICE_COUNT)
		return nullptr;
	return _hwndRT[devid];
}

ID2D1BitmapRenderTarget* D2DRenderer::getBitmapRT(int devid) const
{
	if (devid < 0 || devid >= EDID_DEVICE_COUNT)
		return nullptr;
	return _bitmapRT[devid];
}

void D2DRenderer::discardDeviceResources()
{
	destroyAllGeometries();
	for (int i = 0; i < EDID_DEVICE_COUNT; ++i)
	{
		SAFE_RELEASE(_bitmapRT[i]);
		SAFE_RELEASE(_hwndRT[i]);
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
