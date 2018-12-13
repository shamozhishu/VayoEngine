#include "stdafx.h"
#include "CmdManager.h"

CmdManager* g_cmgMgr = NULL;
bool g_run = true;

static UINT ThreadFunc(LPVOID pParam)
{
	CmdManager* cmdMgr = (CmdManager*)pParam;
	if (cmdMgr)
	{
		cmdMgr->RunCommand();
	}

	return 0;
}

CmdManager::CmdManager()
{
	RegisterCmd();
	m_pThread = AfxBeginThread(ThreadFunc, this, THREAD_PRIORITY_NORMAL, 0, 0);
}

CmdManager::~CmdManager()
{
	g_run = false;
	m_waitCmd.SetEvent();
}

CmdManager* CmdManager::GetIns()
{
	if (!g_cmgMgr)
		g_cmgMgr = new CmdManager();
	return g_cmgMgr;
}

void CmdManager::Destroy()
{
	SAFE_DELETE(g_cmgMgr);
}

void CmdManager::RegisterCmd()
{
}

void CmdManager::RunCommand()
{
	while (g_run && WAIT_OBJECT_0 == WaitForSingleObject(m_waitCmd, INFINITE))
	{
		if (!g_run)
		{
			break;
		}

		map<CString, unsigned>::iterator it = m_allSigs.find(m_curRunCmd);
		if (it != m_allSigs.end())
		{
			SignalTrigger::emit(m_sigPool[it->second]);
		}
	}
}

void CmdManager::SendCmd(CString cmdName)
{
	m_curRunCmd = cmdName;
	m_curRunCmd.MakeUpper();
	m_waitCmd.SetEvent();
}
