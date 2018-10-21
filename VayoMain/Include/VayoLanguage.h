/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* TXT语言包文本文件解析器
\*************************************************************************/
#ifndef __VAYO_LANGUAGE_H__
#define __VAYO_LANGUAGE_H__

#include "VayoSupport.h"
#include "VayoUtils.h"

#define _START_POS_ (9)

NS_VAYO_BEGIN

_VayoExport bool writeToCSV(const wstring& fileName);

//=========================================================================
// Language 类
//=========================================================================
class _VayoExport Language
{
	friend bool writeToCSV(const wstring& fileName);
	DISALLOW_COPY_AND_ASSIGN(Language)
public:
	Language() {}
	~Language();
	bool init();
	wstring getLanguageByID(int id);

private:
	void loadLanPack(wstring& strBuff, wifstream& fin);

private:
	struct tagLanWrapper
	{
		unordered_map<int, wstring>* Lan;
		wstring                      FileName;
		int                          MinID;
		int                          MaxID;
		static bool comp(const tagLanWrapper& var1, const tagLanWrapper& var2)
		{
			return var1.MaxID < var2.MaxID;
		}
		tagLanWrapper() : Lan(NULL), MinID(0), MaxID(0) {}
		void create()
		{
			Lan = new unordered_map<int, wstring>();
			getIDFromFileName(MinID, MaxID, FileName);
		}
		void destroy()
		{
			SAFE_DELETE(Lan);
		}

	private:
		void getIDFromFileName(int& minID, int& maxID, const wstring& fileName)
		{
			minID = 0;
			maxID = 0;
			wstring strVal = fileName;
			strVal = strVal.substr(_START_POS_);
			vector<wstring> vecStrBag;
			wstringtok(vecStrBag, strVal, L"_.");

			if (vecStrBag.size() >= 2)
			{
				minID = _wtoi(vecStrBag[0].c_str());
				maxID = _wtoi(vecStrBag[1].c_str());
			}
		}
	};
	vector<tagLanWrapper> _lanPack;
};

NS_VAYO_END

#endif // __VAYO_LANGUAGE_H__
