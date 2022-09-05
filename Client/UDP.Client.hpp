#pragma once
#include <cstdio>

#include "UDP.Types.hpp"

namespace UDP
{
	class Client : public Socket
	{
	private:
		Buffer tx;

	public:
		Client(char ip[], char port[]);

		template<class type>
		void Transmit(const type& data, const size_t size = sizeof(type)) 
		{
			if (sendto(target, (char*)&data, size, 0, nullptr, nullptr) < 0)
				RecordErrorAndReturn();
		}
		void Transmit(const void* data, const size_t size);
		void Transmit(size_t size);
		Buffer& getBuffer() { return tx; }

		virtual void CloseConnection() final;
	};
}