#include "VayoLog.h"
#include "VayoUtils.h"
#include <locale.h>
#include <tchar.h>
#include <stdarg.h>
#include <Windows.h>
#pragma warning(disable:4996)

NS_VAYO_BEGIN

VayoLogPrint(VayoLog.log)
static Log* s_pLog = NULL;

inline void Log::print(ELogLevel level, const char* szFormat, ...)
{
#if defined(_DEBUG) || defined(VAYO_ENABLE_LOG_PRINT)
	char szArgMessage[2048] = { 0 };
	va_list args;
	va_start(args, szFormat);
	vsprintf(szArgMessage, szFormat, args);
	va_end(args);

	if (s_pLog)
	{
		if (s_pLog->_hasConsole)
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), level | 8);
			printf("%s\n", szArgMessage);
		}

		if (s_pLog->_fout)
			s_pLog->_fout << ansiToUtf8(szArgMessage) << std::endl;
	}
#endif
}

inline void Log::wprint(ELogLevel level, const wchar_t* szFormat, ...)
{
#if defined(_DEBUG) || defined(VAYO_ENABLE_LOG_PRINT)
	wchar_t szArgMessage[2048] = {0};
	va_list args;
	va_start(args, szFormat);
	vswprintf(szArgMessage, 2048, szFormat, args);
	va_end(args);

	if (s_pLog)
	{
		if (s_pLog->_hasConsole)
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), level | 8);
			wprintf(L"%s\n", szArgMessage);
		}

		if (s_pLog->_fout)
			s_pLog->_fout << unicodeToUtf8(szArgMessage) << std::endl;
	}
#endif
}

Log::Log(bool hasConsole /*= true*/, const string& logFileName /*= ""*/)
	: _hasConsole(hasConsole)
{
	if (NULL == s_pLog)
	{
		s_pLog = this;

		if (_hasConsole)
		{
			AllocConsole();
			_tfreopen(_TEXT("CONOUT$"), _TEXT("w+t"), stdout);
			TCHAR szProgramName[MAX_PATH];
			HMODULE hModule(NULL);
			GetModuleFileName(hModule, szProgramName, MAX_PATH);
			SetConsoleTitle(szProgramName);
			setlocale(LC_ALL, "chs");
			DeleteMenu(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_BYCOMMAND);
		}

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
		if (s_pLog->_hasConsole)
			FreeConsole();
		s_pLog = NULL;
	}
}

NS_VAYO_END
