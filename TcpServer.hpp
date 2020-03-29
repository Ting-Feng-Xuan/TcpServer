#ifndef __TcpServer_hpp__
#define __TcpServer_hpp__

#ifdef _WIN32       //windows platform
	#define _CRT_SECURE_NO_WARNINGS
	#define WIN32_LEAN_AND_MEAN
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#include<windows.h>
	#include<WinSock2.h>
	#include<vector>
	#pragma comment(lib,"ws2_32.lib")

#else               //uninx platform
	#define SOCKET unsigned int
	#include<stdio.h>
	#include<sys/types.h>
	#include<sys/socket.h>
	#include<arpa/inet.h>
	#include<unistd.h>
	#include<string.h>
	#include<stdlib.h>
	#include<errno.h>

#endif
#include"TcpServer.h"
#include<iostream>
using namespace std;

class TcpServer
{
	SOCKET _sock;
	vector<SOCKET>g_clients;
public:
	TcpServer()
	{
		_sock = INVALID_SOCKET;
	}
	virtual ~TcpServer()
	{

	}
	/* initialization socket
	 * bind port
	 * listen 
	 */
	SOCKET InintServer(int port)
	{
		//startup windows net enviroment
#define SOCKET unsigned int
#ifdef _WIN32
		WORD ver = MAKEWORD(2, 2);
		int recv_len = 0;
		WSADATA dat;
		WSAStartup(ver, &dat);	//start Windows net enviroment£¬windows apis can be called before cleanup.
#endif
		if (_sock != INVALID_SOCKET)
		{
			cout << "<socket = " << _sock << "> close old connection" << endl;
			Close();
		}
		_sock = socket(AF_INET, SOCK_STREAM, 0);
		if (_sock == INVALID_SOCKET)
		{
			cout << "error: socket create failed!" << endl;
			return SOCKET_ERROR;
		}
		sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;
		_sin.sin_port = htons(port);
#ifdef _WIN32									//Windows Platform
		_sin.sin_addr.S_un.S_addr = INADDR_ANY;
#else											//Linux Platform
		_sin.sin_addr.s_addr = INADDR_ANY;
#endif // _WIN32
		/* bind port and local ip address */
		int ret = bind(_sock, (sockaddr *)&_sin, sizeof(sockaddr_in));
		if (ret < 0)
		{
			cout << "error: bind failed!" << endl;
			return SOCKET_ERROR;
		}

		ret = listen(_sock, LISTEN_MAX);
		if (ret < 0)
		{
			cout << "error: listen failed!" << endl;
			return SOCKET_ERROR;
		}
		cout << "Server start..." << endl;
		return _sock;
	}
	/* Accept client connect*/
	SOCKET Accept(SOCKET socket)
	{
		if (socket == INVALID_SOCKET)
		{
			cout << "socket error: scoket dose not initialization!" << endl;
			return SOCKET_ERROR;
		}
		sockaddr_in ClientAddr = {};
		int ClientAddrLen = sizeof(sockaddr_in);
		SOCKET _ClientSock = INVALID_SOCKET;
#ifdef _WIN32															// Windows Platform
		 _ClientSock = accept(socket, (sockaddr *)&ClientAddr, &ClientAddrLen);
#else																	// Linux Platform
		 _ClientSock = accept(socket, (sockaddr *)&ClientAddr, (socklen_t *)&ClientAddrLen);
#endif // 
		 if (_ClientSock == INVALID_SOCKET)
		 {
			 cout << "error: accept clinet invalid connection!" << endl;
			 return SOCKET_ERROR;
		 }
		 else
		 {
			 cout << "New Client:<socket = " << _ClientSock << ">ip:" << inet_ntoa(ClientAddr.sin_addr) << endl;
			 return _ClientSock;
		 }
		
	}

	/* close socket */
	void Close()
	{
		if (_sock != INVALID_SOCKET)
		{
#ifdef _WIN32
			for (int i = 0; i < (int)g_clients.size(); i++)
			{
				closesocket(g_clients[i]);
			}
			closesocket(_sock);
			WSACleanup();
#else
			for (int i = 0; i < g_clients.size(); i++)
			{
				close(g_clients[i]);
			}
			close(_sock);
#endif
		}
	}
	/* On run */
	void OnRun()
	{
		if (!IsRunning())
		{
			return;
		}
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExcept;

		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExcept);

		FD_SET(_sock,&fdRead);
		FD_SET(_sock, &fdWrite);
		FD_SET(_sock, &fdExcept);

		for (int i = 0; i < (int)g_clients.size(); i++)
		{
			FD_SET(g_clients[i],&fdRead);
		}
		int ret = select(_sock + 1, &fdRead, &fdWrite, &fdExcept, NULL);
		if (ret < 0)
		{
			cout << "error:select task exit!" << endl;
			return;
		}
		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock,&fdRead);
			SOCKET ClientSock = Accept(_sock);
			if (ClientSock != SOCKET_ERROR)
			{
				g_clients.push_back(ClientSock);
			}
		}
		for (int i = 0; i < (int)fdRead.fd_count; i++)
		{
			if (-1 == RecvData(fdRead.fd_array[i]))
			{
				auto iter = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[i]);
				if (iter != g_clients.end())
				{
					g_clients.erase(iter);
				}
			}
		}
	}
	/* receive data */
	int RecvData(SOCKET ClientSocket)
	{
		DataHeader header;
		if (IsConnecting(ClientSocket))
		{
			int ret = (int)recv(ClientSocket,(char *)&header, sizeof(DataHeader), 0);
			if (ret <= 0)
			{
				cout << "warning:client<socket = "<<ClientSocket<<"> clsoed!" << endl;
				return -1;
			}
			HandleMessage(ClientSocket, &header);
		}
		return 0;
	}
	/* handle message */
	virtual void HandleMessage(SOCKET ClientSocket,DataHeader *header)
	{
		MessageData RecvBuf;

		char ErrBuf[1024] = {};
		switch (header->cmd)
		{
		case CMD_LOGIN:
			{
				recv(ClientSocket, (char *)&RecvBuf + sizeof(DataHeader), header->datalen - sizeof(DataHeader), 0);
				cout << "receive login order <" << header->datalen << "> byte from client<socket = " << ClientSocket << ">:" << RecvBuf.data << endl;
				MessageData LgoinResult(CMD_LOGIN_RESULT, "OK");
				SendData(ClientSocket, &LgoinResult);
				SendDataToAll(&LgoinResult);
				break;
			}
		case CMD_LOGOUT:
			{
				recv(ClientSocket, (char *)&RecvBuf + sizeof(DataHeader), header->datalen - sizeof(DataHeader), 0);
				cout << "receive logout order <" << header->datalen << "> byte from client<socket = " << ClientSocket << ">:" << RecvBuf.data << endl;
				MessageData LogoutResult(CMD_LOGOUT_RESULT, "OK");
				SendData(ClientSocket, &LogoutResult);
				break;
			}
		case CMD_ERROR:
			{	
				cout << "receive error order!" << endl;
				MessageData ErrorResult(CMD_ERROR, "Mistake");
				SendData(ClientSocket, &ErrorResult);
				break;
			}
		default:
			{
				strncpy(ErrBuf, (char *)header, sizeof(DataHeader));
				cout << ErrBuf << "sizefo(ErrBuf) = " << sizeof(ErrBuf) << " sizeof(DataHeader) = " << sizeof(DataHeader) << endl;
				int ret = recv(ClientSocket, ErrBuf + strlen(ErrBuf), sizeof(ErrBuf) - strlen(ErrBuf), 0);
				cout << "receive <" << ret << "> byte from client<socket = " << ClientSocket << ">:" << ErrBuf << endl;
				break;
			}
		}
	}
	/* send data */
	int SendData(SOCKET ClientSocket, DataHeader *header)
	{
		if (IsConnecting(ClientSocket) && header)
		{
			int ret = send(ClientSocket, (const char *)header, header->datalen, 0);
			if (ret <= 0)
			{
				cout << "error:send data to client<scoket = "<<ClientSocket<<"> failed!" << endl;
				return false;
			}
			else
			{
				cout << "send " << ret << " byte data to client<socket = "<<ClientSocket<<">" << endl;
				return true;
			}
		}
		return false;
	}
	/* send data to all client */
	void SendDataToAll(DataHeader *header)
	{
		for (int i = (int)g_clients.size() - 1; i >= 0; i--)
		{
			SendData(g_clients[i], header);
		}
		
	}
	/* Is running */
	bool IsRunning()
	{
		return _sock != INVALID_SOCKET;
	}
	/* Is Client connecting */
	bool IsConnecting(SOCKET socket)
	{
		return socket != INVALID_SOCKET;
	}
};



#endif