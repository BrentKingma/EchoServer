#include <iostream>
#include <string>
#include "ClientCore.h"

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "22556"
#define DEFAULT_IP "101.117.13.65"

int main()
{
	ClientCore core = ClientCore();

	std::string ip;
	std::string port;
	std::cout << "Please enter IP to echo server: " << std::endl;
	std::getline(std::cin, ip);
	std::cout << "Please enter port: " << std::endl;
	std::getline(std::cin, port);

	if (core.Setup(
		(ip == "") ? DEFAULT_IP : ip.c_str(),
		(port == "") ? DEFAULT_PORT : port.c_str()) != 0)
	{
		std::cout << "Application Could Not Connect";
		system("pause");
		return 1;
	}

	if (core.Run() != 0)
	{
		std::cout << "Application Could Not Run";
		system("pause");
		return 1;
	}

	if (core.Stop() != 0)
	{
		std::cout << "Application Could Not Stop";
		system("pause");
		return 1;
	}

	return 0;
}
