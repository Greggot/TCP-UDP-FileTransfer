#pragma once
#include <cstdio>
#include <thread>

#include "TCP.Types.hpp"

namespace TCP
{
	class Client : public Socket
	{
	private:
		Buffer rx, tx;

		std::thread receiveThread;
		receiveCallback callbacks[service::Amount]{ nullptr };
	public:
		Client(char ip[], char port[]);

		void Transmit(const service serv);
		void Transmit(const service serv, void* data, int16_t size);
		template<class type>
		void Transmit(const service serv, const type& data, int16_t size = sizeof(type) & 0x7FFF)
		{
			Transmit(serv, (void*) & data, size);
		}

		virtual void CloseConnection() final;
		void set(service serv, receiveCallback call) { callbacks[serv] = call; }

		~Client();
	};
}