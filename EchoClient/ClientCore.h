#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFFLEN 144
class ClientCore
{
private:
	WSADATA wsaData;
	struct addrinfo* result = NULL, * ptr = NULL, hints;
	int iResult;
	SOCKET connectSocket;
	std::string msg;

public:
	ClientCore() :
		connectSocket(INVALID_SOCKET),
		iResult(0),
		hints(),
		wsaData()
	{};

	int Setup(std::string ip, std::string port);

	int Run();

	int Stop();
};

