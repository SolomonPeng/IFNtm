#include "StdAfx.h"
#include ".\log.h"

CLog::CLog(void)
{
}

CLog::~CLog(void)
{
}

CLog::CLog(LPTSTR lpszLogType)
{
	this->m_strLogType=lpszLogType;
}

UINT CLog::Initialize()
{
	if (::GetPrivateProfileString("Log","path","",this->m_strLogPath,15,"./Config.ini")==0)
	{
		AfxMessageBox("获取日志路径失败！");
		return 0;
	}
	return 1;
}

void CLog::WriteLog(LPTSTR lpszHead,LPTSTR lpszMessage)
{
	CString strFileName;
	strFileName.Format("%s%s%s.log",this->m_strLogPath,CTime::GetCurrentTime().Format("%y:%m:%d"),this->m_strLogType);
	this->pLog = new CFile((LPCTSTR)strFileName,CFile::modeCreate||CFile::modeNoTruncate||CFile::modeReadWrite);
	CString strContent;
	strContent.Format("%s,[%s]:  %s",CTime::GetCurrentTime().Format("%y:%m:%d   %H-%M-%S"),lpszHead,lpszMessage);
	pLog->Write((LPTSTR)(LPCTSTR)strContent,strlen((LPTSTR)(LPCTSTR)strContent));
	pLog->Close();
	free(pLog);
}

void CLog::Error(LPTSTR lpszMessage)
{
	this->WriteLog("Error",lpszMessage);
}

void CLog::Fatal(LPTSTR lpszMessage)
{
	this->WriteLog("Fatal",lpszMessage);
}

void CLog::Info(LPTSTR lpszMessage)
{
	this->WriteLog("Info",lpszMessage);
}

void CLog::Warn(LPTSTR lpszMessage)
{
	this->WriteLog("Warn",lpszMessage);
}

void CLog::Debug(LPTSTR lpszMessage)
{
	this->WriteLog("Debug",lpszMessage);
}






















