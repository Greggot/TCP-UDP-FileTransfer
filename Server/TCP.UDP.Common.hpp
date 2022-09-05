#pragma once
#include <stdint.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

inline void stringToIP(char* string, sockaddr_in& sockaddr)
{
	sockaddr.sin_addr.s_addr = 0;
	for (uint8_t i = 0; i < sizeof(sockaddr.sin_addr.s_addr); ++i, ++string)
	{
		sockaddr.sin_addr.s_addr >>= 8;
		sockaddr.sin_addr.s_addr |= strtoul(string, &string, 10) << 24;
	}
}

inline void stringToPort(char* string, sockaddr_in& sockaddr)
{
	sockaddr.sin_port = htons(strtoul(string, NULL, 10));
}

inline void outputLongBuffer(const char description[], const void* data, size_t size, size_t maxout = 10)
{
	maxout = min(maxout, size);
	printf("%s: ", description);
	for (size_t i = 0; i < maxout; ++i)
		printf("%02X ", ((uint8_t*)data)[i]);
	
	size -= maxout;
	if (size)
		printf("... and %zu butes more", size);
	
	printf("\n");
}

#define RecordErrorAndReturn() { error = WSAGetLastError(); return; }
class Socket
{
protected:
	int error = 0;
	SOCKET me, target;
	bool isRunning = false;
	sockaddr_in address = { 0 };

	virtual void AcceptConnection() {};
	virtual void CloseConnection() {};
public:
	Socket() : me(0), target(0) {}
	int getLastError() const { return error; }
};

inline void TryToReportErrorAndExit(const char description[], const Socket& socket)
{
	int error = socket.getLastError();
	if (error == 0)
		return;
	printf("%s - %i\n", description, error);
	exit(error);
}