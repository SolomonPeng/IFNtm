#pragma once
#include "IFNSocket.h"
#include "oracl.h"
#include "Xml.h"
#include "Log.h"
#pragma comment(lib, "ORACLM32")
#define CLOB_SIZE 1000

typedef struct _DATA_HEAD{
	CHAR szVersion;
	CHAR szHeadlen[4];
	CHAR szDataLen[4];
	CHAR szServiceType;
	CHAR szSysFrom[2];
	CHAR szSysTo[2];
}DATA_HEAD;


class CNetTaskToNetMaster
{
public:
	CNetTaskToNetMaster(void);
	~CNetTaskToNetMaster(void);
	BOOL GetSentData();
	LPSTR CreateSendDataHead();
	BOOL CreateAck();
	UINT RunThread(LPVOID pParam);

public:
	CLog log;

private:
	CIFNSocket SockNTToNM;
	DATA_HEAD m_DataHead;
	LPSTR m_lpszDataPak;
	LPCTSTR m_lpszGetPakSQL;
};
