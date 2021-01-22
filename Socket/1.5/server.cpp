#include "server.hpp"

int main()
{
	ServerSocket server;
	server.InitSocket();
	server.Bind(nullptr, 8888);
	server.Listen(5);

	while (server.IsRun())
	{
		server.OnRun();
	}
	server.Close();

	return 0;
}
