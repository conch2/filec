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

enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};

struct DataHeader
{
	short dataLength;
	short cmd;
};

struct Login : public DataHeader
{
	Login()
	{
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char Password[32];
};

struct LoginResult : public DataHeader
{
	LoginResult()
	{
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
};

struct Logout : public DataHeader
{
	Logout()
	{
		dataLength = sizeof(Logout);
		cmd = CMD_LOGOUT;
	}
	char userName[32];
};

struct LogoutResult : public DataHeader
{
	LogoutResult()
	{
		dataLength = sizeof(LogoutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};

struct NewUserJoin : public DataHeader
{
	NewUserJoin()
	{
		dataLength = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		sock = 0;
	}
	int sock;
};

std::vector<SOCKET> g_clients;

int headleClient(SOCKET _client)
{
	//缓冲区
	char recvBuf[1024] = {};
	int nlen = recv(_client, recvBuf, sizeof(DataHeader), 0);
	DataHeader* clientHeader = (DataHeader*)recvBuf;
	if (nlen <= 0)
	{
		printf("客户端已退出...\n");
		return -1;
	}
	switch (clientHeader->cmd)
	{
	case CMD_LOGIN:
	{
		recv(_client, recvBuf + sizeof(DataHeader), sizeof(Login), 0);
		Login* login = (Login*)recvBuf;
		printf("收到%d指令：CMD_LOGIN, 数据长度：%d,用户名：%s,密码：%s \n", _client, login->dataLength, login->userName, login->Password);
		LoginResult result;
		send(_client, (const char*)&result, result.dataLength, 0);
	} break;
	case CMD_LOGOUT:
	{
		recv(_client, recvBuf + sizeof(DataHeader), sizeof(Logout), 0);
		Logout* logout = (Logout*)recvBuf;
		printf("收到%d指令：CMD_LOGOUT, 数据长度：%d,用户名：%s\n", _client, logout->dataLength, logout->userName);
		LogoutResult result;
		send(_client, (const char*)&result, result.dataLength, 0);
	} break;
	default:
	{
		clientHeader->cmd = CMD_ERROR;
		clientHeader->dataLength = 0;
		send(_client, (const char*)&clientHeader, sizeof(DataHeader), 0);
	} break;
	}
	return 0;
}

int main()
{
#ifdef _WIN32
	{
		WSADATA dat;
		//启动Windows网络环境
		if (WSAStartup(MAKEWORD(2, 2), &dat) != 0)
		{
			printf("启动Windows网络环境失败\n");
		}
	}
#endif

	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _sock)
	{
		printf("创建socket失败...\n");
	}
	else {
		printf("创建socket成功...\n");
	}
	struct sockaddr_in _sockAddr = {};
	_sockAddr.sin_family = AF_INET;
	_sockAddr.sin_port = htons(8888);
#ifdef _WIN32
	_sockAddr.sin_addr.S_un.S_addr = INADDR_ANY;
#else
	_sockAddr.sin_addr.s_addr = INADDR_ANY;
#endif

	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sockAddr, sizeof(struct sockaddr_in)))
	{
		printf("绑定socket失败...\n");
	}
	else {
		printf("绑定socket成功...\n");
	}
	if (SOCKET_ERROR == listen(_sock, 5))
	{
		printf("监听socket失败...\n");
	}
	else {
		printf("监听socket成功...\n");
	}

	while (true)
	{
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
		int ret = select(_sock+1, &fdRead, &fdWrite, &fdExcpt, &t);
		if (ret < 0)
		{
			printf("select结束...\n");
			break;
		}
		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);
			struct sockaddr_in _clientAddr = {};
#ifdef _WIN32
			int claddrLen = sizeof(sockaddr_in);
#else
			socklen_t claddrLen = sizeof(sockaddr_in);
#endif
			SOCKET _client = accept(_sock, (struct sockaddr*)&_clientAddr, &claddrLen);
			if (INVALID_SOCKET == _client)
			{
				printf("错误，接收到无效客户端SOCKET...\n");
			}
			else {
				//向客户端发送新客户端加入的信息
				for (size_t n = g_clients.size(); n > 0; n--)
				{
					NewUserJoin user;
					user.sock = _client;
					send(g_clients[n-1], (const char*)&user, sizeof(NewUserJoin), 0);
				}
				g_clients.push_back(_client);
				printf("新客户端接入：SOCKET = %d  IP = %s\n", _client, inet_ntoa(_clientAddr.sin_addr));
			}
		}
#ifdef _WIN32
		for (size_t n = 0; n < fdRead.fd_count; n++)
		{
			if (-1 == headleClient(fdRead.fd_array[n]))
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
				if (-1 == headleClient(g_clients[n]))
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
		//printf("处理其他事...\n");
	}
	for (int n = g_clients.size() - 1; n >= 0; n--)
	{
#ifdef _WIN32
		closesocket(g_clients[n]);
#else
		close(g_clients[n]);
#endif
	}
#ifdef _WIN32
	closesocket(_sock);
	WSACleanup();
	system("pause");
#else
	close(_sock);
#endif
	return 0;
}
