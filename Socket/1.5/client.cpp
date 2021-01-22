#include <thread>
#include "client.hpp"

void cmdThread(ClientSocket* sock)
{
	char cmdBuf[128] = {};

	while (true)
	{
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit"))
		{
			printf("退出...\n");
			sock->Close();
			return;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{
			Login login;
			strcpy(login.userName, "conch");
			strcpy(login.Password, "1234");
			sock->sendData(&login);
		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			Logout logout;
			strcpy(logout.userName, "conch");
			sock->sendData(&logout);
		}
		else {
			printf("无效指令...\n");
		}
	}
}

int main()
{
	ClientSocket client;
	//client.InitSocket();
	if (SOCKET_ERROR == client.Connect("192.168.112.129", 8888))
	{
#ifdef _WIN32
		system("pause");
#endif
		return 0;
	}

	std::thread th(cmdThread, &client);
	th.detach();

	while (client.IsRun())
	{
		client.OnRun();
	}

	client.Close();
#ifdef _WIN32
	system("pause");
#endif
}
