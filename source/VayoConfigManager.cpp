#include "VayoConfigManager.h"
#include "tinyxml2/tinyxml2.h"
#include "VayoUtils.h"

using namespace tinyxml2;

NS_VAYO_BEGIN

bool ConfigManager::init(const wstring& rootDirectory /*= L""*/)
{
	tinyxml2::XMLDocument doc;
	string configDirectory = w2a_(rootDirectory);

	if (configDirectory == "")
	{
		configDirectory = w2a_(getWorkingDirectory());
	}

	if (configDirectory.substr(configDirectory.length() - 1) == "\\"
		|| configDirectory.substr(configDirectory.length() - 1) == "/")
	{
		configDirectory.erase(configDirectory.find_last_not_of("/\\") + 1);
	}

	if (doc.LoadFile((configDirectory + "\\engine_config.xml").c_str()) != XML_SUCCESS)
		return false;

	XMLElement* pRoot = doc.RootElement();
	if (NULL == pRoot)
		return false;

	string curTag;
	_rootResourcePath = a2w_(configDirectory) + L"\\" + utf8ToUnicode(pRoot->Attribute("rootpath"));
	XMLElement* pElem = pRoot->FirstChildElement();
	while (pElem)
	{
		curTag = pElem->Name();

		if (0 == curTag.compare("plugins"))
		{
			XMLElement* plugin = pElem->FirstChildElement();
			while (plugin)
			{
				tagPluginConfig pluginConfig;
				pluginConfig.PluginName = utf8ToUnicode(plugin->Attribute("name"));
				_pluginConfig.push_back(pluginConfig);
				plugin = plugin->NextSiblingElement();
			}
		}
		else if (0 == curTag.compare("languages"))
		{
			XMLElement* language = pElem->FirstChildElement();
			while (language)
			{
				tagLanguageConfig lanConfig;
				lanConfig.FilePath = _rootResourcePath + utf8ToUnicode(language->Attribute("filepath"));
				_languageConfig.push_back(lanConfig);
				language = language->NextSiblingElement();
			}
		}
		else if (0 == curTag.compare("scenes"))
		{
			_sceneConfig.ScenePath = _rootResourcePath + utf8ToUnicode(pElem->Attribute("scenes"));
			_sceneConfig.ModelsPath = _rootResourcePath + utf8ToUnicode(pElem->Attribute("models"));
			_sceneConfig.MaterialsPath = _rootResourcePath + utf8ToUnicode(pElem->Attribute("materials"));
			_sceneConfig.TexturesPath = _rootResourcePath + utf8ToUnicode(pElem->Attribute("textures"));
			_sceneConfig.ShadersPath = _rootResourcePath + utf8ToUnicode(pElem->Attribute("shaders"));
			XMLElement* scene = pElem->FirstChildElement();
			while (scene)
			{
				int mapID = scene->IntAttribute("id");
				wstring mapPath = _rootResourcePath + utf8ToUnicode(scene->Attribute("mappath"));
				_sceneConfig.MapPaths.insert(make_pair(mapID, mapPath));
				scene = scene->NextSiblingElement();
			}
		}
		else if (0 == curTag.compare("ui"))
		{
			_uiConfig.ImgsetPath = _rootResourcePath + utf8ToUnicode(pElem->Attribute("imgsetpath"));
			_uiConfig.DlgXmlPath = _rootResourcePath + utf8ToUnicode(pElem->Attribute("dlgxmlpath"));
			_uiConfig.MapImgPath = _rootResourcePath + utf8ToUnicode(pElem->Attribute("mapimgpath"));
			_uiConfig.TableCSVPath = _rootResourcePath + utf8ToUnicode(pElem->Attribute("tablecsv"));
			XMLElement* skin = pElem->FirstChildElement("skin");
			if (!skin)
				return false;

			_uiConfig.SkinFilePath = _rootResourcePath + utf8ToUnicode(skin->Attribute("file"));
			XMLElement* seqanim = pElem->FirstChildElement("seqanim");
			if (!seqanim)
				return false;

			_uiConfig.SeqAnimPath = _rootResourcePath + utf8ToUnicode(seqanim->Attribute("file"));
			XMLElement* imagesetall = pElem->FirstChildElement("imagesetall");
			if (!imagesetall)
				return false;

			XMLElement* imageset = imagesetall->FirstChildElement();
			while (imageset)
			{
				_uiConfig.ImgsetAll.insert(make_pair(imageset->IntAttribute("id"),
					_rootResourcePath + utf8ToUnicode(imageset->Attribute("file"))));
				imageset = imageset->NextSiblingElement();
			}

			XMLElement* fontall = pElem->FirstChildElement("fonts");
			if (!fontall)
				return false;

			tagFontConfig fontAttrib;
			XMLElement* font = fontall->FirstChildElement();
			while (font)
			{
				fontAttrib.FontSize = font->IntAttribute("size");
				fontAttrib.FontBold = font->BoolAttribute("bold");
				fontAttrib.FontItalic = font->BoolAttribute("italic");
				fontAttrib.FontFilePath = _rootResourcePath + utf8ToUnicode(font->Attribute("file"));
				_fontConfig.push_back(fontAttrib);
				font = font->NextSiblingElement();
			}
		}

		pElem = pElem->NextSiblingElement();
	}

	return true;
}

const tagUIConfig& ConfigManager::getUIConfig() const
{
	return _uiConfig;
}

const tagSceneConfig& ConfigManager::getSceneConfig() const
{
	return _sceneConfig;
}

const vector<tagFontConfig>& ConfigManager::getFontConfig() const
{
	return _fontConfig;
}

const vector<tagPluginConfig>& ConfigManager::getPluginConfig() const
{
	return _pluginConfig;
}

const vector<tagLanguageConfig>& ConfigManager::getLanguageConfig() const
{
	return _languageConfig;
}

const wstring& ConfigManager::getRootResourcePath() const
{
	return _rootResourcePath;
}

NS_VAYO_END
