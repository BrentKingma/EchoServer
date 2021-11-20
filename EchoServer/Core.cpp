#include "Core.h"
#include "Logger.h"

int Core::Setup(Logger* logger)
{
	this->logger = logger;
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
		//queueRef->enqueue(msg);
		WSACleanup();
		return 1;
	}

	listenSocket = INVALID_SOCKET;
	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (listenSocket == INVALID_SOCKET)
	{
		msg = "Error at socket(): " + std::to_string(WSAGetLastError());
		//queueRef->enqueue(msg);
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		msg = "bind failed with error: " + std::to_string(WSAGetLastError());
		//queueRef->enqueue(msg);
		freeaddrinfo(result);
		closesocket(listenSocket);
		WSACleanup();
	}

	freeaddrinfo(result);
}

void Core::Run()
{
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		msg = "Listen failed with error: " + std::to_string(WSAGetLastError());
		//queueRef->enqueue(msg);
		closesocket(listenSocket);
		WSACleanup();
	}

	serverThread = std::thread([this]() {
		SOCKET clientSocket;

		clientSocket = INVALID_SOCKET;
		char recvbuf[DEFAULT_BUFLEN];
		int iSendResult;
		int recvbuflen = DEFAULT_BUFLEN;

		while (runServer)
		{
			// Accept a client socket
			clientSocket = accept(listenSocket, NULL, NULL);
			if (clientSocket == INVALID_SOCKET)
			{
				msg = "accept failed: " + std::to_string(WSAGetLastError());
				//queueRef->enqueue(msg);
				closesocket(listenSocket);
				WSACleanup();
			}

			iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
			if (iResult > 0)
			{
				msg = "Bytes received: " + std::to_string(iResult);
				logger->enqueue(msg);

				// Echo the buffer back to the sender
				iSendResult = send(clientSocket, recvbuf, iResult, 0);
				if (iSendResult == SOCKET_ERROR)
				{
					msg = "Send failed: " + std::to_string(WSAGetLastError());
					//queueRef->enqueue(msg);
					closesocket(clientSocket);
					WSACleanup();
				}
				msg = "Bytes sent: " + std::to_string(iSendResult);
				//queueRef->enqueue(msg);
				iResult = shutdown(clientSocket, SD_SEND);
				if (iResult == SOCKET_ERROR)
				{
					msg = "Shutdown failed: " + std::to_string(WSAGetLastError());
					//queueRef->enqueue(msg);
					closesocket(clientSocket);
					WSACleanup();
				}

				// cleanup
				closesocket(clientSocket);
				//WSACleanup();
			}
			else if (iResult == 0)
			{
				msg = "Connection closing...";
				//queueRef->enqueue(msg);
			}
			else
			{
				msg = "recv failed: " + std::to_string(WSAGetLastError());
				//queueRef->enqueue(msg);
				closesocket(clientSocket);
				WSACleanup();
			}
		}
		});
}

void Core::Stop()
{
	runServer = false;
	// No longer need server socket
	closesocket(listenSocket);
	WSACleanup();
	serverThread.join();
}
