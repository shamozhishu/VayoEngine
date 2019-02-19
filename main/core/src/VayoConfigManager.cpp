#include "VayoConfigManager.h"
#include "tinyxml2/tinyxml2.h"
#include "VayoUtils.h"
#include "VayoLog.h"

using tinyxml2::XMLElement;

NS_VAYO_BEGIN

const Config& ConfigManager::getConfig() const
{
	return _configData;
}

const wstring& ConfigManager::getRootResourcePath() const
{
	return _rootResourcePath;
}

ConfigManager::ConfigManager()
{
}

bool ConfigManager::init(wstring rootDirectory, EDimension dimension /*= _3D*/)
{
	tinyxml2::XMLDocument doc;
	if (rootDirectory == L"")
	{
		rootDirectory = getWorkingDirectory();
	}

	if (rootDirectory.substr(rootDirectory.length() - 1) == L"\\"
		|| rootDirectory.substr(rootDirectory.length() - 1) == L"/")
	{
		rootDirectory.erase(rootDirectory.find_last_not_of(L"/\\") + 1);
	}

	if (doc.LoadFile((w2a_(rootDirectory) + "\\engine_config.xml").c_str()) != tinyxml2::XML_SUCCESS)
	{
		Log::wprint(ELL_ERROR, L"引擎配置文件[%s\\engine_config.xml]加载失败", rootDirectory.c_str());
		return false;
	}

	XMLElement* pRoot = doc.RootElement();
	if (NULL == pRoot)
	{
		Log::wprint(ELL_ERROR, L"引擎配置文件[%s\\engine_config.xml]解析失败", rootDirectory.c_str());
		return false;
	}

	_rootResourcePath = rootDirectory + L"\\" + utf8ToUnicode(pRoot->Attribute("rootpath"));

	wstring dimensionPath;
	if (dimension == _2D)
		dimensionPath = _rootResourcePath + utf8ToUnicode(pRoot->Attribute("path2d"));
	else
		dimensionPath = _rootResourcePath + utf8ToUnicode(pRoot->Attribute("path3d"));

	string curTag;
	XMLElement* pElem = pRoot->FirstChildElement();
	while (pElem)
	{
		curTag = pElem->Name();
		if (0 == curTag.compare("tablecsv"))
		{
			_configData.csvtables = _rootResourcePath + utf8ToUnicode(pElem->Attribute("filepath"));
		}
		else  if (0 == curTag.compare("plugins"))
		{
			XMLElement* plugin = pElem->FirstChildElement();
			while (plugin)
			{
				_configData.plugins.push_back(utf8ToUnicode(plugin->Attribute("name")));
				plugin = plugin->NextSiblingElement();
			}
		}
		else if (0 == curTag.compare("languages"))
		{
			XMLElement* language = pElem->FirstChildElement();
			while (language)
			{
				_configData.languages.push_back(_rootResourcePath + utf8ToUnicode(language->Attribute("filepath")));
				language = language->NextSiblingElement();
			}
		}
		else if (0 == curTag.compare("part_2d"))
		{
			if (dimension == _2D)
			{
				_configData._2d.layersetPath = dimensionPath + utf8ToUnicode(pElem->Attribute("layerset"));
				_configData._2d.featuresPath = dimensionPath + utf8ToUnicode(pElem->Attribute("features"));
				_configData._2d.surfacesPath = dimensionPath + utf8ToUnicode(pElem->Attribute("surfaces"));
			}
		}
		else if (0 == curTag.compare("part_3d"))
		{
			if (dimension == _3D)
			{
				XMLElement* p3dElem = pElem->FirstChildElement();
				while (p3dElem)
				{
					curTag = p3dElem->Name();
					if (0 == curTag.compare("scenes"))
					{
						_configData._3d.scenePath = dimensionPath + utf8ToUnicode(p3dElem->Attribute("scenes"));
						_configData._3d.modelsPath = dimensionPath + utf8ToUnicode(p3dElem->Attribute("models"));
						_configData._3d.materialsPath = dimensionPath + utf8ToUnicode(p3dElem->Attribute("materials"));
						_configData._3d.texturesPath = dimensionPath + utf8ToUnicode(p3dElem->Attribute("textures"));
						_configData._3d.shadersPath = dimensionPath + utf8ToUnicode(p3dElem->Attribute("shaders"));
						XMLElement* scene = p3dElem->FirstChildElement();
						while (scene)
						{
							int mapID = scene->IntAttribute("id");
							wstring mapPath = dimensionPath + utf8ToUnicode(scene->Attribute("mappath"));
							_configData._3d.mapPaths.insert(make_pair(mapID, mapPath));
							scene = scene->NextSiblingElement();
						}
					}
					else if (0 == curTag.compare("ui"))
					{
						_configData._3d.imgsetPath = dimensionPath + utf8ToUnicode(p3dElem->Attribute("imgsetpath"));
						_configData._3d.dlgXmlPath = dimensionPath + utf8ToUnicode(p3dElem->Attribute("dlgxmlpath"));
						_configData._3d.mapImgPath = dimensionPath + utf8ToUnicode(p3dElem->Attribute("mapimgpath"));
						XMLElement* skin = p3dElem->FirstChildElement("skin");
						if (!skin)
						{
							Log::wprint(ELL_ERROR, L"<skin>标签缺失");
							return false;
						}

						_configData._3d.skinFilePath = dimensionPath + utf8ToUnicode(skin->Attribute("file"));
						XMLElement* seqanim = p3dElem->FirstChildElement("seqanim");
						if (!seqanim)
						{
							Log::wprint(ELL_ERROR, L"<seqanim>标签缺失");
							return false;
						}

						_configData._3d.seqAnimPath = dimensionPath + utf8ToUnicode(seqanim->Attribute("file"));
						XMLElement* imagesetall = p3dElem->FirstChildElement("imagesetall");
						if (!imagesetall)
						{
							Log::wprint(ELL_ERROR, L"<imagesetall>标签缺失");
							return false;
						}

						XMLElement* imageset = imagesetall->FirstChildElement();
						while (imageset)
						{
							_configData._3d.imgsets.insert(make_pair(imageset->IntAttribute("id"),
								dimensionPath + utf8ToUnicode(imageset->Attribute("file"))));
							imageset = imageset->NextSiblingElement();
						}

						XMLElement* fontall = p3dElem->FirstChildElement("fonts");
						if (!fontall)
						{
							Log::wprint(ELL_ERROR, L"<fonts>标签缺失");
							return false;
						}

						Config::Font fontAttrib;
						XMLElement* font = fontall->FirstChildElement();
						while (font)
						{
							fontAttrib.size = font->IntAttribute("size");
							fontAttrib.bold = font->BoolAttribute("bold");
							fontAttrib.italic = font->BoolAttribute("italic");
							fontAttrib.filePath = dimensionPath + utf8ToUnicode(font->Attribute("file"));
							_configData._3d.fontset.push_back(fontAttrib);
							font = font->NextSiblingElement();
						}
					}
					else
					{
						Log::wprint(ELL_ERROR, L"错误标签<%s>", curTag.c_str());
						return false;
					}

					p3dElem = p3dElem->NextSiblingElement();
				}
			}
		}
		else
		{
			Log::wprint(ELL_ERROR, L"错误标签<%s>", curTag.c_str());
			return false;
		}

		pElem = pElem->NextSiblingElement();
	}

	Log::wprint(ELL_INFORMATION, L"引擎配置文件[%s\\engine_config.xml]加载成功", rootDirectory.c_str());
	return true;
}

NS_VAYO_END
