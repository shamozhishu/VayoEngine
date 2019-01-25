/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 引擎入口
\*************************************************************************/
#ifndef __VAYO3D_ROOT_H__
#define __VAYO3D_ROOT_H__

#include "VayoTimer.h"
#include "Vayo3dDevice.h"
#include "VayoSingleton.h"
NS_VAYO3D_BEGIN

class _Vayo3dExport Root : public Singleton<Root>
{
public:
	typedef struct tagConfig
	{
		bool           HandleSRGB;
		wstring        RendererName;
		wstring        RootDirectory;
		unsigned char  AntiAliasFactor;
		Device::Attrib MainDeviceAttrib;
		tagConfig()
			: HandleSRGB(false)
			, RendererName(L"RenderSystem_GL")
		    , AntiAliasFactor(0) {}
	} Config;

public:
	Root();
	~Root();
	bool          launch(const Config& config);
	bool          renderOneFrame(Device* renderWnd = NULL);
	void          bootFrame(SceneManager* scene, Device* dev = NULL, const wstring& userEvtID = L"");
	Timer&        getTimer() { return _timer; }
	const Timer&  getTimer() const { return _timer; }
	int           getFrameCnt() const { return _frameCnt; }
	float         getMsOneFrame() const { return _msOneFrame; }
	bool          IsLaunched() const { return _isLaunched; }
	UIManager*    getUIManager() const;
	void          addRenderSystem(RenderSystem* newRenderer);

	SceneManager* createSceneMgr(const wstring& sceneName = L"");
	SceneManager* findSceneMgr(const wstring& sceneName);
	void          destroySceneMgr(const wstring& sceneName);
	void          destroySceneMgr(SceneManager* pSceneMgr);
	void          destroyAllSceneMgrs();
	void          setNullCurSceneMgr();

	Device*       createDevice(void* wndHandle = NULL, bool wndQuit = true, bool wndPaint = false,
		wstring wndCaption = L"Vayo Engine", bool turnOnUI = true, bool fullScreen = false,
		unsigned bgClearColor = 0xff000000, Dimension2di screenSize = Dimension2di(1280, 720));
	Device*       findDevice(unsigned int idx);
	void          destroyDevice(unsigned int idx);
	void          destroyDevice(Device* dev);
	void          destroyAllDevices();
	static int    getMaxSupportDevCnt();

	// 插件相关
	void loadPlugins();
	void initialisePlugins();
	void shutdownPlugins();
	void unloadPlugins();
	void loadPlugin(const wstring& pluginName);
	void unloadPlugin(const wstring& pluginName);
	void installPlugin(Plugin* plugin);
	void uninstallPlugin(Plugin* plugin);
	const vector<Plugin*>& getInstalledPlugins() const;

private:
	Timer                       _timer;
	int                         _frameCnt;
	float                       _msOneFrame;
	bool                        _isLaunched;
	map<wstring, SceneManager*> _sceneMgrPool;
	map<wstring, RenderSystem*> _renderers;
	vector<Plugin*>             _plugins;
	vector<DynLib*>             _pluginLibs;
	static const int            _maxSupportDevCnt = 8;
	Device*                     _multiDevices[_maxSupportDevCnt];

private:
	void updateFrameStats();

protected:
	PROPERTY_R(SceneManager*,     _curSceneMgr,      CurSceneMgr)
	PROPERTY_R(Device*,           _mainDevice,       MainDevice)
	PROPERTY_R(Device*,           _activeDevice,     ActiveDevice)
	PROPERTY_R(RenderSystem*,     _activeRenderer,   ActiveRenderer)
	PROPERTY_RW(ScriptSystem*,    _scriptSystem,     ScriptSystem)
	PROPERTY_R(DynLibManager*,    _dynLibManager,    DynLibManager)
	PROPERTY_R(MeshManager*,      _meshManager,      MeshManager)
	PROPERTY_R(TextureManager*,   _textureManager,   TextureManager)
	PROPERTY_R(MaterialManager*,  _materialManager,  MaterialManager)
};

NS_VAYO3D_END

#endif // __VAYO3D_ROOT_H__
