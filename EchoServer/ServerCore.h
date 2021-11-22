#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <string>
#include "Logger.h"

#define DEFAULT_PORT "22556"
#define DEFAULT_BUFLEN 144

#pragma comment(lib, "Ws2_32.lib")
class ServerCore
{
private:
	Logger* logger;
	WSADATA wsaData;
	struct addrinfo* result = NULL, * ptr = NULL, hints;
	int iResult;
	SOCKET listenSocket;
	bool runServer;
	std::string msg;

	std::thread serverThread;

public:
	ServerCore()
	:runServer(true)
	{};
	int Setup(Logger* logger);
	void Run();
	void Stop();
};

