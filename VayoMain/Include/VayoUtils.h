/*************************************************************************\
* ��������1.0
* Copyright (c) 2018-2018 authored by ��Ӻ�
* ʵ�ú�������
\*************************************************************************/
#ifndef __VAYO_UTILS_H__
#define __VAYO_UTILS_H__

#include "VayoSupport.h"
#include "VayoVector3d.h"
NS_VAYO_BEGIN

/* ���ֽ��ַ����ָ�� */
template<typename Container>
inline void stringtok(Container& container, const std::string& in,
	const char* const delimiters = " \t\n", const int deep = 0)
{
	const std::string::size_type len = in.length();
	std::string::size_type i = 0;
	int count = 0;
	while (i < len)
	{
		i = in.find_first_not_of(delimiters, i);
		if (i == std::string::npos)
			return; // nothing left
		// find the end of the token
		std::string::size_type j = in.find_first_of(delimiters, i);
		count++;
		// push token
		if (j == std::string::npos || (deep > 0 && count > deep))
		{
			container.push_back(in.substr(i));
			return;
		}
		else
			container.push_back(in.substr(i, j - i));
		// set up for next loop
		i = j + 1;
	}
}

/* ���ַ��ַ����ָ�� */
template<typename Container>
inline void wstringtok(Container& container, const std::wstring& in,
	const wchar_t* const delimiters = L" \t\n", const int deep = 0)
{
	const std::wstring::size_type len = in.length();
	std::wstring::size_type i = 0;
	int count = 0;
	while (i < len)
	{
		i = in.find_first_not_of(delimiters, i);
		if (i == std::wstring::npos)
			return; // nothing left
		// find the end of the token
		std::string::size_type j = in.find_first_of(delimiters, i);
		count++;
		// push token
		if (j == std::wstring::npos || (deep > 0 && count > deep))
		{
			container.push_back(in.substr(i));
			return;
		}
		else
			container.push_back(in.substr(i, j - i));
		// set up for next loop
		i = j + 1;
	}
}

/* �޼������ֽ��ַ�����ǰ��Ŀհ׷� */
static inline string& trim(string& str)
{
	if (!str.empty())
	{
		str.erase(0, str.find_first_not_of(" \n\r\t"));
		str.erase(str.find_last_not_of(" \n\r\t") + 1);
	}
	return str;
}

/* �޼������ַ��ַ�����ǰ��Ŀհ׷� */
static inline wstring& trim(wstring& str)
{
	if (!str.empty())
	{
		str.erase(0, str.find_first_not_of(L" \n\r\t"));
		str.erase(str.find_last_not_of(L" \n\r\t") + 1);
	}
	return str;
}

/* ���ַ�����ֽ��ַ���֮���ת�� */
_VayoExport string   w2a_(const wstring& wideStr);
_VayoExport wstring  a2w_(const string& ansiStr);
_VayoExport string   unicodeToUtf8(const wstring& wstr);
_VayoExport wstring  utf8ToUnicode(const string& str);
_VayoExport string   ansiToUtf8(const string& str);
_VayoExport string   utf8ToAnsi(const string& str);

/* �ַ���ת��ΪСд */
_VayoExport string   strToLower(string str);
_VayoExport wstring  strToLower(wstring str);

/* �ַ����Ƚ� */
_VayoExport bool     compareNoCase(string strA, const string strB);
_VayoExport bool     compareNoCase(wstring strA, const wstring strB);

/* �õ�����Ŀ¼ */
_VayoExport wstring  getWorkingDirectory();

/* ��ָ���ļ�����Ѱ������ָ����ʽ���ļ�·���� */
_VayoExport void findFileDir(vector<wstring>& arrFilePath, const wstring& strCurPath, const wstring& fileFormat);

/* �ַ����滻���� */
_VayoExport string&  strReplaceAll(string& str, const string& old_value, const string& new_value);
_VayoExport wstring& wstrReplaceAll(wstring& str, const wstring& old_value, const wstring& new_value);
_VayoExport string&  strReplaceAllDistinct(string& str, const string& old_value, const string& new_value);
_VayoExport wstring& wstrReplaceAllDistinct(wstring& str, const wstring& old_value, const wstring& new_value);

/* ��ȡ��ͼ�ռ��ڵ�ʰȡ���߷���(ʰȡ���ߵ����Ϊ(0,0,0)) */
_VayoExport Vector3df getRayInViewSpace(int xScreen, int yScreen);

NS_VAYO_END

#endif // __VAYO_UTILS_H__
