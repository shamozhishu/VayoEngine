/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* OpenGL插件
\*************************************************************************/
#ifndef __GL_PLUGIN_H__
#define __GL_PLUGIN_H__

#include "GLSupport.h"
#include "VayoPlugin.h"

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

#endif // __GL_PLUGIN_H__
