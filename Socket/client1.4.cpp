#include <thread>
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN //避免Socket版本冲突
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
		printf("收到服务端：CMD_LOGIN_RESULT, 数据长度：%d ，result = %d \n", login->dataLength, login->result);
	} break;
	case CMD_LOGOUT_RESULT:
	{
		recv(_sock, recvBuf + sizeof(DataHeader), sizeof(LogoutResult), 0);
		LogoutResult* logout = (LogoutResult*)recvBuf;
		printf("收到服务端：CMD_LOGOUT_RESULT, 数据长度：%d ，result = %d \n", logout->dataLength, logout->result);
	} break;
	case CMD_NEW_USER_JOIN:
	{
		recv(_sock, recvBuf + sizeof(DataHeader), sizeof(NewUserJoin), 0);
		NewUserJoin* user = (NewUserJoin*)recvBuf;
		printf("收到服务端：CMD_NEW_USER_JOIN, 数据长度：%d ，sock = %d \n", user->dataLength, user->sock);
	} break;
	}
	return 0;
}

bool thRun = true;

void cmdThread(SOCKET sock)
{
	char cmdBuf[128] = {};

	while (true)
	{
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit"))
		{
			printf("退出...\n");
			thRun = false;
			return;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{
			Login login;
			strcpy(login.userName, "conch");
			strcpy(login.Password, "1234");
			send(sock, (const char*)&login, sizeof(Login), 0);
		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			Logout logout;
			strcpy(logout.userName, "conch");
			send(sock, (const char*)&logout, sizeof(Logout), 0);
		}
		else {
			printf("无效指令...\n");
		}
	}
}

int main()
{
#ifdef _WIN32
	WSADATA dat;
	//启动Windows网络环境
	if (WSAStartup(MAKEWORD(2, 2), &dat) != 0)
	{
		printf("启动Windows网络环境失败\n");
	}
#endif

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
#ifdef _WIN32
	_sin.sin_addr.S_un.S_addr = inet_addr("192.168.56.1");
#else
	_sin.sin_addr.s_addr = inet_addr("192.168.56.1");
#endif
	if (SOCKET_ERROR == connect(_sock, (struct sockaddr*)&_sin, sizeof(struct sockaddr_in)))
	{
		printf("连接服务端失败...\n");
	}
	else {
		printf("成功连接到服务端...\n");
	}

	std::thread th(cmdThread, _sock);

	while (thRun)
	{
		fd_set fdRead;
		FD_ZERO(&fdRead);
		FD_SET(_sock, &fdRead);
		timeval t = { 0, 0 };
		int ret = select(_sock + 1, &fdRead, NULL, NULL, &t);
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

		//printf("处理其他事...\n");
	}
	th.join();
#ifdef _WIN32
	closesocket(_sock);
	WSACleanup();
	system("pause");
#else 
	close(_sock);
#endif 
	return 0;
}
