/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 3D引擎入口
\*************************************************************************/
#ifndef __VAYO3D_ROOT_H__
#define __VAYO3D_ROOT_H__

#include "VayoCore.h"
#include "Vayo3dSupport.h"
#include "VayoSingleton.h"
#include "VayoPluginSystem.h"
NS_VAYO3D_BEGIN

class _Vayo3dExport Root : public Singleton<Root>, public Core, public PluginSystem
{
public:
	typedef struct tagConfig : Core::tagConfig
	{
		bool          HandleSRGB;
		unsigned char AntiAliasFactor;
		tagConfig() : HandleSRGB(false), AntiAliasFactor(0)
		{ RendererName = L"RenderSystem_GL"; }
	} Config;

public:
	Root();
	~Root();
	bool          launch(Core::Config* config);
	void          resize(Device* dev = NULL);
	void          activate(Device* dev = NULL);
	bool          fireFrameRendering(Device* renderWnd = NULL);
	void          bootFrame(Device* dev, const wstring& scenename = L"", const wstring& userEvtID = L"");
	
	bool          openUI(Device* dev = NULL);
	void          closeUI(Device* dev = NULL);
	UIManager*    getUIManager(Device* dev = NULL) const;
	bool          configDevice(Device* dev = NULL);
	void          addRenderSystem(RenderSystem* newRenderer);

	SceneManager* createSceneMgr(const wstring& sceneName = L"");
	SceneManager* findSceneMgr(const wstring& sceneName);
	void          destroySceneMgr(const wstring& sceneName);
	void          destroySceneMgr(SceneManager* pSceneMgr);
	void          destroyAllSceneMgrs();
	void          setNullCurSceneMgr();

private:
	UIManager* _uiManager[_maxSupportDevCnt+1];
	map<wstring, RenderSystem*> _renderers;
	map<wstring, SceneManager*> _sceneMgrPool;

protected:
	PROPERTY_R(SceneManager*,    _curSceneMgr,     CurSceneMgr)
	PROPERTY_R(RenderSystem*,    _activeRenderer,  ActiveRenderer)
	PROPERTY_R(MeshManager*,     _meshManager,     MeshManager)
	PROPERTY_R(TextureManager*,  _textureManager,  TextureManager)
	PROPERTY_R(MaterialManager*, _materialManager, MaterialManager)
};

NS_VAYO3D_END

#endif // __VAYO3D_ROOT_H__
