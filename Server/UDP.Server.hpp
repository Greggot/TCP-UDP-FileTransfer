#pragma once
#include <cstdint>
#include <cstdio>
#include <functional>
#include <thread>

#include "UDP.Types.hpp"

namespace UDP
{
	class Server
	{
	private:
		Buffer rx;

		sockaddr_in server, client;
		SOCKET sock;
		int error;
		std::thread receiveThread;
		bool isRunning = false;

		receiveCallback callback = nullptr;
	public:
		Server(char ip[], char port[]);

		void AcceptConnection();
		void CloseConnection();
		
		void set(receiveCallback call) { callback = call; }
		int getLastError() const { return error; }
	};
}