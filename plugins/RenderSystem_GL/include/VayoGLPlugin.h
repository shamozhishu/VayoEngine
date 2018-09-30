/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* OpenGL插件
\*************************************************************************/
#ifndef __VAYO_GL_PLUGIN_H__
#define __VAYO_GL_PLUGIN_H__

#include "VayoGLSupport.h"
#include "VayoPlugin.h"
NS_VAYO_BEGIN

class GLPlugin : public Plugin
{
public:
	GLPlugin();
	const wstring& getName() const;
	void install();
	void initialise();
	void shutdown();
	void uninstall();

protected:
	GLRenderSystem* _renderSys;
};

NS_VAYO_END

#endif // __VAYO_GL_PLUGIN_H__
