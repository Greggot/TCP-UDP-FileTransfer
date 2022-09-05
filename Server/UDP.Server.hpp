#pragma once
#include <cstdint>
#include <cstdio>
#include <functional>
#include <thread>

#include "UDP.Types.hpp"

namespace UDP
{
	class Server : public Socket
	{
	private:
		Buffer rx;

		std::thread receiveThread;
		receiveCallback callback = nullptr;
	public:
		Server(char ip[], char port[]);
		Server(){};

		virtual void AcceptConnection() final;
		virtual void CloseConnection() final;
		
		void set(receiveCallback call) { callback = call; }
	};
}