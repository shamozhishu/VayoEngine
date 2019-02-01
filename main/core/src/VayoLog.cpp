#include "VayoLog.h"
#include "VayoUtils.h"
#include <locale.h>
#include <tchar.h>
#include <stdarg.h>
#include <Windows.h>
#pragma warning(disable:4996)

NS_VAYO_BEGIN

EnableFileLogPrint(VayoLog.log)
static Log* s_pLog = NULL;

void Log::print(ELogLevel level, const char* szFormat, ...)
{
	char szArgMessage[2048] = { 0 };
	va_list args;
	va_start(args, szFormat);
	vsprintf(szArgMessage, szFormat, args);
	va_end(args);
	if (s_pLog)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), level | 8);
		printf("%s\n", szArgMessage);
		if (s_pLog->_fout)
			s_pLog->_fout << ansiToUtf8(szArgMessage) << std::endl;
	}
}

void Log::wprint(ELogLevel level, const wchar_t* szFormat, ...)
{
	wchar_t szArgMessage[2048] = {0};
	va_list args;
	va_start(args, szFormat);
	vswprintf(szArgMessage, 2048, szFormat, args);
	va_end(args);
	if (s_pLog)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), level | 8);
		wprintf(L"%s\n", szArgMessage);
		if (s_pLog->_fout)
			s_pLog->_fout << unicodeToUtf8(szArgMessage) << std::endl;
	}
}

Log::Log(const string& logFileName)
{
	if (NULL == s_pLog)
	{
		s_pLog = this;
		AllocConsole();
		_tfreopen(_TEXT("CONOUT$"), _TEXT("w+t"), stdout);
		TCHAR szProgramName[MAX_PATH];
		HMODULE hModule(NULL);
		GetModuleFileName(hModule, szProgramName, MAX_PATH);
		SetConsoleTitle(szProgramName);
		setlocale(LC_ALL, "chs");
		DeleteMenu(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_BYCOMMAND);
		if (!logFileName.empty())
		{
			_fout.open(logFileName);
			_fout.imbue(std::locale("chs"));
		}
	}
}

Log::~Log()
{
	if (NULL != s_pLog)
	{
		if (s_pLog->_fout)
			_fout.close();
		FreeConsole();
		s_pLog = NULL;
	}
}

NS_VAYO_END
