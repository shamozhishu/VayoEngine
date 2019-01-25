#include "VayoFileIO.h"

NS_VAYO_BEGIN

void ReadFile::openFile()
{
	if (_fileName.size() == 0)
	{
		_file = NULL;
		return;
	}

	_file = _wfopen(_fileName.c_str(), L"rb");

	if (_file)
	{
		fseek(_file, 0, SEEK_END);
		_fileSize = getPos();
		fseek(_file, 0, SEEK_SET);
	}
}

ReadFile::ReadFile(const wstring& fileName)
	: _file(NULL)
	, _fileSize(0)
	, _fileName(fileName)
{
	openFile();
}

ReadFile::~ReadFile()
{
	if (_file)
		fclose(_file);
}

int ReadFile::read(void* buffer, unsigned int sizeToRead)
{
	if (!isOpen())
		return 0;
	return (int)fread(buffer, 1, sizeToRead, _file);
}

bool ReadFile::seek(long finalPos, bool relativeMovement /*= false*/)
{
	if (!isOpen())
		return false;
	return fseek(_file, finalPos, relativeMovement ? SEEK_CUR : SEEK_SET) == 0;
}

bool ReadFile::isOpen() const
{
	return _file != NULL;
}

long ReadFile::getSize() const
{
	return _fileSize;
}

long ReadFile::getPos() const
{
	return ftell(_file);
}

const std::wstring& ReadFile::getFileName() const
{
	return _fileName;
}

//////////////////////////////////////////////////////////////////////////
void WriteFile::openFile(bool append)
{
	if (_fileName.size() == 0)
	{
		_file = 0;
		return;
	}

	_file = _wfopen(_fileName.c_str(), append ? L"ab" : L"wb");

	if (_file)
	{
		fseek(_file, 0, SEEK_END);
		_fileSize = ftell(_file);
		fseek(_file, 0, SEEK_SET);
	}
}

WriteFile::WriteFile(const wstring& fileName, bool append)
	: _file(NULL)
	, _fileSize(0)
	, _fileName(fileName)
{
	openFile(append);
}

WriteFile::~WriteFile()
{
	if (_file)
		fclose(_file);
}

int WriteFile::write(const void* buffer, unsigned int sizeToWrite)
{
	if (!isOpen())
		return 0;
	return (int)fwrite(buffer, 1, sizeToWrite, _file);
}

bool WriteFile::seek(long finalPos, bool relativeMovement /*= false*/)
{
	if (!isOpen())
		return false;
	return fseek(_file, finalPos, relativeMovement ? SEEK_CUR : SEEK_SET) == 0;
}

bool WriteFile::isOpen() const
{
	return _file != NULL;
}

long WriteFile::getPos() const
{
	return ftell(_file);
}

const std::wstring& WriteFile::getFileName() const
{
	return _fileName;
}

NS_VAYO_END
