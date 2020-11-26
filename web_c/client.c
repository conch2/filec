#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(){
    //创建套接字
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    //向服务器（特定的IP和端口）发起请求
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr.sin_port = htons(6669);  //端口
    int i = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	if (i == -1)
	{
		printf("无法链接服务器！\n");
		return -1;
	}
    //读取服务器传回的数据
    char buffer[1024];
//	read(sock, buffer, sizeof(buffer)-1);

//	printf("Message form server: %s\n", buffer);
  
	while (strcmp(buffer, "exit"))
	{
		fprintf(stdout, "请输入字符：");
		scanf("%s", buffer);
		getchar();
		write(sock, buffer, sizeof(buffer));
	}
    //关闭套接字
    close(sock);
    return 0;
}

