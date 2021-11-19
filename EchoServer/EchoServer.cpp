#include "SafeQueue.h"
#include "Core.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <thread>

bool runServer = true;

int main()
{
	SafeQueue<std::string>* queue = new SafeQueue<std::string>();
	Core core = Core();

	core.Setup(queue);
	core.Run();

	std::string cmd;

	/*std::thread printThread = std::thread([queue]() {
		while (runServer)
		{
			if (!queue->isEmpty())
			{
				std::cout << queue->dequeue() << std::endl;
			}
		}
		});*/

	
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
