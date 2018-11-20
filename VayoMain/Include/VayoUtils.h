/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 authored by 朱加浩
* 实用函数工具
\*************************************************************************/
#ifndef __VAYO_UTILS_H__
#define __VAYO_UTILS_H__

#include "VayoSupport.h"
#include "VayoVector3d.h"
NS_VAYO_BEGIN

/* 多字节字符串分割函数 */
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

/* 宽字符字符串分割函数 */
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

/* 修剪掉多字节字符串里前后的空白符 */
static inline string& trim(string& str)
{
	if (!str.empty())
	{
		str.erase(0, str.find_first_not_of(" \n\r\t"));
		str.erase(str.find_last_not_of(" \n\r\t") + 1);
	}
	return str;
}

/* 修剪掉宽字符字符串里前后的空白符 */
static inline wstring& trim(wstring& str)
{
	if (!str.empty())
	{
		str.erase(0, str.find_first_not_of(L" \n\r\t"));
		str.erase(str.find_last_not_of(L" \n\r\t") + 1);
	}
	return str;
}

/* 宽字符与多字节字符串之间的转换 */
_VayoExport string   w2a_(const wstring& wideStr);
_VayoExport wstring  a2w_(const string& ansiStr);
_VayoExport string   unicodeToUtf8(const wstring& wstr);
_VayoExport wstring  utf8ToUnicode(const string& str);
_VayoExport string   ansiToUtf8(const string& str);
_VayoExport string   utf8ToAnsi(const string& str);

/* 字符串转换为小写 */
_VayoExport string   strToLower(string str);
_VayoExport wstring  strToLower(wstring str);

/* 字符串比较 */
_VayoExport bool     compareNoCase(string strA, const string strB);
_VayoExport bool     compareNoCase(wstring strA, const wstring strB);

/* 得到工作目录 */
_VayoExport wstring  getWorkingDirectory();

/* 在指定文件夹下寻找所有指定格式的文件路径名 */
_VayoExport void findFileDir(vector<wstring>& arrFilePath, const wstring& strCurPath, const wstring& fileFormat);

/* 字符串替换函数 */
_VayoExport string&  strReplaceAll(string& str, const string& old_value, const string& new_value);
_VayoExport wstring& wstrReplaceAll(wstring& str, const wstring& old_value, const wstring& new_value);
_VayoExport string&  strReplaceAllDistinct(string& str, const string& old_value, const string& new_value);
_VayoExport wstring& wstrReplaceAllDistinct(wstring& str, const wstring& old_value, const wstring& new_value);

/* 获取视图空间内的拾取射线方向(拾取射线的起点为(0,0,0)) */
_VayoExport Vector3df getRayInViewSpace(int xScreen, int yScreen);

NS_VAYO_END

#endif // __VAYO_UTILS_H__
