#pragma once
#include <stdint.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define RecordErrorAndReturn() { error = WSAGetLastError(); return; }

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