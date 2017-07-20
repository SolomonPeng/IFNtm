//
//  MODULE: IFNSocket.h
//
//	AUTHOR: Scott Pang 
//
//  mailto: zergps@163.com
//
//	Date: 03/12/2007
//
//	Version 0.01
// 

#pragma once
#pragma comment(lib, "WS2_32")

#define MAX_SOCK_NUM 20

class CIFNSocket
{
public:
	CIFNSocket(void);
	~CIFNSocket(void);

public:
	UINT ReceiveThread(LPVOID pParam);
	UINT SendThread(LPVOID pParam);
};
