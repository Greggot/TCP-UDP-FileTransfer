#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <cstdio>
#include <thread>

#include "TCP.Types.hpp"

namespace TCP
{
	class Server : public Socket
	{
	private:
		Buffer rx, tx;

		std::thread receiveThread;
		receiveCallback callbacks[service::Amount]{nullptr};
	public:
		Server(char ip[], char port[]);
		/**
		* @brief Blocking call creates thread which listenes to socket and
		* handles callbacks for services.
		* Setup all callbacks before
		* Place connection close and error parse after.
		*/
		virtual void AcceptConnection() final;
		/**
		* @brief Finish AcceptConnection call, close socket
		*/
		virtual void CloseConnection() final;

		void Transmit(const service serv);
		void Transmit(void* data, int16_t size);
		template<class type>
		void Transmit(const service serv, const type& data, int16_t size = sizeof(type) & 0x7FFF)
		{
			tx.service = serv;
			Transmit(&data, size);
		}
		
		void set(service serv, receiveCallback call) { callbacks[serv] = call; }

		~Server();
	};
}