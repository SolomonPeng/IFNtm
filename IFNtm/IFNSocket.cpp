//
//  MODULE: IFNSocket.cpp
//
//	AUTHOR: Scott Pang 
//
//  mailto: zergps@163.com
//
//	Date: 03/12/2007
//
//	Version 0.01
// 

#include "StdAfx.h"
#include ".\ifnsocket.h"

CIFNSocket::CIFNSocket(void)
{
}

CIFNSocket::~CIFNSocket(void)
{
}

UINT CIFNSocket::ReceiveThread(LPVOID pParam)
{
	SOCKET server;
	WSADATA wsaData;
	sockaddr_in local;
	if (WSAStartup(0x101,&wsaData) != 0)
	{
		return INVALID_SOCKET;
	}
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port = htons((u_short)::GetPrivateProfileInt("NetSocket","recvport",2048,"./Config.ini"));
	server = socket(AF_INET,SOCK_STREAM,0);
	if (server == INVALID_SOCKET)
	{
		return INVALID_SOCKET;
	}
	if (bind(server,(sockaddr*)&local,sizeof(local)) == SOCKET_ERROR)
	{
		return INVALID_SOCKET;
	}
	if (listen(server,MAX_SOCK_NUM) == SOCKET_ERROR)
	{
		return INVALID_SOCKET;
	}
	SOCKET client;
	sockaddr_in from;
	int fromlen = sizeof(from);
	int bufferlen = ::GetPrivateProfileInt("NetSocket","buffersize",1024,"./Config.ini");
	char * buf = new char[bufferlen];
	while (true)
	{
		client = accept(server,(struct sockaddr*)&from,&fromlen);
		if (client != INVALID_SOCKET)
		{
			if (setsockopt(client,SOL_SOCKET,SO_RCVBUF,(char *)&bufferlen,sizeof(int)) == SOCKET_ERROR)
			{
				return INVALID_SOCKET;
			}
			if (setsockopt(client,SOL_SOCKET,SO_SNDBUF,(char *)&bufferlen,sizeof(int)) == SOCKET_ERROR)
			{
				return INVALID_SOCKET;
			}
			if (recv(client,buf,bufferlen,0) != SOCKET_ERROR)
			{
			}
			closesocket(client);
		}
	}
	closesocket(server);
	WSACleanup();
	return 1;
}

UINT CIFNSocket::SendThread(LPVOID pParam)
{
	char * remoteAddr;
	remoteAddr = (char *)malloc(15);
	if (::GetPrivateProfileString("NetSocket","remoteaddr","",remoteAddr,15,"./Config.ini")==0)
	{
		return 0;
	}
	int bufferlen;
	bufferlen = ::GetPrivateProfileInt("NetSocket","buffersize",1024,"./Config.ini");
	char * buf = (char *)pParam;
	SOCKET client;
	WSADATA wsaData;
	sockaddr_in remote;
	remote.sin_family = AF_INET;
	remote.sin_addr.s_addr = inet_addr(remoteAddr);
	remote.sin_port = htons((u_short)::GetPrivateProfileInt("NetSocket","sendport",2049,"./Config.ini"));
	if (WSAStartup(0x101,&wsaData) != 0)
	{
		return INVALID_SOCKET;
	}
	client = socket(AF_INET,SOCK_STREAM,0);
	if (client == INVALID_SOCKET)
	{
		return INVALID_SOCKET;
	}
	if (setsockopt(client,SOL_SOCKET,SO_RCVBUF,(char *)&bufferlen,sizeof(int)) == SOCKET_ERROR)
	{
		return INVALID_SOCKET;
	}
	if (setsockopt(client,SOL_SOCKET,SO_SNDBUF,(char *)&bufferlen,sizeof(int)) == SOCKET_ERROR)
	{
		return INVALID_SOCKET;
	}
	if (connect(client,(const sockaddr *)&remote,sizeof(remote)) == SOCKET_ERROR)
	{
		return INVALID_SOCKET;
	}
	free(remoteAddr);
	if (send(client,buf,bufferlen,0) != SOCKET_ERROR)
	{
	}
	closesocket(client);
	WSACleanup();
	return 1;
}
