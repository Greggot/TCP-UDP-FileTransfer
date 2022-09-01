#pragma once
#include <cstdio>

#include "UDP.Types.hpp"

namespace UDP
{
	class Client
	{
	private:
		sockaddr_in server;
		int sock, error;
		Buffer tx;

	public:
		Client(char ip[], char port[]);

		template<class type>
		void Transmit(const type& data, const size_t size = sizeof(type)) 
		{
			if (sendto(sock, (char*)&data, size, 0, (sockaddr*)&server, sizeof(server)) < 0)
				RecordErrorAndReturn();
		}
		void Transmit(const void* data, const size_t size);
		void Transmit(size_t size);
		Buffer& getBuffer() { return tx; }

		void CloseConnection();
	};
}