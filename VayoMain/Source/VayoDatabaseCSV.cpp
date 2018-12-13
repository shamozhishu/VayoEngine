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
TableCSV::TableCSV(DatabaseCSV* csvdb)
	: _associatedDB(csvdb)
{
}

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
	unordered_map<wstring, vector<ptrdiff_t>>::iterator findData = _tableData.find(rowIdx);
	if (findData != _tableData.end())
	{
		const vector<ptrdiff_t>& arrRowText = findData->second;
		if (indexField >= 0 && indexField < (int)arrRowText.size() && arrRowText[indexField] != -1)
			return _associatedDB->_buffer + arrRowText[indexField];
	}
	return L"";
}

const wchar_t* TableCSV::item2str(int rowIdx, const wchar_t* colIdx)
{
	wchar_t szRowIdx[BUFFER_SIZE] = {};
	_snwprintf(szRowIdx, BUFFER_SIZE, L"%d", rowIdx);
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
	_snwprintf(szRowIdx, BUFFER_SIZE, L"%d", rowIdx);
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
	_snwprintf(szRowIdx, BUFFER_SIZE, L"%d", rowIdx);
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
	_snwprintf(szRowIdx, BUFFER_SIZE, L"%d", rowIdx);
	return item2float(szRowIdx, colIdx);
}

//-------------------------------------------------------------------------
// DatabaseCSV class.
//-------------------------------------------------------------------------
DatabaseCSV::DatabaseCSV() 
	: _buffer(NULL)
	, _canInit(true)
	, _curCharOffset(0)
	, _maxBufferSize(BUFFER_MAX_SIZE * BUFFER_MAX_SIZE * 3)
	, _startValidLine(2)
{
}

DatabaseCSV::~DatabaseCSV()
{
	destroy();
}

bool DatabaseCSV::init()
{
	if (_canInit)
	{
		destroy();
		wstring csvTablePath = Root::getSingleton().getConfigManager()->getUIConfig().TableCSVPath;
		vector<wstring> allFilePath;
		findFileDir(allFilePath, csvTablePath, L"csv");

		wstring tableDataBuff;
		unsigned len = allFilePath.size();
		if (len > 0)
			tableDataBuff.resize(_maxBufferSize);

		for (unsigned i = 0; i < len; i++)
			loadTable(wstrReplaceAll(allFilePath[i], csvTablePath, L""), tableDataBuff);

		_canInit = false;
	}

	return true;
}

bool DatabaseCSV::loadTable(const wstring& fileName, wstring& tableDataBuff)
{
	wstring csvTablePath = Root::getSingleton().getConfigManager()->getUIConfig().TableCSVPath;
	wstring fileFullName = csvTablePath + fileName;
	fileFullName = wstrReplaceAll(fileFullName, L"/", L"\\");
	if (fileName.rfind(L".csv") == wstring::npos)
	{
		Log::wprint(ELL_WARNING, L"CSV表[%s]文件格式错误", fileFullName.c_str());
		return false;
	}

	lazyInit();

	wifstream fin(fileFullName);
	if (!fin)
	{
		Log::wprint(ELL_ERROR, L"CSV表[%s]打开失败", fileFullName.c_str());
		return false;
	}

	fin.imbue(locale("chs"));
	fin.seekg(0, std::ios_base::end);
	int sizeToRead = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	if (sizeToRead == 0)
	{
		Log::wprint(ELL_ERROR, L"CSV表[%s]为空", fileFullName.c_str());
		return false;
	}

	if (tableDataBuff.size() < sizeToRead)
	{
		tableDataBuff.clear();
		tableDataBuff.resize(sizeToRead);
	}

	fin.read(&tableDataBuff[0], sizeToRead);
	sizeToRead = (int)fin.gcount();
	if (sizeToRead == 0)
	{
		Log::wprint(ELL_ERROR, L"CSV表[%s]读取失败", fileFullName.c_str());
		return false;
	}

	wchar_t buff[BUFFER_MAX_SIZE * 4];
	int lineNum = 0;

	wchar_t* p = &tableDataBuff[0];
	int readCharOffset = 0;
	TableCSV* pTable = NULL;

	csvTablePath = wstrReplaceAll(fileFullName, csvTablePath, L"");
	unordered_map<wstring, TableCSV*>::iterator it = _tables.find(csvTablePath);
	if (it != _tables.end())
	{
		pTable = it->second;
		pTable->_tableName.clear();
		pTable->_fieldName.clear();
		pTable->_tableData.clear();
	}
	else
	{
		pTable = new TableCSV(this);
		pTable->_tableName = csvTablePath;
		_tables[pTable->_tableName] = pTable;
	}

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
	_curCharOffset = 0;
	_canInit = true;
	if (_buffer)
	{
		free(_buffer);
		_buffer = NULL;
	}
}

int DatabaseCSV::getTableCount() const
{
	return (int)_tables.size();
}

TableCSV* DatabaseCSV::getTable(const wstring& tableName)
{
	wstring strTableName = tableName;
	strTableName = wstrReplaceAll(strTableName, L"/", L"\\");
	unordered_map<wstring, TableCSV*>::iterator findVal = _tables.find(strTableName);
	if (findVal != _tables.end())
		return findVal->second;
	return NULL;
}

void DatabaseCSV::lazyInit()
{
	if (NULL != _buffer)
		return;

	_buffer = (wchar_t*)malloc(_maxBufferSize * sizeof(wchar_t));
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

	vector<ptrdiff_t> tableDataOffset;

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
					tableDataOffset.push_back(pDst - _buffer);
				}
				else
				{
					tableDataOffset.push_back(-1);
				}

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
					tableDataOffset.push_back(pDst - _buffer);
				}
				else
				{
					tableDataOffset.push_back(-1);
				}

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
			tableDataOffset.push_back(pDst - _buffer);
		}
		else
		{
			tableDataOffset.push_back(-1);
		}
	}

	if (tableDataOffset.size() <= 0)
	{
		return;
	}

	wstring idx = _buffer + tableDataOffset[0];
	pTable->_tableData[idx] = tableDataOffset;

	int restNum = pTable->_fieldName.size() - tableDataOffset.size();

	for (int i = 0; i < restNum + 1; i++)
	{
		pTable->_tableData[idx].push_back(-1);
	}
}

wchar_t* DatabaseCSV::getCurCharBuff(int len)
{
	if (NULL == _buffer)
		return NULL;

	if ((_curCharOffset + len) >= _maxBufferSize - BUFFER_MAX_SIZE * BUFFER_MAX_SIZE)
		_canInit = true;

	if ((_curCharOffset + len) >= _maxBufferSize)
	{
		int bufferSize = (BUFFER_MAX_SIZE * BUFFER_MAX_SIZE + _maxBufferSize) * sizeof(wchar_t);
		void* pNewBuffer = realloc(_buffer, bufferSize);
		if (pNewBuffer)
		{
			_buffer = (wchar_t*)pNewBuffer;
			_maxBufferSize = bufferSize;
		}
		else
			return NULL;
	}

	wchar_t* pRet = _buffer + _curCharOffset;
	_curCharOffset += len;
	return pRet;
}

NS_VAYO_END
