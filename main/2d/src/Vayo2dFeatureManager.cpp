#include "Vayo2dFeatureManager.h"
#include "Vayo2dPictureManager.h"
#include "VayoConfigManager.h"
#include "Vayo2dPivot.h"
#include "VayoUtils.h"
#include "VayoLog.h"

NS_VAYO2D_BEGIN

FeatureManager::FeatureManager()
{
	int i = 0;
	while (featureScriptAttribs[i].TheKey)
	{
		_attribsWordMap[featureScriptAttribs[i].TheKey] = featureScriptAttribs[i].TheValue;
		++i;
	}
}

FeatureManager::~FeatureManager()
{
}

bool FeatureManager::init()
{
	_featurePool.clear();
	vector<wstring> allFilePath;
	findFileDir(allFilePath, ConfigManager::getSingleton().getConfig()._2d.featuresPath, L"feature");
	unsigned len = allFilePath.size();
	for (unsigned i = 0; i < len; ++i)
		parseFeature(allFilePath[i], true);
	return true;
}

bool FeatureManager::parseFeature(const wstring& filename, bool fullPath /*= false*/)
{
	wstring fileName = filename;
	trim(fileName);
	if (fileName == L"" || fileName.substr(fileName.rfind(L'.')) != L".features")
	{
		Log::wprint(ELL_ERROR, L"特征脚本[%s]文件扩展名必须是[.features]", fileName.c_str());
		return false;
	}

	wstring filePath;
	if (fullPath)
		filePath = fileName;
	else
		filePath = ConfigManager::getSingleton().getConfig()._2d.featuresPath + fileName;

	std::ifstream fin(filePath);
	if (!fin)
	{
		Log::wprint(ELL_ERROR, L"特征脚本[%s]打开失败", filePath.c_str());
		return false;
	}

	fin.seekg(0, ios_base::end);
	if ((int)fin.tellg() == 0)
	{
		Log::wprint(ELL_ERROR, L"特征脚本[%s]为空", filePath.c_str());
		return false;
	}

	fin.seekg(0, ios_base::beg);
	stringstream filestream;
	filestream << fin.rdbuf();
	return parseFeature(filestream);
}

bool FeatureManager::parseFeature(stringstream& filestream)
{
	FeaturePtr     featurePtr;
	string         featureName;
	string         strTag;
	vector<string> container;
	int            singleFeatureParseEndMark = 0;

	do
	{
		std::getline(filestream, strTag);
		strTag = trim(strTag);

		if (0 == strTag.find('//') || 0 == strTag.compare(""))
			continue;

		if (0 == strTag.compare("{"))
		{
			++singleFeatureParseEndMark;
			continue;
		}

		if (0 == strTag.substr(0, 7).compare("feature"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				featureName = container[1];
				featurePtr = FeaturePtr(new Feature(utf8ToUnicode(featureName)));
				continue;
			}
		}

		if (0 == strTag.substr(0, 4).compare("fill"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				featurePtr->_fill = (container[1] == "off" ? false : true);
				continue;
			}
		}

		if (0 == strTag.substr(0, 5).compare("color"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 5)
			{
				float r = (float)atof(container[1].c_str());
				float g = (float)atof(container[2].c_str());
				float b = (float)atof(container[3].c_str());
				float a = (float)atof(container[4].c_str());
				featurePtr->_color = Color(r, g, b, a);
				continue;
			}
		}

		if (0 == strTag.substr(0, 12).compare("stroke_width"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				featurePtr->_strokeWidth = (float)atof(container[1].c_str());
				continue;
			}
		}

		if (0 == strTag.substr(0, 7).compare("picture"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				featurePtr->_picture = Pivot::getSingleton().getPictureManager()->getPicture(utf8ToUnicode(container[1].c_str()));
				continue;
			}
		}

		if (0 == strTag.substr(0, 12).compare("stroke_style"))
			continue;

		if (0 == strTag.substr(0, 9).compare("start_cap"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				unordered_map<string, int>::iterator it = _attribsWordMap.find(container[1]);
				if (it != _attribsWordMap.end())
				{
					featurePtr->_strokeStyle.startCap = (ECapStyle)it->second;
					continue;
				}
			}
		}

		if (0 == strTag.substr(0, 7).compare("end_cap"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				unordered_map<string, int>::iterator it = _attribsWordMap.find(container[1]);
				if (it != _attribsWordMap.end())
				{
					featurePtr->_strokeStyle.endCap = (ECapStyle)it->second;
					continue;
				}
			}
		}

		if (0 == strTag.substr(0, 8).compare("dash_cap"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				unordered_map<string, int>::iterator it = _attribsWordMap.find(container[1]);
				if (it != _attribsWordMap.end())
				{
					featurePtr->_strokeStyle.dashCap = (ECapStyle)it->second;
					continue;
				}
			}
		}

		if (0 == strTag.substr(0, 9).compare("line_join"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				unordered_map<string, int>::iterator it = _attribsWordMap.find(container[1]);
				if (it != _attribsWordMap.end())
				{
					featurePtr->_strokeStyle.lineJoin = (ELineJoin)it->second;
					continue;
				}
			}
		}

		if (0 == strTag.substr(0, 11).compare("miter_limit"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				featurePtr->_strokeStyle.miterLimit = (float)atof(container[1].c_str());
				continue;
			}
		}

		if (0 == strTag.substr(0, 10).compare("dash_style"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				unordered_map<string, int>::iterator it = _attribsWordMap.find(container[1]);
				if (it != _attribsWordMap.end())
				{
					featurePtr->_strokeStyle.dashStyle = (EDashStyle)it->second;
					continue;
				}
			}
		}

		if (0 == strTag.substr(0, 11).compare("dash_offset"))
		{
			container.clear();
			stringtok(container, strTag, " ");
			if (container.size() == 2)
			{
				featurePtr->_strokeStyle.dashOffset = (float)atof(container[1].c_str());
				continue;
			}
		}

		if (0 == strTag.compare("}"))
		{
			--singleFeatureParseEndMark;

			if (0 == singleFeatureParseEndMark)
			{
				if (featureName != "")
					_featurePool[featurePtr->_featureName] = featurePtr;

				featureName.clear();
				featurePtr = nullptr;
				continue;
			}
			else if (singleFeatureParseEndMark > 0)
				continue;
		}

		Log::wprint(ELL_ERROR, L"特征脚本解析失败：[%s:%s]", utf8ToUnicode(featureName).c_str(), utf8ToUnicode(strTag).c_str());
		return false;

	} while (!filestream.eof());

	return true;
}

FeaturePtr FeatureManager::createFeature(const wstring& name /*= L""*/)
{
	static unsigned short idx = 0;
	wstring featureName;
	if (name == L"" || name == L"default_feature")
	{
		std::wstringstream ss;
		ss << L"Feature" << idx;
		++idx;
		featureName = ss.str();
	}
	else
	{
		map<wstring, FeaturePtr>::iterator it = _featurePool.find(name);
		if (it != _featurePool.end())
			return it->second;
		featureName = name;
	}

	FeaturePtr featurePtr(new Feature(featureName));
	_featurePool[featurePtr->_featureName] = featurePtr;
	return featurePtr;
}

FeaturePtr FeatureManager::findFeature(const wstring& name)
{
	if (name == L"" || name == L"default_feature")
		return nullptr;

	map<wstring, FeaturePtr>::iterator it = _featurePool.find(name);
	if (it != _featurePool.end())
		return it->second;

	return nullptr;
}

void FeatureManager::destroyFeature(FeaturePtr feature)
{
	if (feature)
		destroyFeature(feature->_featureName);
}

void FeatureManager::destroyFeature(const wstring& name)
{
	if (name == L"")
		return;

	map<wstring, FeaturePtr>::iterator it = _featurePool.find(name);
	if (it != _featurePool.end())
		_featurePool.erase(it);
}

void FeatureManager::clearAllFeatures()
{
	_featurePool.clear();
}

NS_VAYO2D_END
