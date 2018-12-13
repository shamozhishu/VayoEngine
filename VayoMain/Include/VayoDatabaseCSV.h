/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 authored by 朱加浩
* 以逗号分隔的文本文件解析器
\*************************************************************************/
#ifndef __VAYO_DATABASE_CSV_H__
#define __VAYO_DATABASE_CSV_H__

#include "VayoSupport.h"
NS_VAYO_BEGIN

class _VayoExport TableCSV
{
	friend class DatabaseCSV;
	TableCSV(DatabaseCSV* csvdb);
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
	wstring                                   _tableName;
	unordered_map<wstring, int>               _fieldName;
	unordered_map<wstring, vector<ptrdiff_t>> _tableData;
	DatabaseCSV*                              _associatedDB;
};

class _VayoExport DatabaseCSV
{
	friend class TableCSV;
	DISALLOW_COPY_AND_ASSIGN(DatabaseCSV)
public:
	DatabaseCSV();
	~DatabaseCSV();
	bool init();
	bool loadTable(const wstring& fileName, wstring& tableDataBuff);
	void destroy();
	int  getTableCount() const;
	TableCSV* getTable(const wstring& tableName);

private:
	void     lazyInit();
	void     parseTextLine(TableCSV* pTable, const wchar_t* pszLine, int lineNum);
	wchar_t* getCurCharBuff(int len);

private:
	wchar_t*                          _buffer;
	unordered_map<wstring, TableCSV*> _tables;
	bool                              _canInit;
	int                               _curCharOffset;
	int                               _maxBufferSize;
	const int                         _startValidLine;
};

NS_VAYO_END

#endif // __VAYO_DATABASE_CSV_H__
