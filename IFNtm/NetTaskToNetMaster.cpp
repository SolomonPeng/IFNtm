#include "StdAfx.h"
#include ".\nettasktonetmaster.h"

CNetTaskToNetMaster::CNetTaskToNetMaster(void)
{
	this->log = CLog("NTToNM");
	this->log.Initialize();
	m_lpszGetPakSQL = "select * from clobtest";
}

CNetTaskToNetMaster::~CNetTaskToNetMaster(void)
{
}

BOOL CNetTaskToNetMaster::GetSentData()
{
	BOOL res=FALSE;
	char * database=(char *)malloc(15);
	char * uid=(char *)malloc(15);
	char * pwd=(char *)malloc(15);
	::GetPrivateProfileString("DataBase","database","",database,15,"./Config.ini");
	::GetPrivateProfileString("DataBase","uid","",uid,15,"./Config.ini");
	::GetPrivateProfileString("DataBase","pwd","",pwd,15,"./Config.ini");
	OStartup();
	ODatabase odb(database, uid, pwd);
	ODynaset odyn(odb, this->m_lpszGetPakSQL);
	OClob oclob;
	odyn.GetFieldValue("text", &oclob);
	unsigned char *buffer = 0;
	try
	{
		unsigned long size = oclob.GetSize();

		buffer = (unsigned char *)malloc(size);
		memset(buffer, 0, size);
		oclob.EnableStreaming(size);
		short status = OLOB_NEED_DATA;
		unsigned long amtread = 0;
		while (status == OLOB_NEED_DATA)
		{
			// 下面是把数据读到 buffer 中
			amtread = oclob.Read(&status, buffer, CLOB_SIZE);
		}
		oclob.DisableStreaming();
	}
	catch(OException Exc)
	{
		AfxMessageBox((CString)Exc.GetFailedMethodName() + " Error: " + (CString)Exc.GetErrorText());
	}
	this->m_lpszDataPak = (LPSTR)(LPCTSTR)buffer;
	free(buffer);
	free(database);
	free(uid);
	free(pwd);
	if (strlen(this->m_lpszDataPak)>10)
	{
		res=TRUE;
	}
	return res;
}

LPSTR CNetTaskToNetMaster::CreateSendDataHead()
{
	LPSTR lpszHead = new char[15];
	/*int len;
	char* tmp;
	lpszHead[0]=char(0x02);
	itoa(14,tmp,10);
	strcat(lpszHead,tmp);
	len = strlen(this->m_lpszDataPak);
	itoa(len,tmp,10);
	strcat(lpszHead,tmp);
	lpszHead[9]='2';
	strcat(lpszHead,"br");  //源系统代号
	strcat(lpszHead,"nm");  //目的系统代号*/
	m_DataHead.szVersion=char(0x02);
	char* tmp="";
	itoa(14,tmp,10);
	strcpy(m_DataHead.szHeadlen,tmp);
	tmp="";
	int len = strlen(this->m_lpszDataPak);
	itoa(len,tmp,10);
	strcpy(m_DataHead.szDataLen,tmp);
	m_DataHead.szServiceType='2';
	strcpy(m_DataHead.szSysFrom,"br");
	strcpy(m_DataHead.szSysTo,"nm");

	lpszHead[0] = m_DataHead.szVersion;
	strcat(lpszHead,m_DataHead.szHeadlen);
	strcat(lpszHead,m_DataHead.szDataLen);
	lpszHead[9] = m_DataHead.szServiceType;
	strcat(lpszHead,m_DataHead.szSysFrom);
	strcat(lpszHead,m_DataHead.szSysTo);
	return lpszHead;
}

UINT CNetTaskToNetMaster::RunThread(LPVOID pParam)
{
	while(TRUE)
	{
		if (GetSentData())
		{
			LPSTR lpszData = CreateSendDataHead();
			strcat(lpszData,this->m_lpszDataPak);
			SockNTToNM.SendThread(lpszData);
		}
		else
			::Sleep(600000);
	}
}
























