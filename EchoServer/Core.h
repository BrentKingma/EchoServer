#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <string>
#include "SafeQueue.h"

#define DEFAULT_PORT "22556"
#define DEFAULT_BUFLEN 144

#pragma comment(lib, "Ws2_32.lib")
class Core
{
private:
	SafeQueue<std::string>* queueRef;

	WSADATA wsaData;
	struct addrinfo* result = NULL, * ptr = NULL, hints;
	int iResult;
	SOCKET listenSocket;
	bool runServer;
	std::string msg;

	std::thread serverThread;

public:
	Core()
	:runServer(true)
	{};
	int Setup(SafeQueue<std::string>* queueRef);
	void Run();
	void Stop();
};

