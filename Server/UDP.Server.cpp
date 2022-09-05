#include "UDP.Server.hpp"
using namespace UDP;

Server::Server(char ip[], char port[])
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa))
		RecordErrorAndReturn();

	if ((target = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
		RecordErrorAndReturn();
	
	address.sin_family = AF_INET;
	stringToIP(ip, address);
	stringToPort(port, address);

	if (bind(target, (sockaddr*)&address, sizeof(address)))
		RecordErrorAndReturn();
	printf("UDP accepting on port: %s\n", port);
}

void Server::AcceptConnection()
{
	isRunning = true;
	receiveThread = std::thread([this]() {
		int length = 0;
		while (isRunning)
		{
			length = recvfrom(target, (char*)&rx, sizeof(rx), 0, nullptr, nullptr);
			if(length < 0)
				RecordErrorAndReturn();

			if (callback)
				callback(rx, length - sizeof(rx.sequenceNumber));
		}
	});
	receiveThread.detach();
}

void Server::CloseConnection()
{
	isRunning = false;
	closesocket(target);
	WSACleanup();
}