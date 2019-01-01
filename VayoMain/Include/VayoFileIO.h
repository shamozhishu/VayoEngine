/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 文件IO
\*************************************************************************/
#ifndef __VAYO_FILE_IO_H__
#define __VAYO_FILE_IO_H__

#include "VayoSupport.h"
NS_VAYO_BEGIN

class _VayoExport ReadFile
{
public:
	ReadFile(const wstring& fileName);
	~ReadFile();
	int            read(void* buffer, unsigned int sizeToRead);
	bool           seek(long finalPos, bool relativeMovement = false);
	bool           isOpen() const;
	long           getSize() const;
	long           getPos() const;
	const wstring& getFileName() const;

private:
	void openFile();
private:
	FILE*   _file;
	long    _fileSize;
	wstring _fileName;
};

class _VayoExport WriteFile
{
public:
	WriteFile(const wstring& fileName, bool append);
	~WriteFile();
	int            write(const void* buffer, unsigned int sizeToWrite);
	bool           seek(long finalPos, bool relativeMovement = false);
	bool           isOpen() const;
	long           getPos() const;
	const wstring& getFileName() const;

private:
	void openFile(bool append);
private:
	wstring _fileName;
	FILE*   _file;
	long    _fileSize;
};

NS_VAYO_END

#endif // __VAYO_FILE_IO_H__
