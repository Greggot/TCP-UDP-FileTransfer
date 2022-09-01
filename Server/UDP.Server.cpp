#include "UDP.Server.hpp"
using namespace UDP;

Server::Server(char ip[], char port[])
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa))
		RecordErrorAndReturn();

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
		RecordErrorAndReturn();
	
	server.sin_family = AF_INET;
	stringToIP(ip, server);
	stringToPort(port, server);

	if (bind(sock, (sockaddr*)&server, sizeof(server)))
		RecordErrorAndReturn();
}

void Server::AcceptConnection()
{
	isRunning = true;
	receiveThread = std::thread([this]() {
		int length = 0;
		int fromlength = sizeof(client);
		while (isRunning)
		{
			length = recvfrom(sock, (char*)&rx, sizeof(rx), 0, (sockaddr*)&client, &fromlength);
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

	closesocket(sock);
	WSACleanup();
}