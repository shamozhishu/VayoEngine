/*************************************************************************\
* ��������1.0
* Copyright (c) 2018-2019 authored by ��Ӻ�
* �ļ����ù�����
\*************************************************************************/
#ifndef __VAYO_CONFIG_MANAGER_H__
#define __VAYO_CONFIG_MANAGER_H__

#include "VayoSupport.h"
#include "VayoSingleton.h"
NS_VAYO_BEGIN

struct Config
{
	wstring csvtables;
	vector<wstring> plugins;
	vector<wstring> languages;

	struct
	{
		wstring layersetPath;
		wstring picturesPath;
		wstring featuresPath;
	} _2d;

	struct Font
	{
		int     size;
		bool    bold;
		bool    italic;
		wstring filePath;
	};

	struct
	{
		wstring scenePath;
		wstring modelsPath;
		wstring shadersPath;
		wstring texturesPath;
		wstring materialsPath;
		wstring imgsetPath;
		wstring dlgXmlPath;
		wstring mapImgPath;
		wstring seqAnimPath;
		wstring skinFilePath;
		vector<Font> fontset;
		unordered_map<int, wstring> imgsets;
		unordered_map<int, wstring> mapPaths;
	} _3d;
};

class _VayoExport ConfigManager : public Singleton<ConfigManager>
{
	DISALLOW_COPY_AND_ASSIGN(ConfigManager)
public:
	enum EDimension { _2D, _3D };
public:
	ConfigManager();
	bool init(wstring rootDirectory, EDimension dimension = _3D);
	const Config& getConfig() const;
	const wstring& getRootResourcePath() const;

private:
	Config  _configData;
	wstring _rootResourcePath;
};

NS_VAYO_END

#endif // __VAYO_CONFIG_MANAGER_H__
