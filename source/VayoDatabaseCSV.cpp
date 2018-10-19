#include "VayoDatabaseCSV.h"
#include "VayoUtils.h"
#include "VayoRoot.h"
#include "VayoLog.h"
#include "VayoConfigManager.h"

#define BUFFER_SIZE        (64)
#define BUFFER_MAX_SIZE    (1024)

NS_VAYO_BEGIN

//-------------------------------------------------------------------------
// TableCSV class.
//-------------------------------------------------------------------------
int TableCSV::getItemCount() const
{
	return (int)_tableData.size();
}

const wstring& TableCSV::getTableName() const
{
	return _tableName;
}

const wchar_t* TableCSV::item2str(const wchar_t* rowIdx, const wchar_t* colIdx)
{
	int indexField = 0;
	unordered_map<wstring, int>::iterator findVal = _fieldName.find(colIdx);
	if (findVal == _fieldName.end())
		return L"";
	indexField = findVal->second;
	unordered_map<wstring, vector<const wchar_t*>>::iterator findData = _tableData.find(rowIdx);
	if (findData != _tableData.end())
	{
		const vector<const wchar_t*>& arrRowText = findData->second;
		if (indexField >= 0 && indexField < (int)arrRowText.size())
			return arrRowText[indexField];
	}
	return L"";
}

const wchar_t* TableCSV::item2str(int rowIdx, const wchar_t* colIdx)
{
	wchar_t szRowIdx[BUFFER_SIZE] = {};
	_snwprintf(szRowIdx, sizeof(szRowIdx), L"%d", rowIdx);
	return item2str(szRowIdx, colIdx);
}

int TableCSV::item2int(const wchar_t* rowIdx, const wchar_t* colIdx)
{
	const wchar_t* val = item2str(rowIdx, colIdx);
	if (NULL == val)
		return 0;
	return _wtoi(val);
}

int TableCSV::item2int(int rowIdx, const wchar_t* colIdx)
{
	wchar_t szRowIdx[BUFFER_SIZE] = {};
	_snwprintf(szRowIdx, sizeof(szRowIdx), L"%d", rowIdx);
	return item2int(szRowIdx, colIdx);
}

unsigned int TableCSV::item2uint(const wchar_t* rowIdx, const wchar_t* colIdx)
{
	const wchar_t* val = item2str(rowIdx, colIdx);
	if (NULL == val)
		return 0u;
	return (unsigned int)_wtoi(val);
}

unsigned int TableCSV::item2uint(int rowIdx, const wchar_t* colIdx)
{
	wchar_t szRowIdx[BUFFER_SIZE] = {};
	_snwprintf(szRowIdx, sizeof(szRowIdx), L"%d", rowIdx);
	return item2uint(szRowIdx, colIdx);
}

float TableCSV::item2float(const wchar_t* rowIdx, const wchar_t* colIdx)
{
	const wchar_t* val = item2str(rowIdx, colIdx);
	if (NULL == val || 0 == wcscmp(val, L""))
		return 0.0f;
	return std::stof(val);
}

float TableCSV::item2float(int rowIdx, const wchar_t* colIdx)
{
	wchar_t szRowIdx[BUFFER_SIZE] = {};
	_snwprintf(szRowIdx, sizeof(szRowIdx), L"%d", rowIdx);
	return item2float(szRowIdx, colIdx);
}

//-------------------------------------------------------------------------
// DatabaseCSV class.
//-------------------------------------------------------------------------
DatabaseCSV::DatabaseCSV()
	: _maxBufferSize(BUFFER_MAX_SIZE * BUFFER_MAX_SIZE * 3)
	, _buffer(NULL)
	, _curCharOffset(0)
	, _startValidLine(2)
{
}

DatabaseCSV::~DatabaseCSV()
{
	destroy();
}

bool DatabaseCSV::init()
{
	wstring csvTablePath = Root::getSingleton().getConfigManager()->getUIConfig().TableCSVPath;
	vector<wstring> allFilePath;
	findFileDir(allFilePath, csvTablePath, L"csv");

	wstring tableDataBuff;
	unsigned len = allFilePath.size();
	if (len > 0)
		tableDataBuff.resize(_maxBufferSize);

	for (unsigned i = 0; i < len; i++)
	{
		if (!loadTable(allFilePath[i], tableDataBuff))
			Log::wprint(ELL_ERROR, L"表数据文件[%s]加载失败", allFilePath[i].c_str());
	}

	return true;
}

bool DatabaseCSV::loadTable(const wstring& filePath, wstring& tableDataBuff)
{
	lazyInit();

	wifstream fin(filePath);
	if (!fin)
		return false;

	fin.imbue(locale("chs"));
	fin.seekg(0, std::ios_base::end);
	int sizeToRead = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	fin.read(&tableDataBuff[0], sizeToRead);
	sizeToRead = (int)fin.gcount();
	if (sizeToRead == 0)
		return false;

	wchar_t buff[BUFFER_MAX_SIZE * 4];
	int lineNum = 0;

	wchar_t* p = &tableDataBuff[0];
	int readCharOffset = 0;
	TableCSV* pTable = new TableCSV();
	wstring strFileName = filePath;
	size_t idx = strFileName.rfind(L'\\');
	pTable->_tableName = strFileName.substr(++idx);
	_tables[pTable->_tableName] = pTable;

	for (; sizeToRead > 0; p++, sizeToRead--)
	{
		wchar_t curCharacter = *p;

		if (L'\n' == curCharacter || L'\r' == curCharacter)
		{
			if (readCharOffset > 0)
			{
				lineNum++;
				buff[readCharOffset] = L'\0';
				parseTextLine(pTable, buff, lineNum);
				readCharOffset = 0;
			}

			continue;
		}

		if (readCharOffset >= 0 && readCharOffset < BUFFER_MAX_SIZE * 4)
		{
			buff[readCharOffset++] = curCharacter;
		}
	}

	if (readCharOffset > 0)
	{
		lineNum++;
		buff[readCharOffset] = L'\0';
		parseTextLine(pTable, buff, lineNum);
	}

	return true;
}

void DatabaseCSV::destroy()
{
	unordered_map<wstring, TableCSV*>::iterator itor = _tables.begin();
	for (; itor != _tables.end(); ++itor)
		SAFE_DELETE(itor->second);
	_tables.clear();
	SAFE_DELETE_ARRAY(_buffer);
	_buffer = NULL;
	_curCharOffset = 0;
}

TableCSV* DatabaseCSV::getTable(const wstring& strTableName)
{
	unordered_map<wstring, TableCSV*>::iterator findVal = _tables.find(strTableName);
	if (findVal != _tables.end())
		return findVal->second;
	return NULL;
}

void DatabaseCSV::lazyInit()
{
	if (NULL != _buffer)
		return;

	_buffer = new wchar_t[_maxBufferSize];
	_curCharOffset = 0;
}

void DatabaseCSV::parseTextLine(TableCSV* pTable, const wchar_t* pszLine, int lineNum)
{
	static wchar_t buff[BUFFER_MAX_SIZE];
	if (lineNum < _startValidLine)
		return;

	int len = wcslen(pszLine);
	int curByteOffset = 0;
	buff[curByteOffset] = 0;
	wchar_t firstChar = 0;
	int semicolonNum = 0;

	// 解析字段名文本行
	if (_startValidLine == lineNum)
	{
		int fieldIndex = 0;

		for (int i = 0; i < len; i++)
		{
			if (0 == curByteOffset)
				firstChar = pszLine[i];

			if (L'"' != firstChar)
			{
				if (L',' == pszLine[i])
				{
					buff[curByteOffset] = 0;
					pTable->_fieldName[buff] = fieldIndex++;
					curByteOffset = 0;
					continue;
				}

				buff[curByteOffset++] = pszLine[i];
			}
			else
			{
				if (L'"' == pszLine[i])
				{
					++semicolonNum;
				}

				if (0 == semicolonNum % 2 && L'"' == pszLine[i - 1] && L',' == pszLine[i])
				{
					buff[curByteOffset] = 0;
					pTable->_fieldName[buff] = fieldIndex++;
					curByteOffset = 0;
					semicolonNum = 0;
					continue;
				}

				if (L'"' == pszLine[i] && semicolonNum > 1 && 0 == semicolonNum % 2)
				{
					continue;
				}

				buff[curByteOffset++] = pszLine[i];
			}
		}

		if (curByteOffset > 0)
		{
			buff[curByteOffset] = 0;
			pTable->_fieldName[buff] = fieldIndex;
		}

		return; // 返回
	}

	vector<const wchar_t*> tmp;

	for (int i = 0; i < len; i++)
	{
		if (0 == curByteOffset)
			firstChar = pszLine[i];

		if (L'"' != firstChar)
		{
			if (L',' == pszLine[i])
			{
				buff[curByteOffset] = 0;
				wchar_t* pDst = getCurCharBuff(curByteOffset + 1);

				if (NULL != pDst)
				{
					if (curByteOffset > 0)
						memmove(pDst, buff, sizeof(wchar_t)*curByteOffset);

					pDst[curByteOffset] = 0;
				}
				else
				{
					pDst = L"";
				}

				tmp.push_back(pDst);
				curByteOffset = 0;
				continue;
			}

			buff[curByteOffset++] = pszLine[i];
		}
		else
		{
			if (L'"' == pszLine[i])
			{
				++semicolonNum;
			}

			if (0 == semicolonNum % 2 && L'"' == pszLine[i - 1] && L',' == pszLine[i])
			{
				buff[curByteOffset] = 0;
				wchar_t* pDst = getCurCharBuff(curByteOffset + 1);

				if (NULL != pDst)
				{
					if (curByteOffset > 0)
						memmove(pDst, buff, sizeof(wchar_t)*curByteOffset);

					pDst[curByteOffset] = 0;
				}
				else
				{
					pDst = L"";
				}

				tmp.push_back(pDst);
				curByteOffset = 0;
				semicolonNum = 0;
				continue;
			}

			if (L'"' == pszLine[i] && semicolonNum > 1 && 0 == semicolonNum % 2)
			{
				continue;
			}

			buff[curByteOffset++] = pszLine[i];
		}
	}

	if (curByteOffset > 0)
	{
		buff[curByteOffset] = 0;
		wchar_t* pDst = getCurCharBuff(curByteOffset + 1);

		if (NULL != pDst)
		{
			memmove(pDst, buff, sizeof(wchar_t)*curByteOffset);
			pDst[curByteOffset] = 0;
		}
		else
		{
			pDst = L"";
		}

		tmp.push_back(pDst);
	}

	if (tmp.size() <= 0)
	{
		return;
	}

	wstring idx = tmp[0];
	pTable->_tableData[idx] = tmp;

	int restNum = pTable->_fieldName.size() - tmp.size();

	for (int i = 0; i < restNum + 1; i++)
	{
		pTable->_tableData[idx].push_back(L"");
	}

	tmp.clear();
}

wchar_t* DatabaseCSV::getCurCharBuff(int len)
{
	if (NULL == _buffer || (_curCharOffset + len) >= _maxBufferSize)
		return NULL;

	wchar_t* pRet = _buffer + _curCharOffset;
	_curCharOffset += len;
	return pRet;
}

NS_VAYO_END
