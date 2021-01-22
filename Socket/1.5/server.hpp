#ifndef _server_hpp_
#define _server_hpp_

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN //避免Socket版本冲突
	#define _WINSOCK_DEPRECATED_NO_WARNINGS

	#include <Windows.h>
	#include <WinSock2.h>

	#pragma comment(lib, "ws2_32.lib")
#else 
	#include <unistd.h>
	#include <string.h>
	#include <arpa/inet.h>
	#include <sys/socket.h>
	#include <sys/select.h>

	#define SOCKET int
	#define INVALID_SOCKET (SOCKET)(~0)
	#define SOCKET_ERROR           (-1)
#endif

#include "MessageHeader.hpp"

class ServerSocket
{
public:
	ServerSocket()
	{
		_sock = INVALID_SOCKET;
	}

	virtual ~ServerSocket()
	{
		Close();
	}
	//初始化Socket
	SOCKET InitSocket()
	{
#ifdef _WIN32
		WSADATA dat;
		//启动Windows网络环境
		if (WSAStartup(MAKEWORD(2, 2), &dat) != 0)
		{
			printf("启动Windows网络环境失败\n");
		}
#endif
		if (INVALID_SOCKET != _sock)
		{
			printf("关闭旧链接<SOCKET=%d>...\n", _sock);
			Close();
		}
		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (INVALID_SOCKET == _sock) {
			printf("错误，创建socket失败...\n");
		}
		else {
			printf("创建<SOCKET=%d>成功...\n", _sock);
		}
		return _sock;
	}

	//关闭Socket
	void Close()
	{
		if (INVALID_SOCKET != _sock)
		{
			for (int n = g_clients.size() - 1; n >= 0; n--)
			{
#ifdef _WIN32
				closesocket(g_clients[n]);
			}
			closesocket(_sock);
			WSACleanup();
			system("pause");
#else
				close(g_clients[n]);
			}
			close(_sock);
#endif
		}
	}

	//绑定IP和端口
	int Bind(const char* ip, unsigned short port)
	{
		if (INVALID_SOCKET == _sock)
		{
			InitSocket();
		}
		struct sockaddr_in _sockAddr = {};
		_sockAddr.sin_family = AF_INET;
		_sockAddr.sin_port = htons(port);
#ifdef _WIN32
		if (ip) {
			_sockAddr.sin_addr.S_un.S_addr = inet_addr(ip);
		}
		else {
			_sockAddr.sin_addr.S_un.S_addr = INADDR_ANY;
		}
#else
		if (ip) {
			_sockAddr.sin_addr.s_addr = inet_addr(ip);
		}
		else {
			_sockAddr.sin_addr.s_addr = INADDR_ANY;
		}
#endif
		int ret = bind(_sock, (sockaddr*)&_sockAddr, sizeof(struct sockaddr_in));
		if (SOCKET_ERROR == ret) {
			printf("ERROR <SOCKET=%d>绑定<%u>失败...\n", _sock, port);
		}
		else {
			printf("<SOCKET=%d>绑定<%u>成功...\n", _sock, port);
		}
		return ret;
	}

	//监听网络端口
	int Listen(int n)
	{
		int ret = listen(_sock, n);
		if (SOCKET_ERROR == ret) {
			printf("ERROR <SOCKET=%d>监听网络端口失败...\n", _sock);
		}
		else {
			printf("<SOCKET=%d>监听网络端口成功...\n", _sock);
		}
		return ret;
	}

	//接受客户端连接
	SOCKET Accept()
	{
		struct sockaddr_in _clientAddr = {};
#ifdef _WIN32
		int claddrLen = sizeof(sockaddr_in);
#else
		socklen_t claddrLen = sizeof(sockaddr_in);
#endif
		SOCKET _client = accept(_sock, (struct sockaddr*)&_clientAddr, &claddrLen);
		if (INVALID_SOCKET == _client) {
			printf("ERROR <SOCKET=%d>接收到无效客户端SOCKET...\n", _sock);
		}
		else {
			//向客户端发送新客户端加入的信息
			NewUserJoin newUser;
			SendDataToAll(&newUser);
			g_clients.push_back(_client);
			printf("<SOCKET=%d>新客户端接入：SOCKET = %d IP = %s\n", _sock, _client, inet_ntoa(_clientAddr.sin_addr));
		}
		return _client;
	}

	//处理网络请求
	bool OnRun()
	{
		if (!IsRun()) {
			return false;
		}
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExcpt;

		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExcpt);

		FD_SET(_sock, &fdRead);
		FD_SET(_sock, &fdWrite);
		FD_SET(_sock, &fdExcpt);

		SOCKET maxSock = _sock;

		for (size_t n = g_clients.size(); n > 0; )
		{
			n--;
			FD_SET(g_clients[n], &fdRead);
			if (maxSock < g_clients[n])
			{
				maxSock = g_clients[n];
			}
		}
		timeval t = { 0, 0 };
		int ret = select(maxSock + 1, &fdRead, &fdWrite, &fdExcpt, &t);
		if (ret < 0)
		{
			printf("ERROR <SOCKET=%d>select错误...\n", _sock);
			Close();
			return false;
		}
		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);
			Accept();
		}
#ifdef _WIN32
		for (size_t n = 0; n < fdRead.fd_count; n++)
		{
			if (-1 == RecvData(fdRead.fd_array[n]))
			{
				auto iter = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[n]);
				if (iter != g_clients.end())
				{
					g_clients.erase(iter);
				}
			}
		}
#else 
		for (size_t n = g_clients.size(); n > 0; )
		{
			n--;
			if (FD_ISSET(g_clients[n], &fdRead))
			{
				if (-1 == RecvData(g_clients[n]))
				{
					std::vector<SOCKET>::iterator iter = g_clients.begin() + n;
					if (iter != g_clients.end())
					{
						g_clients.erase(iter);
					}
				}
			}
		}
#endif
		return true;
	}

	int RecvData(SOCKET _client)
	{
		//缓冲区
		char recvBuf[1024 * 2] = {};
		int nlen = recv(_client, recvBuf, sizeof(DataHeader), 0);
		DataHeader* header = (DataHeader*)recvBuf;
		if (nlen <= 0)
		{
			printf("客户端<client %d>已退出...\n", _client);
			return -1;
		}
		recv(_client, recvBuf + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		onNetMsg(_client, header);

		return 0;
	}

	//响应网络消息
	virtual void onNetMsg(SOCKET _client, DataHeader* header)
	{
		switch (header->cmd)
		{
		case CMD_LOGIN:
		{
			Login* login = (Login*)header;
			printf("收到<SOCKET=%d>指令：CMD_LOGIN, 数据长度：%d,用户名：%s,密码：%s \n", _client, login->dataLength, login->userName, login->Password);
			LoginResult result;
			send(_client, (const char*)&result, result.dataLength, 0);
		} break;
		case CMD_LOGOUT:
		{
			Logout* logout = (Logout*)header;
			printf("收到<SOCKET=%d>指令：CMD_LOGOUT, 数据长度：%d,用户名：%s\n", _client, logout->dataLength, logout->userName);
			LogoutResult result;
			send(_client, (const char*)&result, result.dataLength, 0);
		} break;
		default:
		{
			DataHeader resultHead = { 0 , CMD_ERROR };
			send(_client, (const char*)&resultHead, resultHead.dataLength, 0);
		} break;
		}
	}

	//单独向指定SOCKET发送数据
	int SendData(SOCKET _client, DataHeader* header)
	{
		if (IsRun() && header)
		{
			return send(_client, (const char*)header, header->dataLength, 0);
		}
		return SOCKET_ERROR;
	}

	//向所有客户端发送数据
	void SendDataToAll(DataHeader* header)
	{
		for (size_t n = g_clients.size(); n > 0; n--)
		{
			SendData(g_clients[n - 1], header);
		}
	}

	//是否在工作中
	bool IsRun()
	{
		return INVALID_SOCKET != _sock;
	}

private:
	SOCKET _sock;
	std::vector<SOCKET> g_clients;
};

#endif // !_server_hpp_
