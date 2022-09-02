#pragma once
#include <cstdio>
#include <thread>

#include "TCP.Types.hpp"

namespace TCP
{
	class Client
	{
	private:
		Buffer rx, tx;
		SOCKET server;
		int error;
		bool isRunning = false;

		std::thread receiveThread;
		receiveCallback callbacks[service::Amount]{ nullptr };
	public:
		Client(char ip[], char port[]);

		void CloseConnection();
		
		void Transmit(const service serv);
		void Transmit(const service serv, void* data, int16_t size);
		template<class type>
		void Transmit(const service serv, const type& data, int16_t size = sizeof(type) & 0x7FFF)
		{
			Transmit(serv, (void*) & data, size);
		}

		int getLastError() const { return error; }
		void set(service serv, receiveCallback call) { callbacks[serv] = call; }

		~Client();
	};
}