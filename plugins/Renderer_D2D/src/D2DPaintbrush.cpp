#include "D2DPaintbrush.h"
#include "D2DRenderer.h"

D2DPaintbrush::D2DPaintbrush(ERenderTarget rt, EDeviceID devid, D2DRenderer* renderer)
	: _renderer(renderer)
	, _linkRTID(devid)
	, _brushReset(false)
{
	resetBrush(rt);
	_renderer->getDWriteFactory()->CreateTextFormat(L"Arial", NULL,
		DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
		24, L"en-us", &_textFormat);
	_renderer->getD2DFactory()->CreateStrokeStyle(D2D1::StrokeStyleProperties(
		D2D1_CAP_STYLE_FLAT, D2D1_CAP_STYLE_FLAT, D2D1_CAP_STYLE_FLAT, D2D1_LINE_JOIN_MITER,
		10.0f, D2D1_DASH_STYLE_SOLID, 0.0f), NULL, 0, &_strokeStyle);
}

D2DPaintbrush::~D2DPaintbrush()
{
}

void D2DPaintbrush::resetBrush(ERenderTarget rt)
{
	_brushReset = true;
	ComPtr<ID2D1RenderTarget> pRT;
	switch (rt)
	{
	case ERT_WINDOW_DEFAULT:
	case ERT_WINDOW_SOFTWARE:
	case ERT_WINDOW_HARDWARE:
		pRT = _renderer->getHwndRT(_linkRTID);
		break;
	case ERT_MEMORY:
		pRT = _renderer->getBitmapRT(_linkRTID);
		break;
	}
	if (pRT)
		pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &_colorBrush);
	else
		_colorBrush.Reset();
}

void D2DPaintbrush::onPainting(Feature& feature, const Feature& lastFeature)
{
	if (_colorBrush && (feature._color != lastFeature._color || _brushReset))
		_colorBrush->SetColor(D2D1::ColorF(feature._color._r, feature._color._g, feature._color._b, feature._color._a));

	if (feature._strokeStyle != lastFeature._strokeStyle)
	{
		D2D1_CAP_STYLE startCap = D2D1_CAP_STYLE_FLAT;
		D2D1_CAP_STYLE endCap = D2D1_CAP_STYLE_FLAT;
		D2D1_CAP_STYLE dashCap = D2D1_CAP_STYLE_FLAT;
		D2D1_LINE_JOIN lineJoin = D2D1_LINE_JOIN_MITER;
		FLOAT miterLimit = 10.0f;
		D2D1_DASH_STYLE dashStyle = D2D1_DASH_STYLE_SOLID;
		FLOAT dashOffset = 0.0f;

		switch (feature._strokeStyle.startCap)
		{
		case ECS_SQUARE: startCap = D2D1_CAP_STYLE_SQUARE; break;
		case ECS_ROUND: startCap = D2D1_CAP_STYLE_ROUND; break;
		case ECS_TRIANGLE: startCap = D2D1_CAP_STYLE_TRIANGLE; break;
		}

		switch (feature._strokeStyle.endCap)
		{
		case ECS_SQUARE: endCap = D2D1_CAP_STYLE_SQUARE; break;
		case ECS_ROUND: endCap = D2D1_CAP_STYLE_ROUND; break;
		case ECS_TRIANGLE: endCap = D2D1_CAP_STYLE_TRIANGLE; break;
		}

		switch (feature._strokeStyle.dashCap)
		{
		case ECS_SQUARE: dashCap = D2D1_CAP_STYLE_SQUARE; break;
		case ECS_ROUND: dashCap = D2D1_CAP_STYLE_ROUND; break;
		case ECS_TRIANGLE: dashCap = D2D1_CAP_STYLE_TRIANGLE; break;
		}

		switch (feature._strokeStyle.lineJoin)
		{
		case ELJ_BEVEL: lineJoin = D2D1_LINE_JOIN_BEVEL; break;
		case ELJ_ROUND: lineJoin = D2D1_LINE_JOIN_ROUND; break;
		case ELJ_MITER_OR_BEVEL: lineJoin = D2D1_LINE_JOIN_MITER_OR_BEVEL; break;
		}

		switch (feature._strokeStyle.dashStyle)
		{
		case EDS_DASH: dashStyle = D2D1_DASH_STYLE_DASH; break;
		case EDS_DOT: dashStyle = D2D1_DASH_STYLE_DOT; break;
		case EDS_DASH_DOT: dashStyle = D2D1_DASH_STYLE_DASH_DOT; break;
		case EDS_DASH_DOT_DOT: dashStyle = D2D1_DASH_STYLE_DASH_DOT_DOT; break;
		case EDS_CUSTOM: dashStyle = D2D1_DASH_STYLE_CUSTOM; break;
		}

		_strokeStyle.Reset();
		_renderer->getD2DFactory()->CreateStrokeStyle(D2D1::StrokeStyleProperties(
			startCap, endCap, dashCap, lineJoin, feature._strokeStyle.miterLimit,
			dashStyle, feature._strokeStyle.dashOffset), NULL, 0, &_strokeStyle);
	}
}
