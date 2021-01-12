/* 分配路由
 * 请将要分配的路由和路由处理方法按格式写好 */

#include "myhd.h"

std::vector<UrlFunction> urlpatterns = 
{
	UrlFunction("/", home),
	UrlFunction("/favicon.ico", ico),
	UrlFunction("/home", home),
};

void home(SOCKET client, std::string request, ...)
{
	clear_client(client, false);

	hreaders(client);
	sendFile(client, GET_TEMPLATES_PATH + "\\home.html");
}

void ico(SOCKET client, std::string request, ...)
{
	std::string buf, path = GET_STATIC_PATH + "\\images\\favicon.ico";
	std::stringstream ss;

	buf.assign("HTTP/1.1 200 OK\r\n");
	send(client, buf.c_str(), buf.length(), 0);
	buf.assign(SERVER_STRING);
	send(client, buf.c_str(), buf.length(), 0);
	buf.assign("Accept-Ranges: bytes\r\n");
	send(client, buf.c_str(), buf.length(), 0);
	buf.assign("Content-Length: ");
	ss << getFileSize(path) << "\r\n";
	buf.append(ss.str());
	send(client, buf.c_str(), buf.length(), 0);
	buf.assign("Content-Type: image/ico\r\n");
	send(client, buf.c_str(), buf.length(), 0);
	send(client, "\r\n", 2, 0);

	sendFile(client, path);
}
