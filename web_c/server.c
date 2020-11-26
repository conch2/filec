#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT 6669

int crate_socket(void)
{
	int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_socket == -1)
	{
		perror("socket");
		return -1;
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));  // 初始化addr

	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int ret = bind(listen_socket, (struct sockaddr*)&addr, sizeof(addr));
	if (ret == -1)
	{
		perror("bind");
		return -1;
	}

	ret = listen(listen_socket, 5);
	if (ret == -1)
	{
		perror("listen");
		return -1;
	}

	return listen_socket;
}

int wait_client(listen_socket)
{
	struct sockaddr_in cliaddr;
	unsigned int addrlen = sizeof(cliaddr);
	fprintf(stdout, "等待客户端链接...\n");
	int client_socket = accept(listen_socket, (struct sockaddr *)&cliaddr, &addrlen);
	if (client_socket == -1)
	{
		perror("accept");
		return -1;
	}

	fprintf(stdout, "成功接受到客户端：%s\n", inet_ntoa(cliaddr.sin_addr));

	return client_socket;
}

void hanld_client(int listen_socket, int client_socket)
{
	int ret, ch;
	char str[1024];

	while (1)
	{
		ret = read(client_socket, str, 1024);
		// 读取的数量应大于或等于客户端穿过来的大小
		if (ret == -1)
		{
			perror("read");
			return ;
		}

		if (ret == 0)
		{
			break;
		}
		printf("客户端的数据：%s\n", str);
		ch++;

	}
	printf("ch = %d\n", ch);
}

int main(void)
{
	int listen_socket = crate_socket();
	int client_socket = wait_client(listen_socket);
	hanld_client(listen_socket, client_socket);

	close(listen_socket);
	return 0;
}
