#include "ServerCore.h"
#include "Logger.h"

int ServerCore::Setup(Logger* a_logger, long timeoutSec)
{
	this->logger = a_logger;

	//Sets timeout variable
	timeoutLength.tv_sec = timeoutSec;
	timeoutLength.tv_usec = 0;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0)
	{
		msg = "WSAStartup failed: " + std::to_string(iResult);
		logger->enqueue(msg);
		return 1;
	}	

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		msg = "getaddrinfo failed: " + std::to_string(iResult);
		logger->enqueue(msg);
		WSACleanup();
		return 1;
	}

	listenSocket = INVALID_SOCKET;
	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (listenSocket == INVALID_SOCKET)
	{
		msg = "Error at socket(): " + std::to_string(WSAGetLastError());
		logger->enqueue(msg);
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		msg = "bind failed with error: " + std::to_string(WSAGetLastError());
		logger->enqueue(msg);
		freeaddrinfo(result);
		closesocket(listenSocket);
		WSACleanup();
	}

	freeaddrinfo(result);
}

void ServerCore::Run()
{
	listenThread = std::thread([this]() {
		std::string lclmsg;
		if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
		{
			lclmsg = "Listen failed with error: " + std::to_string(WSAGetLastError());
			logger->enqueue(msg);
			closesocket(listenSocket);
			WSACleanup();
		}

		//Creates and prepares set for select function
		fd_set listenSet;
		fd_set readSet;
		fd_set exceptSet;	

		while (runListenThread)
		{
			//Have to recall these functions for loop
			//Dont know why
			FD_ZERO(&listenSet);
			FD_ZERO(&readSet);
			FD_ZERO(&exceptSet);
			FD_SET(listenSocket, &listenSet);
			FD_SET(listenSocket, &readSet);

			if (select(0, &readSet, &listenSet, &exceptSet, &timeoutLength) == SOCKET_ERROR)
			{
				lclmsg = "Listen Thread failed in select" + std::to_string(WSAGetLastError());
				logger->enqueue(lclmsg);
				break;
			}
			if (FD_ISSET(listenSocket, &readSet))
			{
				// Accept a client socket
				SOCKET clientSocket = accept(listenSocket, NULL, NULL);
				if (clientSocket == INVALID_SOCKET)
				{
					lclmsg = "accept failed: " + std::to_string(WSAGetLastError());
					logger->enqueue(lclmsg);
				}
				else
				{
					lclmsg = "Client Connected";
					logger->enqueue(lclmsg);

					allThreads.push_back(new std::thread([this, clientSocket]() {
						while (true)
						{
							std::string lclmsg;
							int iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
							if (iResult > 0)
							{
								lclmsg = "Bytes received: " + std::to_string(iResult);
								logger->enqueue(lclmsg);

								// Echo the buffer back to the sender
								int iSendResult = send(clientSocket, recvbuf, iResult, 0);
								if (iSendResult == SOCKET_ERROR)
								{
									lclmsg = "Send failed: " + std::to_string(WSAGetLastError());
									logger->enqueue(lclmsg);
									break;
								}
								lclmsg = "Bytes sent: " + std::to_string(iSendResult);
								logger->enqueue(lclmsg);
								iResult = shutdown(clientSocket, SD_SEND);
								if (iResult == SOCKET_ERROR)
								{
									lclmsg = "Shutdown failed: " + std::to_string(WSAGetLastError());
									logger->enqueue(lclmsg);
									break;
								}
								break;
							}
							else if (iResult == 0)
							{
								lclmsg = "Connection closing...";
								logger->enqueue(lclmsg);
								break;
							}
							else
							{
								lclmsg = "recv failed: " + std::to_string(WSAGetLastError());
								logger->enqueue(lclmsg);
								break;
							}
						}
						closesocket(clientSocket);
						}));
				}
			}
			lclmsg = "Checking for connections";
			logger->enqueue(lclmsg);
		}	
		closesocket(listenSocket);
		WSACleanup();
	});
}

void ServerCore::Stop()
{
	runListenThread = false;
	closesocket(listenSocket);
	WSACleanup();
	listenThread.join();
	for (std::thread* th : allThreads)
	{
		th->join();
		delete th;
	}
	allThreads.clear();
}

