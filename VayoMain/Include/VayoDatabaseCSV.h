/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* 以逗号分隔的文本文件解析器
\*************************************************************************/
#ifndef __VAYO_CSV_DATABASE_H__
#define __VAYO_CSV_DATABASE_H__

#include "VayoSupport.h"
NS_VAYO_BEGIN

class _VayoExport TableCSV
{
	friend class DatabaseCSV;
public:
	int            getItemCount() const;
	const wstring& getTableName() const;

	const wchar_t* item2str(const wchar_t* rowIdx, const wchar_t* colIdx);
	const wchar_t* item2str(int rowIdx, const wchar_t* colIdx);

	int            item2int(const wchar_t* rowIdx, const wchar_t* colIdx);
	int            item2int(int rowIdx, const wchar_t* colIdx);

	unsigned int   item2uint(const wchar_t* rowIdx, const wchar_t* colIdx);
	unsigned int   item2uint(int rowIdx, const wchar_t* colIdx);

	float          item2float(const wchar_t* rowIdx, const wchar_t* colIdx);
	float          item2float(int rowIdx, const wchar_t* colIdx);

private:
	wstring                                        _tableName;
	unordered_map<wstring, int>                    _fieldName;
	unordered_map<wstring, vector<const wchar_t*>> _tableData;
};

class _VayoExport DatabaseCSV
{
	DISALLOW_COPY_AND_ASSIGN(DatabaseCSV)
public:
	DatabaseCSV();
	~DatabaseCSV();
	bool init();
	bool loadTable(const wstring& filePath, wstring& tableDataBuff);
	void destroy();
	TableCSV* getTable(const wstring& strTableName);
	PROPERTY_RW(int, _startValidLine, StartValidLine)

private:
	void     lazyInit();
	void     parseTextLine(TableCSV* pTable, const wchar_t* pszLine, int lineNum);
	wchar_t* getCurCharBuff(int len);

private:
	const int                         _maxBufferSize;
	wchar_t*                          _buffer;
	int                               _curCharOffset;
	unordered_map<wstring, TableCSV*> _tables;
};

NS_VAYO_END

#endif // __VAYO_CSV_DATABASE_H__
