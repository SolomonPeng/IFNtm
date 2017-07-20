#pragma once

class CLog
{
public:
	CLog(void);
	CLog(LPSTR lpszLogType);
	~CLog(void);
	UINT Initialize();
	void WriteLog(LPTSTR lpszHead,LPTSTR lpszMessage);
	void Info(LPTSTR lpszMessage);
	void Error(LPTSTR lpszMessage);
	void Fatal(LPTSTR lpszMessage);
	void Warn(LPTSTR lpszMessage);
	void Debug(LPTSTR lpszMessage);

private:
	CFile* pLog;
	LPTSTR m_strLogPath;
	LPTSTR m_strLogType;
};
