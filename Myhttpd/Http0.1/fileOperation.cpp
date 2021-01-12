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
	if (!_access(thisFilePath.c_str(), 0x0))
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

	std::cout << FilePath << std::endl;
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

bool findFolderFiles(std::string path, std::vector<std::string>& files)
{
	//文件句柄
	long hFile = 0;
	struct _finddata_t fileinfo;
	std::string p;

	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//判断文件是文件夹
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					findFolderFiles(p.assign(path).append("\\").append(fileinfo.name), files);
				}
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
		return true;
	}
	return false;

}

bool urlMateFile(SOCKET client, std::string url)
{
	//当前目录下的所有文件夹
	std::vector<std::string> folder;
	std::string filePath = thisFilePath;
	if (_access(thisFilePath.c_str(), 0x0))
		filePath = getPath();
	if ((url.find_last_of("/") + 1 == url.length()))
		url.erase(url.find_last_of("/"));
	int g;
	if ((g = url.substr(url.find_last_of("/"), url.length()).find('.')) == -1 || url.substr(url.find_last_of("/"), url.length()).find(".html") != -1)
	{
		if (g == -1)
			url.append(".html");
		std::string path = filePath + "\\templates" + url;
		pathToWa(path, '/', FILE_PATH_CHAR);
		if (!_access(path.c_str(), 0x0))
		{
			hreaders(client);
			sendFile(client, path);
			return true;
		}
		std::cout << "urlMateFile: " << "无法找到 " << path << std::endl;
	} {  //删除vector的元素和其所占的内存空间
		std::vector<std::string> tmp = folder;
		folder.swap(tmp);
	}
	folder = std::vector<std::string>();
	{ //获取当前目录下的所有文件夹，将文件名保存到folder
		long hFile = 0;  //文件句柄
		std::string p;
		struct _finddata_t fileinfo;
		if ((hFile = _findfirst(p.assign(filePath).append("\\*").c_str(), &fileinfo)) != -1)
		{
			do
			{
				//判断文件是文件夹
				if ((fileinfo.attrib & _A_SUBDIR))
				{
					if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
						folder.push_back(p.assign("\\").append(fileinfo.name));
				}
			} while (_findnext(hFile, &fileinfo) == 0);
			_findclose(hFile);
		}
	}
	std::vector<std::string> absFiles;
	int j = 0, filePathLen = filePath.length();
	for (int i = 0; i < folder.size(); i++)
	{
		if (findFolderFiles(filePath + folder[i], absFiles))
		{
			for (; j < absFiles.size(); j++)
			{
				absFiles[j].erase(0, filePathLen);
				pathToWa(absFiles[j], FILE_PATH_CHAR, '/');
				//std::cout << absFiles[j] << "\n";
				if (absFiles[j] == url)
				{
					//std::cout << absFiles[j] << "\n";
					std::string path = filePath + absFiles[j];
					pathToWa(path, '/', FILE_PATH_CHAR);
					//std::cout << path << "\n";
					if (handleFileHread(client, path))
					{
						sendFile(client, path);
						return true;
					}
					std::cout << "urlMateFile: " << "handleFileHread error! " << std::endl;
					return false;
				}
			}
		}
		else
			std::cout << "无法找到" << filePath + folder[i] << "下的文件" << std::endl;
	}

	return false;
}

bool handleFileHread(SOCKET client, std::string path)
{
	pathToWa(path, '/', FILE_PATH_CHAR);
	if (_access(thisFilePath.c_str(), 0x0))
	{
		std::cout << "handleFileHread: " << "_access error! " << std::endl;
		return false;
	}
	int i = path.find_last_of('\\');
	int j;
	if ((j = path.find_last_of('.')) != -1)
	{
		std::string suffix = path.substr(j, path.length());
		//std::cout << suffix << "\n";
		if (!sendString(client, "handleFileHread", "HTTP/1.1 200 OK\r\n"))
			return false;
		if (suffix == ".html" || (suffix == ".js" || (suffix == ".txt" || suffix == ".md")))
		{
			if (!sendString(client, "handleFileHread", SERVER_STRING))
				return false;
			if (!sendString(client, "handleFileHread", "Content-Type: " + suffix.substr(1, suffix.length()) + "\r\n\r\n"))
				return false;
		}
		else
		{
			//std::cout << suffix.substr(1, suffix.length()) << "\n";
			std::stringstream ss;
			if (!sendString(client, "handleFileHread", SERVER_STRING))
				return false;
			if (!sendString(client, "handleFileHread", "Accept-Ranges: bytes\r\n"))
				return false;
			ss << getFileSize(path) << "\r\n";
			if (!sendString(client, "handleFileHread", "Content-Length: " + ss.str()))
				return false;
			if (!sendString(client, "handleFileHread", "Content-Type: " + suffix.substr(1, suffix.length()) + "\r\n\r\n"))
				return false;
		}
	}
	return true;
}
