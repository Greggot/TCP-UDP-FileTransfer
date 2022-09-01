#include "UDP.Client.hpp"
using namespace UDP;

#define RecordErrorAndReturn() { error = WSAGetLastError(); return; }

Client::Client(char ip[], char port[])
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa))
		RecordErrorAndReturn();
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock < 0)
		RecordErrorAndReturn();

	server.sin_family = AF_INET;
	stringToIP(ip, server);
	stringToPort(port, server);

	printf("Started client that will send data to UDP: %s:%s\n", ip, port);
}

void Client::Transmit(const void* data, const size_t size)
{
	memcpy(tx.data, data, size);
	if (sendto(sock, (char*)&tx, size + sizeof(tx.sequenceNumber), 0, (sockaddr*)&server, sizeof(server)) < 0)
		RecordErrorAndReturn();
	++tx.sequenceNumber;
}

void Client::Transmit(size_t size)
{
	if (sendto(sock, (char*)&tx, size + sizeof(tx.sequenceNumber), 0, (sockaddr*)&server, sizeof(server)) < 0)
		RecordErrorAndReturn();
}

void Client::CloseConnection()
{
	closesocket(sock);
	WSACleanup();
}
