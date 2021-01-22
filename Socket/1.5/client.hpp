#ifndef _client_hpp_
#define _client_hpp_

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

class ClientSocket
{
public:
	ClientSocket()
	{
		_sock = INVALID_SOCKET;
	}

	virtual ~ClientSocket()
	{
		if (INVALID_SOCKET != _sock)
		{
			Close();
		}
	}

	//初始化Socket
	void InitSocket()
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
		if (INVALID_SOCKET == _sock)
		{
			printf("错误，创建socket失败...\n");
		}
		else {
			printf("创建<SOCKET=%d>成功...\n", _sock);
		}
	}

	//连接服务器
	int Connect(const char* ip, unsigned short port)
	{
		if (INVALID_SOCKET == _sock)
		{
			InitSocket();
		}
		struct sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;
		_sin.sin_port = htons(port);
#ifdef _WIN32
		_sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else
		_sin.sin_addr.s_addr = inet_addr(ip);
#endif
		int ret = connect(_sock, (struct sockaddr*)&_sin, sizeof(struct sockaddr_in));
		if (SOCKET_ERROR == ret)
		{
			printf("<SOCKET=%d>连接服务端<%s:%u>失败...\n", _sock, ip, port);
		}
		else {
			printf("<SOCKET=%d>成功连接到服务端<%s:%u>...\n", _sock, ip, port);
		}
		return ret;
	}

	//关闭Socket
	void Close()
	{
		if (INVALID_SOCKET != _sock)
		{
#ifdef _WIN32
			closesocket(_sock);
			WSACleanup();
#else 
			close(_sock);
#endif 
			_sock = INVALID_SOCKET;
		}
	}

	//消息处理
	bool OnRun()
	{
		if (!IsRun())
		{
			return false;
		}
		fd_set fdRead;
		FD_ZERO(&fdRead);
		FD_SET(_sock, &fdRead);
		timeval t = { 0, 0 };
		int ret = select(_sock + 1, &fdRead, NULL, NULL, &t);
		if (ret < 0)
		{
			printf("<SOCKET=%d>select任务结束...\n", _sock);
			Close();
			return false;
		}
		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);
			if (-1 == RecvData())
			{
				Close();
				return false;
			}
		}
		return true;
	}

	bool IsRun()
	{
		return INVALID_SOCKET != _sock;
	}

	//接收数据
	int RecvData()
	{
		//缓冲区
		char recvBuf[1024 * 2] = {};
		long nlen = recv(_sock, recvBuf, sizeof(DataHeader), 0);
		DataHeader* header = (DataHeader*)recvBuf;
		if (nlen <= 0)
		{
			printf("<SOCKET=%d>与服务端断开连接...\n", _sock);
			return -1;
		}
		recv(_sock, recvBuf + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		onNetMsg(header);

		return 0;
	}

	//响应网络消息
	virtual void onNetMsg(DataHeader* header)
	{
		switch (header->cmd)
		{
		case CMD_LOGIN_RESULT:
		{
			LoginResult* login = (LoginResult*)header;
			printf("<SOCKET=%d>收到服务端：CMD_LOGIN_RESULT, 数据长度：%d ，result = %d \n",
				_sock, login->dataLength, login->result);
		} break;
		case CMD_LOGOUT_RESULT:
		{
			LogoutResult* logout = (LogoutResult*)header;
			printf("<SOCKET=%d>收到服务端：CMD_LOGOUT_RESULT, 数据长度：%d ，result = %d \n",
				_sock, logout->dataLength, logout->result);
		} break;
		case CMD_NEW_USER_JOIN:
		{
			NewUserJoin* user = (NewUserJoin*)header;
			printf("<SOCKET=%d>收到服务端：CMD_NEW_USER_JOIN, 数据长度：%d ，sock = %d \n",
				_sock, user->dataLength, user->sock);
		} break;
		}
	}

	//发送数据
	int sendData(DataHeader* header)
	{
		if (IsRun() && header)
		{
			return send(_sock, (const char*)header, header->dataLength, 0);
		}
		return SOCKET_ERROR;
	}

protected:

private:
	SOCKET _sock;

};

#endif // !_client_hpp_
