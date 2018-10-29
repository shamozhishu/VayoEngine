#include "VayoUtils.h"
#include <Windows.h>
#include <io.h>
#include "VayoLog.h"
#include "VayoRoot.h"
#include "VayoCamera.h"
#include "VayoDevice.h"
#include "VayoRenderSystem.h"
#include "VayoMatrix4x4.h"
#include "VayoSceneManager.h"

NS_VAYO_BEGIN

string w2a_(const wstring& wideStr)
{
	if (wideStr == L"")
		return "";

	string ansiStr;
	int needBytes = WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), wideStr.size(), NULL, 0, NULL, NULL);
	if (needBytes > 0)
	{
		ansiStr.resize(needBytes);
		WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(), wideStr.size(), &ansiStr[0], needBytes, NULL, NULL);
	}

	return ansiStr;
}

wstring a2w_(const string& ansiStr)
{
	if (ansiStr == "")
		return L"";

	wstring wideStr;
	int needWChar = MultiByteToWideChar(CP_ACP, 0, ansiStr.c_str(), ansiStr.size(), NULL, 0);
	if (needWChar > 0)
	{
		wideStr.resize(needWChar);
		MultiByteToWideChar(CP_ACP, 0, ansiStr.c_str(), ansiStr.size(), &wideStr[0], needWChar);
	}

	return wideStr;
}

string unicodeToUtf8(const wstring& wstr)
{
	int ansiiLen = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	char *pAssii = (char*)malloc(sizeof(char)*ansiiLen);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, pAssii, ansiiLen, NULL, NULL);
	string ret_str = pAssii;
	free(pAssii);
	return ret_str;
}

wstring utf8ToUnicode(const string& str)
{
	int unicodeLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	wchar_t *pUnicode = (wchar_t*)malloc(sizeof(wchar_t)*unicodeLen);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, pUnicode, unicodeLen);
	wstring ret_str = pUnicode;
	free(pUnicode);
	return ret_str;
}

string ansiToUtf8(const string& str)
{
	return unicodeToUtf8(a2w_(str));
}

string utf8ToAnsi(const string& str)
{
	return w2a_(utf8ToUnicode(str));
}

string strToLower(string str)
{
	std::string strTmp = str;
	std::transform(strTmp.begin(), strTmp.end(), strTmp.begin(), towupper);
	return strTmp;
}

wstring strToLower(wstring str)
{
	std::wstring strTmp = str;
	std::transform(strTmp.begin(), strTmp.end(), strTmp.begin(), towupper);
	return strTmp;
}

bool compareNoCase(string strA, const string strB)
{
	string str1 = strToLower(strA);
	string str2 = strToLower(strB);
	return (str1 == str2);
}

bool compareNoCase(wstring strA, const wstring strB)
{
	wstring str1 = strToLower(strA);
	wstring str2 = strToLower(strB);
	return (str1 == str2);
}

wstring getWorkingDirectory()
{
	wchar_t tmp[_MAX_PATH];
	_wgetcwd(tmp, _MAX_PATH);
	wstring workDir = tmp;
	workDir += L"\\";
	return workDir;
}

void findFileDir(vector<wstring>& arrFilePath, const wstring& strCurPath, const wstring& fileFormat)
{
	wstring searchPath = strCurPath + L"*";
	_wfinddata_t sFileInfo;
	intptr_t handle = _wfindfirst(searchPath.c_str(), &sFileInfo);
	if (-1l == handle)
	{
		Log::wprint(ELL_ERROR, L"Can't match the folder path!");
		return; // 返回
	}

	do
	{
		// 判断是否包含子目录
		if (sFileInfo.attrib & _A_SUBDIR)
		{
			if (0 != wcscmp(sFileInfo.name, L".") && 0 != wcscmp(sFileInfo.name, L".."))
			{
				wstring newPath = strCurPath + sFileInfo.name + L"\\";
				findFileDir(arrFilePath, newPath, fileFormat);
			}
			continue;
		}

		wstring resultPath = strCurPath + sFileInfo.name;
		wstring strExName = resultPath;
		size_t idx = strExName.rfind('.');
		strExName = strExName.substr(++idx);
		if (0 == strExName.compare(fileFormat))
		{
			arrFilePath.push_back(resultPath);
		}

	} while (0 == _wfindnext(handle, &sFileInfo));

	_findclose(handle);
}

string& strReplaceAll(string& str, const string& old_value, const string& new_value)
{
	while (true)
	{
		string::size_type pos(0);
		if ((pos = str.find(old_value)) != string::npos)
			str.replace(pos, old_value.length(), new_value);
		else
			break;
	}
	return str;
}

wstring& wstrReplaceAll(wstring& str, const wstring& old_value, const wstring& new_value)
{
	while (true)
	{
		wstring::size_type pos(0);
		if ((pos = str.find(old_value)) != wstring::npos)
			str.replace(pos, old_value.length(), new_value);
		else
			break;
	}
	return str;
}

string& strReplaceAllDistinct(string& str, const string& old_value, const string& new_value)
{
	for (string::size_type pos(0); pos != string::npos; pos += new_value.length())
	{
		if ((pos = str.find(old_value, pos)) == string::npos)
			break;
		str.replace(pos, old_value.length(), new_value);
	}
	return str;
}

wstring& wstrReplaceAllDistinct(wstring& str, const wstring& old_value, const wstring& new_value)
{
	for (wstring::size_type pos(0); pos != wstring::npos; pos += new_value.length())
	{
		if ((pos = str.find(old_value, pos)) != wstring::npos)
			str.replace(pos, old_value.length(), new_value);
		else
			break;
	}
	return str;
}

Vector3df getRayInViewSpace(int xScreen, int yScreen)
{
	Camera* pCamera = Root::getSingleton().getCurSceneMgr()->getActiveCamera();
	if (!pCamera)
		return Vector3df::Origin;

	Matrix4x4 projMat = pCamera->getProj();
	if (!projMat.makeInverse())
		return Vector3df::Origin;

	Recti viewPort = Root::getSingleton().getActiveRenderer()->getViewPort();
	float xN = (float)(xScreen - viewPort._upperLeftCorner._x) * 2.0f / (float)viewPort.getWidth() - 1.0f;
	float yN = (float)(-yScreen + viewPort._upperLeftCorner._y) * 2.0f / (float)viewPort.getHeight() + 1.0f;
	float wC = pCamera->getNearZ();
	bool isOrtho = pCamera->isOrthogonal();
	float xC = isOrtho ? xN : xN * wC;
	float yC = isOrtho ? yN : yN * wC;
	Vector3df rayDir(xC, yC, 0.0f);
	projMat.transformVect(rayDir);
	rayDir.normalize();
	return rayDir;
}

NS_VAYO_END
