#include "Logger.h"
#include "ServerCore.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <thread>

bool runServer = true;

int main()
{
	Logger* logger = new Logger();
	ServerCore core = ServerCore();

	core.Setup(logger);
	core.Run();

	std::string cmd;
	
	while(runServer)
	{
		std::getline(std::cin, cmd);
		if (cmd == "stop")
		{
			core.Stop();
			runServer = false;
		}
	}
	//printThread.join();
	//delete queue;
	return 0;
}
