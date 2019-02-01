#include "Vayo2dRenderer.h"

NS_VAYO2D_BEGIN

bool Renderer::setRenderTarget(ERenderTarget rt)
{
	_renderTarget = rt;
	return true;
}

Renderer::Renderer()
	: _renderTarget(ERT_HWND)
{

}

Renderer::~Renderer()
{

}

NS_VAYO2D_END
