#include "Logger.h"
#include "ServerCore.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <thread>

#pragma once

bool runServer = true;

int main()
{
	Logger* logger = new Logger();
	ServerCore core = ServerCore();

	if (core.Setup(logger, 5) != 0) return 1;
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
	delete logger;
	return 0;
}
