#include"TcpServer.hpp"


int main()
{
	TcpServer server;

	server.InintServer(8080);

	while (server.IsRunning())
	{
		server.OnRun();
	}
	server.Close();
	cout << "Server close!" << endl;
	return 0;
}