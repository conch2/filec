
/***********************************************
#
#      Filename: classes.cpp
#
#        Author: 
#   Description: ---
#        Create: 2021-01-05 03:23:02
# Last Modified: 2021-01-05 03:23:02
***********************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "myhttpd.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace myhtp
{
void *accept_request(void *);

Httpd::Httpd(const UINT port) : port(port), 
		server_sock(0x0), client_sock(0x0)
{
	memset(&this->server_addr, 0x0, sizeof(this->server_addr));
	memset(&this->client_addr, 0x0, sizeof(this->client_addr));
	server_addr.sin_family      = AF_INET;
	server_addr.sin_port        = htons(this->port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	this->client_len = sizeof(this->client_addr);
	this->server_len = sizeof(this->server_addr);
}

Httpd::~Httpd() {}

void Httpd::run(void)
{
	pthread_t newthread;

	this->startup();
	printf("httpd run on port %d\n", this->port);

	while(true)
	{
		this->client_sock = accept(this->server_sock,
				(struct sockaddr *)&this->client_addr,
				&this->client_len);
		if(this->client_sock == -1)
		{
			perror("accept");
			exit(1);
		}
		if(pthread_create(&newthread, NULL, 
					accept_request, (void *)this) != 0)
			perror("pthread_create");
	}

	close(this->client_sock);
}

int Httpd::startup(void)
{
	int on = 1;
	server_sock = socket(PF_INET, SOCK_STREAM, 0);

	if((setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0)
	{
		perror("setsockopt");
		exit(1);
	}
	if(bind(this->server_sock, (struct sockaddr *)&this->server_addr,
				sizeof(this->server_addr)) < 0)
	{
		perror("bind");
		exit(1);
	}
	if(listen(this->server_sock, 5) < 0)
	{
		perror("listen");
		exit(1);
	}

	return this->server_sock;
}

void *accept_request(void *sock)
{
	Httpd *_this = (Httpd *)sock;

	printf("client : %s as %lu\n",
			inet_ntoa(_this->client_addr.sin_addr), time(NULL));

	_this->get_len();

	close(_this->client_sock);
	return NULL;
}

int Httpd::get_len(void)
{
	int i = 0;
	char c = '\0';
	int n;

	while(i < 100 && c != '\n')
	{
		n = recv(this->client_sock, &c, 1, 0);
		if(n > 0)
			putchar(c);
		else
			c = '\n';
	}

	return i;
}

}  //end myhtp

