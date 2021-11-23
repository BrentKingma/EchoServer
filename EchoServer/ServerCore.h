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
	bool runListenThread;
	std::string msg;
	TIMEVAL timeoutLength;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen;

	std::thread listenThread;
	std::vector<std::thread*> allThreads;

public:
	ServerCore() :
		runListenThread(true),
		hints(),
		logger(nullptr),
		iResult(0),
		listenSocket(INVALID_SOCKET),
		wsaData(),
		allThreads(),
		recvbuflen(DEFAULT_BUFLEN),
		recvbuf(),
		timeoutLength()
	{};
	int Setup(Logger* logger, long timeoutSec);
	void Run();
	void Stop();
};

