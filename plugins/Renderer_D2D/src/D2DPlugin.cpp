#include "D2DPlugin.h"
#include "D2DRenderer.h"
#include "Vayo2dPivot.h"

static const wstring s_pluginName = L"Renderer_D2D";
D2DPlugin::D2DPlugin()
	: _renderer(nullptr)
{
}

const wstring& D2DPlugin::getName() const
{
	return s_pluginName;
}

void D2DPlugin::install()
{
	CoInitialize(nullptr);
	_renderer = new D2DRenderer(s_pluginName);
	Pivot::getSingleton().addRenderer(_renderer);
}

void D2DPlugin::initialise()
{
}

void D2DPlugin::shutdown()
{
}

void D2DPlugin::uninstall()
{
	SAFE_DELETE(_renderer);
	CoUninitialize();
}
