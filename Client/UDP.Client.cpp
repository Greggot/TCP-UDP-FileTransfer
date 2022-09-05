#include "UDP.Client.hpp"
using namespace UDP;

#define RecordErrorAndReturn() { error = WSAGetLastError(); return; }

Client::Client(char ip[], char port[])
	: Socket()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa))
		RecordErrorAndReturn();
	target = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (target < 0)
		RecordErrorAndReturn();
	
	address.sin_family = AF_INET;
	stringToIP(ip, address);
	stringToPort(port, address);

	printf("Started client that will send data to UDP: %s:%s\n", ip, port);
}

void Client::Transmit(const void* data, const size_t size)
{
	memcpy(tx.data, data, size);
	if (sendto(target, (char*)&tx, size + sizeof(tx.sequenceNumber), 0, (sockaddr*)&address, sizeof(address)) < 0)
		RecordErrorAndReturn();
	++tx.sequenceNumber;
}

void Client::Transmit(size_t size)
{
	if (sendto(target, (char*)&tx, size + sizeof(tx.sequenceNumber), 0, (sockaddr*)&address, sizeof(address)) < 0)
		RecordErrorAndReturn();
}

void Client::CloseConnection()
{
	closesocket(target);
	WSACleanup();
}
