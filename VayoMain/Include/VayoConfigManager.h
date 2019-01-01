/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 文件配置管理器
\*************************************************************************/
#ifndef __VAYO_CONFIG_MANAGER_H__
#define __VAYO_CONFIG_MANAGER_H__

#include "VayoSupport.h"
NS_VAYO_BEGIN

struct tagPluginConfig
{
	wstring PluginName;
};

struct tagLanguageConfig
{
	wstring FilePath;
};

struct tagSceneConfig
{
	wstring ScenePath;
	wstring ModelsPath;
	wstring MaterialsPath;
	wstring TexturesPath;
	wstring ShadersPath;
	unordered_map<int, wstring> MapPaths;
};

struct tagUIConfig
{
	wstring ImgsetPath;
	wstring DlgXmlPath;
	wstring MapImgPath;
	wstring SkinFilePath;
	wstring SeqAnimPath;
	wstring TableCSVPath;
	unordered_map<int, wstring> ImgsetAll;
};

struct tagFontConfig
{
	int     FontSize;
	bool    FontBold;
	bool    FontItalic;
	wstring FontFilePath;
};

class _VayoExport ConfigManager
{
	DISALLOW_COPY_AND_ASSIGN(ConfigManager)
public:
	ConfigManager() {}
	bool                             init(wstring rootDirectory);
	const tagUIConfig&               getUIConfig() const;
	const tagSceneConfig&            getSceneConfig() const;
	const vector<tagFontConfig>&     getFontConfig() const;
	const vector<tagPluginConfig>&   getPluginConfig() const;
	const vector<tagLanguageConfig>& getLanguageConfig() const;
	const wstring&                   getRootResourcePath() const;

private:
	tagUIConfig               _uiConfig;
	tagSceneConfig            _sceneConfig;
	vector<tagFontConfig>     _fontConfig;
	vector<tagPluginConfig>   _pluginConfig;
	vector<tagLanguageConfig> _languageConfig;
	wstring                   _rootResourcePath;
};

NS_VAYO_END

#endif // __VAYO_CONFIG_MANAGER_H__
