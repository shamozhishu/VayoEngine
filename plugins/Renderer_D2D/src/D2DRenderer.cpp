#include "D2DRenderer.h"

D2DRenderer::D2DRenderer()
{

}

D2DRenderer::~D2DRenderer()
{

}

bool D2DRenderer::beginDraw()
{
	return true;
}

bool D2DRenderer::endDraw()
{
	return true;
}

void D2DRenderer::drawRect(const Rectf& rc)
{

}

void D2DRenderer::drawRect(float x, float y, float w, float h)
{

}

void D2DRenderer::drawRound(float radius, float center)
{

}

void D2DRenderer::drawGeometry()
{

}

bool D2DRenderer::setRenderTarget(ERenderTarget rt)
{
	Renderer::setRenderTarget(rt);
	return true;
}
