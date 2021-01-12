/*******************************************************/
/* 该工程已屏蔽4996的错误代码，该错误是由localtime函数引起
 * 这个问题就是本地时间和世界时间冲突引起 
 * 函数声明在myh.h里 */
/*******************************************************/

#include "myhd.h"

std::string thisFilePath; 

/* 主程序
 * 主要是创建套接字，等待客户端连接。
 * 当客户端接入的时候创建一个新线程来处理与客户端的数据交互。 */
void run(void)
{
	struct sockaddr_in client_addr;
	int client_len = sizeof(client_addr);
	SOCKET server = startup();
	printf("httpd run on port %d\n", SERVER_PORT);
	/*  //设置发送缓冲区为nZero
	int nZero = 0;
	int sizeN = sizeof(nZero);
	//调用失败会放回-1，成功返回0
	if (setsockopt(server_sock, SOL_SOCKET, SO_SNDBUF, (char*)&nZero, sizeN) == -1)
		printf("err\n"); 
	*/
	ThreadPool tp(THREADPOOL_SIZE);

	{ //设置发送延迟
		/* 设置如果发送缓冲区中还有数据线程进入休眠模式，等待缓冲区内的数据被客户端接收
		 * 如果超过l_linger的时间限制则强行关闭套接字 */
		struct linger so_linger;
		so_linger.l_onoff = TRUE;
		so_linger.l_linger = RESPOND_IN_TIME;  //时间，单位秒

		int z = setsockopt(server, SOL_SOCKET, SO_LINGER, (char*)&so_linger, sizeof(so_linger));
		if (z == -1)
			printf("setsockopt(server_sock, SOL_SOCKET, SO_LINGER, "
				"(char*)&so_linger, sizeof(so_linger)) error! (htt.cpp: run()) %x\n", WSAGetLastError());
	}  // end

	while (true)
	{
		SOCKET client_sock = accept(server,
			(SOCKADDR*)&client_addr,
			&client_len);

		if (client_sock < 0)
		{
			printf("accept error !%x\n", WSAGetLastError());
		}
		//std::cout << inet_ntoa(client_addr.sin_addr) << " ";
		if (tp.push(client_sock))
		{
			printf("push error!\n");
			closesocket(client_sock);
		}
		//刷新线程池
		tp.renovate();
	}
	closesocket(server);
}

/* 创建socket套接字，绑定套接字。 */
SOCKET startup(void)
{
	SOCKET server;
	char on[255];
	struct sockaddr_in server_addr;
	memset(on, 0, sizeof(on));
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	//PF_INET在windows与AF_INET作用相同，IPPROTO_TCP传输协议即ICP
	server = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server == INVALID_SOCKET)
	{
		printf("socket error !%x\n", WSAGetLastError());
		system("pause");
		exit(1);
	}
	if ((setsockopt(server, SOL_SOCKET, SO_REUSEADDR, on, sizeof(on))) < 0)
	{
		printf("setsockopt error!%x\n", WSAGetLastError());
		system("pause");
		exit(1);
	}
	if (bind(server, (struct sockaddr*)&server_addr,
		sizeof(server_addr)) < 0)
	{
		printf("bind error! %x\n", WSAGetLastError());
		system("pause");
		exit(1);
	}
	if (listen(server, THREADPOOL_SIZE) < 0)
	{
		printf("listen error! %x\n", WSAGetLastError());
		system("pause");
		exit(1);
	}
	return server;
}

int main(int argc, char* argv[])
{
#ifdef WIN32  //windows下
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2) // socket版本，这里的是2.2
		, &wsaData) != 0)
	{
		printf("WSAStartup error! %lu", GetLastError());
		return 0;
	}
#endif

	thisFilePath = argv[0];
	thisFilePath.erase(thisFilePath.find_last_of(FILE_PATH_CHAR), thisFilePath.length());
	//std::cout << thisFilePath << "\n";

	run();

	//解除与socket的绑定
	WSACleanup();
	system("pause");
	return 0x0;
}
