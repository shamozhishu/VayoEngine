#include "VayoLanguage.h"
#include "VayoConfigManager.h"
#include "VayoRoot.h"
#include "VayoLog.h"

#define MAX_BUFF_SIZE (1024 * 512)

NS_VAYO_BEGIN

static wstring matchFormatCSV(int id, const wstring& content)
{
	wstring strRet;
	wstring strContent = content;
	wchar_t szid[64] = {};
	_snwprintf(szid, sizeof(szid), L"%d,", id);
	strRet = szid;

	if (0 == strContent.compare(L""))
	{
		strRet += L"\r\n";
		return strRet;
	}

	if (std::string::npos != strContent.find(L",") || L'\"' == strContent[0])
	{
		wstrReplaceAllDistinct(strContent, L"\"", L"\"\"");
		strRet += (L"\"" + strContent + L"\"" + L"\r\n");
	}
	else
	{
		strRet += (strContent + L"\r\n");
	}

	return strRet;
}

bool writeToCSV(const wstring& fileName)
{
	wofstream fout(fileName, std::ios::binary);
	fout.imbue(locale("chs"));
	if (!fout)
		return false;

	struct tagTemp
	{
		int LanID;
		wstring LanContent;

		tagTemp(int id, const wstring& strLan) : LanID(id), LanContent(strLan) {}
		static bool comp(const tagTemp& var1, const tagTemp& var2)
		{
			return var1.LanID < var2.LanID;
		}
	};

	vector<tagTemp> arrLanPack;
	wstring strBuffer;
	unsigned int len = Root::getSingleton().getLanguage()->_lanPack.size();
	for (unsigned i = 0; i < len; ++i)
	{
		const unordered_map<int, wstring>* curLanPack = Root::getSingleton().getLanguage()->_lanPack[i].Lan;
		unordered_map<int, wstring>::const_iterator citor = curLanPack->cbegin();
		unordered_map<int, wstring>::const_iterator citorEnd = curLanPack->end();
		for (; citor != citorEnd; ++citor)
		{
			int id = citor->first;
			strBuffer = matchFormatCSV(id, citor->second);
			arrLanPack.push_back(tagTemp(id, strBuffer));
		}

		std::sort(arrLanPack.begin(), arrLanPack.end(), tagTemp::comp);
	}

	std::sort(arrLanPack.begin(), arrLanPack.end(), tagTemp::comp);

	len = arrLanPack.size();
	for (unsigned i = 0; i < len; ++i)
	{
		fout << arrLanPack[i].LanContent;
	}

	fout << std::endl;
	fout.close();
	return true;
}

//-------------------------------------------------------------------------
// Language class.
//-------------------------------------------------------------------------
Language::~Language()
{
	size_t size = _lanPack.size();
	for (size_t i = 0; i < size; ++i)
		_lanPack[i].destroy();
	_lanPack.clear();
}

bool Language::init()
{
	wstring strFileName;
	wstring strBuff;
	strBuff.resize(MAX_BUFF_SIZE);
	const vector<tagLanguageConfig>& languageConfig = Root::getSingleton().getConfigManager()->getLanguageConfig();

	for (size_t i = 0; i < languageConfig.size(); ++i)
	{
		strFileName = languageConfig[i].FilePath;
		wifstream fin(strFileName);
		fin.imbue(locale("chs"));
		if (fin)
		{
			size_t idx = strFileName.rfind(L"\\");
			strFileName = strFileName.substr(++idx);
			tagLanWrapper lan;
			lan.FileName = strFileName;
			_lanPack.push_back(lan);
			_lanPack.back().create();

			loadLanPack(strBuff, fin);
			fin.close();
		}
		else
			Log::wprint(ELL_ERROR, L"语言包文件[%s]打开失败", strFileName.c_str());
	}

	std::sort(_lanPack.begin(), _lanPack.end(), tagLanWrapper::comp);
	return true;
}

wstring Language::getLanguageByID(int id)
{
	int arrSize = (int)_lanPack.size();
	for (int i = 0; i < arrSize; ++i)
	{
		if (id < _lanPack[i].MaxID)
		{
			unordered_map<int, wstring>::iterator itor = _lanPack[i].Lan->find(id);
			if (itor == _lanPack[i].Lan->end())
				return L"";
			return itor->second;
		}
	}
	return L"";
}

void Language::loadLanPack(wstring& strBuff, wifstream& fin)
{
	fin.seekg(0, std::ios_base::end);
	int sizeToRead = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	fin.read(&strBuff[0], sizeToRead);
	sizeToRead = fin.gcount();
	int textId = -1;
	wstring strPack;
	bool between = true; // 只有位于换行和逗号之间的数字才作为语言包ID处理(包括了对第一行这个特殊情况的处理)

	for (int i = 0; i < sizeToRead; ++i)
	{
		wchar_t curCharacter = strBuff[i];

		if (L',' == curCharacter && between)
		{
			between = false;
			textId = _wtoi(strPack.c_str());
			strPack.clear();
		}
		else if (L'\n' == curCharacter || L'\r' == curCharacter)
		{
			between = true;
			_lanPack.back().Lan->insert(make_pair(textId, strPack));
			strPack.clear();
		}
		else
		{
			strPack += curCharacter;
		}

		if (sizeToRead - 1 == i)
		{
			_lanPack.back().Lan->insert(make_pair(textId, strPack));
		}
	}
}

NS_VAYO_END
