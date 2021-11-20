#include "Logger.h"
#include "Core.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <thread>

bool runServer = true;

int main()
{
	Logger* logger = new Logger();
	Core core = Core();

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
