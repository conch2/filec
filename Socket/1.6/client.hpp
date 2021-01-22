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

//缓冲区的最小单元
#define RECV_BUFF_SIZE 1024

class ClientSocket
{
public:
	ClientSocket()
	{
		_sock = INVALID_SOCKET;
		memset(_szRecv, 0, RECV_BUFF_SIZE);
		memset(_szMsgBuf, 0, RECV_BUFF_SIZE * 10);
		_lastPos = 0;
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

	//select消息处理
	bool OnRun()
	{
		if (!IsRun())
		{
			return false;
		}
		fd_set fdRead;
		FD_ZERO(&fdRead);
		FD_SET(_sock, &fdRead);
		timeval t = { 1, 0 };
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

	//判断socket是否还在运行
	bool IsRun()
	{
		return INVALID_SOCKET != _sock;
	}

	//接收数据
	int RecvData()
	{
		long nlen = recv(_sock, _szRecv, RECV_BUFF_SIZE, 0);
		DataHeader* header = (DataHeader*)_szRecv;
		if (nlen <= 0)
		{
			printf("<SOCKET=%d>与服务端断开连接...\n", _sock);
			return -1;
		}
		//将收取到的消息拷贝到消息缓冲区
		memcpy(_szMsgBuf + _lastPos, _szRecv, nlen);
		//消息缓冲区的数据尾部位置后移
		_lastPos += nlen;
		//判断消息缓冲区的数据长度大于等于消息头DataHeader的长度
		while (_lastPos >= sizeof(DataHeader))
		{
			//取出消息头
			DataHeader* header = (DataHeader*)_szMsgBuf;
			//判断消息缓冲区的数据长度大于等于消息长度
			if (_lastPos >= header->dataLength)
			{
				//消息缓冲区未处理的数据长度
				int nSize = _lastPos - header->dataLength;
				//处理网络数据
				OnNetMsg(header);
				//将消息缓冲区未处理数据前移
				memcpy(_szMsgBuf, _szMsgBuf + header->dataLength, nSize);
				//消息缓冲区的数据尾部位置前移
				_lastPos = nSize;
			}
			else {
				//消息缓冲区里的数据不够一条完整消息
				break;
			}
		}

		return 0;
	}

	//响应网络消息
	virtual void OnNetMsg(DataHeader* header)
	{
		switch (header->cmd)
		{
		case CMD_LOGIN_RESULT:
		{
			LoginResult* login = (LoginResult*)header;
			//printf("<SOCKET=%d>收到服务端：CMD_LOGIN_RESULT, 数据长度：%d ，result = %d \n",
				//_sock, login->dataLength, login->result);
		} break;
		case CMD_LOGOUT_RESULT:
		{
			LogoutResult* logout = (LogoutResult*)header;
			//printf("<SOCKET=%d>收到服务端：CMD_LOGOUT_RESULT, 数据长度：%d ，result = %d \n",
				//_sock, logout->dataLength, logout->result);
		} break;
		case CMD_NEW_USER_JOIN:
		{
			NewUserJoin* user = (NewUserJoin*)header;
			//printf("<SOCKET=%d>收到服务端：CMD_NEW_USER_JOIN, 数据长度：%d ，sock = %d \n",
				//_sock, user->dataLength, user->sock);
		} break;
		case CMD_ERROR:
		{
			printf("<SOCKET=%d>收到服务端：CMD_ERROR, 数据长度：%d", _sock, header->dataLength);
		} break;
		default:
		{
			printf("<SOCKET=%d>收到服务端未知消息, 数据长度：%d", _sock, header->dataLength);
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

	//每次接收数据的缓冲区
	char _szRecv[RECV_BUFF_SIZE];

	//第二缓冲区 消息缓冲区
	char _szMsgBuf[RECV_BUFF_SIZE * 10];

	//消息缓冲区里数据尾部位置
	int _lastPos;

};

#endif // !_client_hpp_
