/*******************************************************/
/* 该文件是定义函数，声明在myhd.h */
/*******************************************************/

#include <ctime>
#include "myhd.h"
#include <cctype>
#include <iomanip>

extern std::vector<UrlFunction> urlpatterns;

//与ThreadPool::push是捆绑关系
#define FIND_NULL_FP for (myh::U_INT i = 0; i < size; i++) { \
		if (data[i] == nullptr) {                     \
			voidfp = i;                               \
			return 0x0;                               \
		}}
int ThreadPool::push(const SOCKET client_sock)
{
	if (data[voidfp] == nullptr)
	{
		data[voidfp] = new std::thread(accept_request, client_sock, this, voidfp);
		//std::cout << "创建了一个线程" << std::endl;
		this->is_out[voidfp] = 0x0;
		FIND_NULL_FP
		return 0x0;
	}
	else
	{
		for (myh::U_INT i = 0; i < size; i++)
		{
			if (data[i] == nullptr)
			{
				data[i] = new std::thread(accept_request, client_sock, this, i);
				//std::cout << "创建了一个线程" << std::endl;
				this->is_out[i] = 0x0;
				FIND_NULL_FP
				return 0x0;
			}
		}
	}

	return THREADPOOL_FULL;
}

void ThreadPool::renovate(void)
{
	for (myh::U_INT i = 0; i < this->size; i++)
	{
		if (data[i] != nullptr)
		{
			if (is_out[i] == 0x02)
			{
				data[i]->join();
				delete data[i];
				data[i] = nullptr;
				//std::cout << "消灭了一个线程" << std::endl;
				this->is_out[i] = 0x0;
			}
		}
	}
}

bool UrlFunction::run(SOCKET client, std::string& url, std::string& request)
{
	if (this->url != url)
		return false;

	this->funFp(client, request);
	return true;
}

/* 线程函数，用与处理与客户端的交互。 */
void accept_request(const SOCKET client, ThreadPool* tp, const int tpid)
{
	//线程开始的标志
	tp->is_out[tpid] = 0x01;
	//printf("client id : %d\n", tpid);
	std::string req_hread; //第一行请求头
	std::string method;    //请求方式
	std::string url;       //访问的url
	myh::U_INT i=0, j=0;
	myh::U_INT req_len;

	get_len(client, req_hread);
	req_len = req_hread.length();
	if (!req_len)
	{
		not_found(client);
		closesocket(client);
		//线程结束的标志
		tp->is_out[tpid] = 0x02;
		return;
	}

	std::cout << "Client: " << client << ": " << req_hread + " ";
	print_time();
	std::cout << std::endl;

	//isspace(char) 判断字符是否是空白字符，如空格、回车
	while (j < req_len && !isspace(req_hread.at(j)))
		j++;
	method = req_hread.substr(0, j);

	while (j < req_len && isspace(req_hread.at(j)))
		j++;
	i = j;
	while (j < req_len && !isspace(req_hread.at(j)))
		j++;
	url = req_hread.substr(i, j-i);

	if (!strcasecmp(method, std::string("GET")) && !strcasecmp(method, std::string("POST")))
	{
		clear_client(client, false);
		unimplemented(client);
		closesocket(client);
		tp->is_out[tpid] = 0x02;
		return;
	}

	if ((url.find_last_of("/") + 1 == url.length()))
		url.erase(url.find_last_of("/"));

	for (i = 0; i < urlpatterns.size(); i++)
	{
		if (urlpatterns[i].run(client, url, method))
			break;
	}

	if (i == urlpatterns.size())
	{
		if (!getHandle(client, url))
		{
			//clear_client(client, false);
			not_found(client);
		}
	}

	//关闭socket
	closesocket(client);
	//线程结束的标志
	tp->is_out[tpid] = 0x02;
}

#define RIGTH_FILL_ZERO std::right << std::setw(2) << std::setfill('0')
/* 输出当前时间 */
void print_time(void)
{
	time_t now = time(0);
	struct tm* curr_time = localtime(&now);

	std::cout << curr_time->tm_year + 1900 << '-' <<
		RIGTH_FILL_ZERO << curr_time->tm_mon + 1 << '-' <<
		RIGTH_FILL_ZERO << curr_time->tm_mday << ' ' <<
		RIGTH_FILL_ZERO << curr_time->tm_hour << ':' <<
		RIGTH_FILL_ZERO << curr_time->tm_min << ':' <<
		RIGTH_FILL_ZERO << curr_time->tm_sec;
}

/* 获取一行数据 */
void get_len(const SOCKET client, std::string& req_hread)
{
	int i;
	char c = 0;
	req_hread.assign("");

	while (c != '\n')
	{
		i = recv(client, &c, 1, 0);
		if (i > 0)
		{
			if (c == '\r')
			{
				i = recv(client, &c, 1, MSG_PEEK);
				if ((i > 0) && (c == '\n'))
					recv(client, &c, 1, 0);
				else
					c = '\n';
			}
			if (c != '\n')
				req_hread = req_hread + c;
		}
		else
			c = '\n';
	}
}

/* send Hreaders */
void hreaders(const SOCKET client)
{
	if (!sendString(client, "hreaders", "HTTP/1.1 200 OK\r\n"))
		return;
	if (!sendString(client, "hreaders", SERVER_STRING))
		return;
	if (!sendString(client, "hreaders", "Content-Type: text/html;charset=utf-8\r\n\r\n"))
		return;
}

/* 清空一次接收缓冲区的内容。 */
void clear_client(const SOCKET client, bool print)
{
	char* str = new char[getRCVbuf(client, SO_RCVBUF) + 1];

	//std::cout << getRCVbuf(client_sock, SO_RCVBUF) << std::endl;

	int size = recv(client, str, getRCVbuf(client, SO_RCVBUF), 0);
	if (size == SOCKET_ERROR)
	{
		std::cout << "clear_client: recv error! " << WSAGetLastError() << std::endl;
		str[0] = '\0';
	}
	else if (size > 0)
		str[size] = '\0';

	if (print)
	{
		std::cout << str;
	}
	delete[] str;
}

/* Error 501
 * 通知客户端请求的web方法未实现
 * Parameter: 客户端socket */
void unimplemented(const SOCKET client)
{
	if (!sendString(client, "unimplemented", "HTTP/1.1 501 Method Not Implemented\r\n"))
		return;
	if (!sendString(client, "unimplemented", SERVER_STRING))
		return;
	if (!sendString(client, "unimplemented", "Content-type: text/html charset=utf-8\r\n\r\n"))
		return;
	if (!sendString(client, "unimplemented", "<HTML><HEAD><TITLE>Method Not Implemented\r\n</TITLE></HEAD>\r\n"))
		return;
	if (!sendString(client, "unimplemented", "<BODY>\r\n<H1>Error:501</H1>\r\n"))
		return;
	if (!sendString(client, "unimplemented", "<P>HTTP request method not supported.\r\n</BODY></HTML>\r\n"))
		return;
}

/* 获取接收、发送缓冲区的大小。 */
myh::U_INT getRCVbuf(SOCKET sock, const int vlaur)
{
	int nZero = 0;
	int sizeN = sizeof(nZero);
	if (getsockopt(sock, SOL_SOCKET,
		SO_RCVBUF,       //接收缓冲区
		(char*)&nZero, &sizeN) == -1)
	{
		printf("getsockopt error!\n");
		return 0;
	}
	if (vlaur == SO_RCVBUF)
		return nZero;
	if (getsockopt(sock, SOL_SOCKET,
		SO_SNDBUF,       //发送缓冲区
		(char*)&nZero, &sizeN) == -1)
	{
		printf("getsockopt error!\n");
		return 0;
	}

	return nZero;
}

/* 判断两个string类是否一样，不区分大小写 */
bool strcasecmp(std::string str1, std::string str2)
{
	char j;
	int str1_len = str1.length();
	int str2_len = str2.length();

	if (str1_len != str2_len)
		return false;

	for(int i=0; i < str1_len; i++)
	{
		j = str1.at(i);
		if(j > 96 && j < 123)
		{
			str1[i] = j - 32;
		}
	}

	for (int i = 0; i < str1_len; i++)
	{
		j = str2.at(i);
		if (j > 96 && j < 123)
		{
			str2[i] = j - 32;
		}
	}

	return str1 == str2;
}

void not_found(SOCKET client)
{
	if (!sendString(client, "not_found", "HTTP/1.0 404 NOT FOUND\r\n"))
		return;
	if (!sendString(client, "not_found", SERVER_STRING))
		return;
	if (!sendString(client, "not_found", "Content-Type: text/html charset=utf-8\r\n\r\n"))
		return;
	if (!sendString(client, "not_found", "<HTML><TITLE>Not Found</TITLE>\r\n"))
		return;
	if (!sendString(client, "not_found", "<BODY><H1>404</H1>\r\n<P>未知的网址\r\n"))
		return;
	if (!sendString(client, "not_found", "</BODY></HTML>\r\n"))
		return;
}

bool sendString(SOCKET client, std::string fun, std::string buf)
{
	long buffer_space = 0;
	buffer_space = send(client, buf.c_str(), buf.length(), 0);
	//std::cout << "sendString " << buffer_space << "\n";
	if (buffer_space == SOCKET_ERROR)
	{
		std::cout << client << ": " << fun << ": send error! " << WSAGetLastError() << std::endl;
		return false;
	}
	else if (buffer_space == 0)
		std::cout << client << ": " << fun << ": send error! 客户端已退出。" << std::endl;
	return true;
}
