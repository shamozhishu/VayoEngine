#pragma once

#define CMD_MAX_NUM (1024)

class CmdManager
{
public:
	CmdManager();
	~CmdManager();
	static CmdManager* GetIns();
	static void Destroy();
	void RunCommand();
	void SendCmd(CString cmdName);

private:
	void RegisterCmd();

private:
	CWinThread* m_pThread;
	CEvent m_waitCmd;
	CString m_curRunCmd;
	map<CString, unsigned> m_allSigs;
	Signal m_sigPool[CMD_MAX_NUM];
};
