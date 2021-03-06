#define WIN32_LEAN_AND_MEAN //避免Socket版本冲突

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

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

int headleClient(SOCKET _sock)
{
	//缓冲区
	char recvBuf[1024] = {};
	int nlen = recv(_sock, recvBuf, sizeof(DataHeader), 0);
	DataHeader* clientHeader = (DataHeader*)recvBuf;
	if (nlen <= 0)
	{
		printf("与服务端断开连接...\n");
		return -1;
	}
	switch (clientHeader->cmd)
	{
	case CMD_LOGIN_RESULT:
	{
		recv(_sock, recvBuf + sizeof(DataHeader), sizeof(LoginResult), 0);
		LoginResult* login = (LoginResult*)recvBuf;
		printf("收到客户端：CMD_LOGIN_RESULT, 数据长度：%d \n", login->dataLength);
	} break;
	case CMD_LOGOUT_RESULT:
	{
		recv(_sock, recvBuf + sizeof(DataHeader), sizeof(LogoutResult), 0);
		LogoutResult* logout = (LogoutResult*)recvBuf;
		printf("收到客户端：CMD_LOGOUT_RESULT, 数据长度：%d \n", logout->dataLength);
	} break;
	case CMD_NEW_USER_JOIN:
	{
		recv(_sock, recvBuf + sizeof(DataHeader), sizeof(NewUserJoin), 0);
		NewUserJoin* user = (NewUserJoin*)recvBuf;
		printf("收到客户端：CMD_NEW_USER_JOIN, 数据长度：%d \n", user->dataLength);
	} break;
	}
}

int main()
{
	WSADATA dat;
	//启动Windows网络环境
	if (WSAStartup(MAKEWORD(2, 2), &dat) != 0)
	{
		printf("启动Windows网络环境失败\n");
	}

	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == _sock)
	{
		printf("错误，创建socket失败...\n");
	}
	else {
		printf("创建socket成功...\n");
	}
	struct sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(8888);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (SOCKET_ERROR == connect(_sock, (struct sockaddr*)&_sin, sizeof(struct sockaddr_in)))
	{
		printf("连接服务端失败...\n");
	}
	else {
		printf("成功连接到服务端...\n");
	}
	while (true)
	{
		fd_set fdRead;
		FD_ZERO(&fdRead);
		FD_SET(_sock, &fdRead);
		timeval t = { 1, 0 };
		int ret = select(_sock, &fdRead, NULL, NULL, &t);
		if (ret < 0)
		{
			printf("服务端已断开连接...\n");
			break;
		}
		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);
			if (-1 == headleClient(_sock))
			{
				break;
			}
		}

		printf("处理其他事...\n");
	}

	closesocket(_sock);
	WSACleanup();
	system("pause");
	return 0;
}
