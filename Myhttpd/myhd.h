#pragma once

#ifndef __MYHD_H__
#define __MYHD_H__
#include <io.h>
#include <vector>
#include <thread>
#include <string>
#include <iostream>
#include <winsock2.h>
#include <initializer_list>

#pragma comment(lib, "ws2_32.lib")

#ifndef __linux__
#define FILE_PATH_CHAR         '\\'
#else 
#define FILE_PATH_CHAR         '/'
#endif

//socket监听的端口
#define SERVER_PORT            8888

//服务器信息
#define SERVER_STRING "Server: ConchHTTP/0.2\r\n"

//线程池的大小
#define THREADPOOL_SIZE        0x100
#define THREADPOOL_FULL        0x033
#define THREADPOOL_POP_ERROR   0x031
#define THREADPOOL_PUSH_ERROR  0x032
#define FIFO_ERROR_POP         0x048
#define FIFO_ERROR_PUSH        0x049

//获取当前工作路径
#define GET_WORK_PATH          getPath()

//获取当前工作路径下的静态文件夹
#define GET_STATIC_PATH        getPath().append("\\static")

//获取当前工作路径下的模板文件夹
#define GET_TEMPLATES_PATH     getPath().append("\\templates")

//应答时间
#define RESPOND_IN_TIME        0x01E

//一次读取文件的字节长度
#define READ_FILE_SIZE         1024 * 32

namespace myh
{
	typedef unsigned int U_INT;
	typedef unsigned long ULONG;

	/* 模板类，数据类型，先进先出 */
	template <typename T>
	class Fifo
	{
	public:
		Fifo(U_INT us_size) : size(us_size), fp(0x0), dataSize(0x0)
		{
			this->data = new T[this->size];
		}

		virtual ~Fifo() 
		{
			delete[] data;
		}

		//存入数据
		void push(const T ch)
		{
			U_INT num = this->size - (this->fp + this->dataSize);

			if (this->fp + this->dataSize < this->size)
			{
				data[this->fp + this->dataSize] = ch;
				this->dataSize++;
			}
			else if (this->fp > num)
			{
				this->data[num] = ch;
				this->dataSize++;
			}
			else
			{
				throw FIFO_ERROR_PUSH;
			}
		}

		//取出数据
		T pop(void)
		{
			if (!this->dataSize)
			{
				throw FIFO_ERROR_POP;
			}
			if (this->fp < this->size - 1)
			{
				this->dataSize--;
				return this->data[fp++];
			}
			else
			{
				this->dataSize--;
				this->fp = 0x0;
				return this->data[this->size - 1];
			}
		}

	protected:
		T* data;
		U_INT fp;
		U_INT size;
		U_INT dataSize;

	private:

	};
}

/* 一个简单的线程内存池，方便管理线程
 * 函数定义在myHreader.cpp里 */
class ThreadPool
{
public:
	ThreadPool(myh::U_INT us_size) : size(us_size), voidfp(0)
	{
		is_out = new int[this->size];
		data = new std::thread * [this->size];
		memset(is_out, 0, sizeof(int) * this->size);
		memset(data, 0, sizeof(std::thread*) * this->size);
	}
	virtual ~ThreadPool()
	{
		for (myh::U_INT i = 0; i < this->size; i++)
		{
			if (data[i] != nullptr)
			{
				data[i]->join();
				delete data[i];
				data[i] = nullptr;
				this->is_out[i] = 0x0;
			}
		}
		delete[] is_out;
	}
	int push(const SOCKET);
	/* 刷新内存池，将已经结束的线程安全退出并清理内存 */
	void renovate(void);

protected:

private:
	//程序当前状态
	int voidfp;
	int* is_out;
	myh::U_INT size;
	std::thread** data;

	friend void accept_request(const SOCKET client, ThreadPool* tp, const int tpid);

};

class UrlFunction
{
public:
	UrlFunction(std::string url, void (*funFp)(SOCKET, std::string, ...)) : 
		url(url), funFp(funFp) 
	{
		if ((this->url.find_last_of("/") + 1 == this->url.length()))
			this->url.erase(this->url.find_last_of("/"));
	}

	~UrlFunction() {}

	std::string getUrl(void)
	{
		return this->url;
	}

	bool run(SOCKET client, std::string& url, std::string& request);

protected:

private:
	std::string url;
	void (*funFp)(SOCKET, std::string, ...);

};

/* 主程序
 * 主要是创建套接字，等待客户端连接。
 * 当客户端接入的时候创建一个新线程来处理与客户端的数据交互。 */
void run(void);

/* 创建socket套接字，绑定套接字。 */
SOCKET startup(void);

/* 线程函数，用与处理与客户端的交互。 */
void accept_request(const SOCKET client, ThreadPool* tp, const int tpid);

/* 输出当前时间 */
void print_time(void);

/* 获取一行数据 */
void get_len(const SOCKET client, std::string& req_hread);

/* send Hreaders */
void hreaders(const SOCKET client);

/* 清空接收缓冲区的内容。
 * 值得注意的是这个函数只是清空一次缓冲区的内容，
 * 当还有数据从客户端传来时缓冲区内就是新的数据了。 */
void clear_client(const SOCKET client, bool print);

/* Error 501
 * 通知客户端请求的web方法未实现
 * Parameter: 客户端socket */
void unimplemented(const SOCKET client);

/* 获取接收、发送缓冲区的大小。
 * vlaur为SO_RCVBUF是获取接收缓冲区的大小，为SO_SNDBUF是获取发送缓冲区的大小。 */
myh::U_INT getRCVbuf(SOCKET sock, const int vlaur);

/* 判断两个string类是否一样，不区分大小写
 * 先将字符串的所有字母该成大写，再判断
 * 如果两个字符串一样返回true，否则返回false */
bool strcasecmp(std::string str1, std::string str2);

/* Error 404 */
void not_found(SOCKET client);

/* 向客户端发送一条string，发送成功返回true否则返回false */
bool sendString(SOCKET client, std::string fun, std::string buf);

/* 处理GET请求 */
bool getHandle(SOCKET client, std::string url);

/* 处理POST请求 */
void postHandle(SOCKET client, std::string url);

/* 在当前工作路径下找templates文件夹
 * 找得到就返回一个templates的绝对路径，找不到就返回一个空的string类
 * 在templates里面找到的文件将存在vector<std::string>里 */
std::string fileOperation(std::vector<std::string>& files, std::string folder);

/* 找到要找的文件夹，并存储文件夹里的所有文件的路径 */
bool findFolderFiles(std::string path, std::vector<std::string>& files, std::string folder);

/* 将TCHAR类型转换成string */
void tcharTostring(TCHAR* input, std::string& output);

/* 将ch1的文件路径符换成 ch2 */
void pathToWa(std::string& path, char ch1, char ch2);
bool sendFile(SOCKET client, std::string filePath);

/* 获取当前工作目录 */
std::string getPath(void);
long long getFileSize(std::string filePath);

void home(SOCKET client, std::string request, ...);
void index(SOCKET client, std::string request, ...);
void image(SOCKET client, std::string request, ...);
void image2(SOCKET client, std::string request, ...);
void image3(SOCKET client, std::string request, ...);
void ico(SOCKET client, std::string request, ...);

#endif
