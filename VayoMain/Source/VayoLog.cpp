#include "VayoLog.h"
#include "VayoUtils.h"
#include <locale.h>
#include <tchar.h>
#include <stdarg.h>
#include <Windows.h>
#pragma warning(disable:4996)

NS_VAYO_BEGIN

#ifdef _DEBUG
static Log s_logger("VayoLog.txt");
#endif

static Log* s_pLog = NULL;

void Log::print(ELogLevel level, const char* szFormat, ...)
{
	if (s_pLog)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), level | 8);
	}

	char szArgMessage[2048] = { 0 };
	va_list args;
	va_start(args, szFormat);
	vsprintf(szArgMessage, szFormat, args);
	va_end(args);
	printf("%s\n", szArgMessage);

	if (s_pLog && s_pLog->_fout)
	{
		s_pLog->_fout << ansiToUtf8(szArgMessage) << std::endl;
	}
}

void Log::wprint(ELogLevel level, const wchar_t* szFormat, ...)
{
	if (s_pLog)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), level | 8);
	}

	wchar_t szArgMessage[2048] = {0};
	va_list args;
	va_start(args, szFormat);
	vswprintf(szArgMessage, 2048, szFormat, args);
	va_end(args);
	wprintf(L"%s\n", szArgMessage);

	if (s_pLog && s_pLog->_fout)
	{
		s_pLog->_fout << unicodeToUtf8(szArgMessage) << std::endl;
	}
}

Log::Log(const string& logFileName)
	: _fout(logFileName)
{
	VAYO_ASSERT(NULL == s_pLog);
	s_pLog = this;
	AllocConsole();
	_tfreopen(_TEXT("CONOUT$"), _TEXT("w+t"), stdout);
	TCHAR szProgramName[MAX_PATH];
	HMODULE hModule(NULL);
	GetModuleFileName(hModule, szProgramName, MAX_PATH);
	SetConsoleTitle(szProgramName);
	setlocale(LC_ALL, "chs");
	DeleteMenu(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_BYCOMMAND);
	_fout.imbue(std::locale("chs"));
}

Log::~Log()
{
	VAYO_ASSERT(NULL != s_pLog);
	s_pLog = NULL;
	_fout.close();
	FreeConsole();
}

NS_VAYO_END
