/*******************************************************/
/* 该文件是定义函数，声明在myhd.h */
/*******************************************************/

#include "myhd.h"
#include <fstream>

extern std::string thisFilePath;

/* 处理GET请求 */
bool getHandle(SOCKET client, std::string url)
{
	myh::U_INT fp = 0;
	std::vector<std::string> absFiles;
	clear_client(client, false);

	std::string filePath = fileOperation(absFiles, "templates");
	int absSize = absFiles.size();
	//std::cout << filePath << std::endl;

	for (int i = 0; i < absSize; i++)
		pathToWa(absFiles[i], FILE_PATH_CHAR, '/');

	myh::U_INT urlLen = url.length();
	if (urlLen == 0)
	{
		//std::cout << "urlLen\n";
		return false;
	}
	myh::U_INT urlLastF = url.find_last_of("/");
	if (urlLastF == -1)
	{
		//std::cout << "urlLastF\n";
		return false;
	}
	if ((urlLastF+1 == urlLen))
	{
		url.erase(urlLastF);
		url.append(".html");
	}
	else if (url.substr(urlLastF, urlLen).find('.') == url.npos)
		url.append(".html");

	for (myh::U_INT i = 0; i < absFiles.size(); i++)
	{
		if (absFiles[i] == url)
		{
			//Do Something...
			hreaders(client);
			sendFile(client, filePath + absFiles[i]);
			return true;
		}
	}
	return false;
}

/* 处理POST请求 */
void postHandle(SOCKET client, std::string url)
{

}

std::string fileOperation(std::vector<std::string>& files, std::string folder)
{
	std::string filePath = thisFilePath;
	if (filePath.length() == 0)
		filePath = getPath();

	if (!findFolderFiles(filePath, files, folder))
	{
		std::cout << "find\n";
		return std::string();
	}

	folder.insert(0, "\\");

	int size = files.size();
	int filePathSize = filePath.length() + folder.length();
	for (int i = 0; i < size; i++)
		files[i].erase(0, filePathSize);

#if 0
	size = files.size();
	for (int i = 0; i < size; i++)
	{
		std::cout << files[i] << std::endl;
	}
#endif
	return filePath.append(folder);
}

/* 找到要找的文件夹，并存储文件夹里的所有文件的路径 */
bool findFolderFiles(std::string path, std::vector<std::string>& files, std::string folder)
{
	//文件句柄
	long hFile = 0;
	struct _finddata_t fileinfo;
	std::string p;
	//向files里添加条件开关
	static bool finTem = false;

	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//判断文件是文件夹
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					if (strcasecmp(fileinfo.name, folder) && !finTem)
					{
						finTem = true;
						findFolderFiles(p.assign(path).append("\\").append(fileinfo.name), files, folder);
						finTem = false;
						_findclose(hFile);
						return true;
					}
					else if(finTem)
						findFolderFiles(p.assign(path).append("\\").append(fileinfo.name), files, folder);
				}
			}
			else
			{
				if (finTem)
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	return false;
}

/* 将TCHAR类型转换成string */
void tcharTostring(TCHAR* input, std::string& output)
{
	char ch[MAX_PATH*2] = "";
	int length = WideCharToMultiByte(CP_ACP, 0, input, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, input, -1, ch, length, NULL, NULL);
	output.assign(ch);
}

/* 将ch1的文件路径符换成 ch2 */
void pathToWa(std::string& path, char ch1, char ch2)
{
	int pron = 0;
	//std::cout << path << std::endl;
	while ((pron=path.find(ch1, pron)) != path.npos)
	{
		path.replace(pron, 1, &ch2);
		pron++;
	}
	//std::cout << path << std::endl;
}

bool sendFile(SOCKET client, std::string filePath)
{
	pathToWa(filePath, '/', FILE_PATH_CHAR);
	//std::cout << filePath << std::endl;

	char* ch = new char[READ_FILE_SIZE];
	std::ifstream ina;
	ina.open(filePath.c_str(), std::ios::binary | std::ios::in);
	if (!ina.is_open())
	{
		std::cout << client << ": " << "sendFile: 打开文件失败" << std::endl;
		delete[] ch;
		return false;
	}
	long long sendLen = 1;

	while (!ina.eof() && sendLen > 0)
	{
		ina.read(ch, READ_FILE_SIZE - 1);
		sendLen = ina.gcount();
		//Sleep(100);
		sendLen = send(client, ch, sendLen, 0);
		if (sendLen == SOCKET_ERROR)
		{
			std::cout << client << ": " << "sendFile: send error! " << WSAGetLastError() << std::endl;
			break;
		}
		else if (sendLen == 0)
			std::cout << client << ": " << "sendFile: send error! 客户端已退出。" << std::endl;
	}
	ina.close();
	delete[] ch;
	return true;
}

/* 获取当前工作路径 */
std::string getPath(void)
{
	if (!access(thisFilePath.c_str(), 0x0))
		return thisFilePath;
	std::cout << "getPath: access error!" << std::endl;

	std::string FilePath;

	{   //获取当前工作路径
		//看好多文章都说如果在窗口中选了其他路径会导致获取到的就不是当前路径
		//但用下面的方法就会和main函数里的WSAStartup调用冲突，但我一个服务器应用又不会更换路径，
		//所以先暂时用着有问题再说
		TCHAR Buffer[MAX_PATH];
		//失败返回0，成功返回写入缓冲区的字符串长度
		if (!GetCurrentDirectory(MAX_PATH, Buffer))
		{
			std::cout << "getPath: GetCurrentDirectory Error! 获取文件路径失败！"
				<< WSAGetLastError() << std::endl;
		}
		tcharTostring(Buffer, FilePath);  //转换类型
	}
	//FilePath.append("\\Debug");

	//std::cout << thisFilePath << std::endl;
	/*  //用这个会报莫名其妙的错误
	LPTSTR szPath;
	memset(&szPath, 0, sizeof(szPath));
	GetModuleFileName(NULL, szPath, sizeof(szPath));
	*/

	return FilePath;
}

long long getFileSize(std::string filePath)
{
	struct _stat info;
	//获取指定路径的文件或者文件夹的信息。
	//成功返回0，失败返回-1，具体错误码保存在errno中
	if (_stat(filePath.c_str(), &info) != -1)
	{
		size_t size = info.st_size;
		return size;
	}
	std::cout << "getFileSize: 获取文件信息失败！" << errno << std::endl;

	std::ifstream ina;
	ina.open(filePath.c_str(), std::ios::binary | std::ios::in);
	if (!ina.is_open())
	{
		std::cout << "getFileSize: 打开文件失败" << std::endl;
		return -1;
	}
	std::streamoff sendLen = 0;
	ina.seekg(0, std::ios::end);
	sendLen = ina.tellg();
	ina.close();

	return sendLen;
}
