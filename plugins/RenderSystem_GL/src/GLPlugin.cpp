#include "GLPlugin.h"
#include "GLRenderSystem.h"
#include "Vayo3dRoot.h"

static const wstring s_pluginName = L"RenderSystem_GL";

GLPlugin::GLPlugin()
	: _renderSys(NULL)
{
}

const wstring& GLPlugin::getName() const
{
	return s_pluginName;
}

void GLPlugin::install()
{
	_renderSys = new GLRenderSystem(s_pluginName);
	Root::getSingleton().addRenderSystem(_renderSys);
}

void GLPlugin::initialise()
{
	// nothing to do.
}

void GLPlugin::shutdown()
{
	// nothing to do.
}

void GLPlugin::uninstall()
{
	SAFE_DELETE(_renderSys);
}
