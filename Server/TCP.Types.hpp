#pragma once
#include <stdint.h>
#include <functional>

#include "TCP.UDP.Common.hpp"

namespace TCP
{
	enum service : uint8_t
	{
		UDPstart,
		UDPclose,
		FileName,
		Confirmation,

		Amount,
	};

	static const size_t BufferSize = 32;
	static const size_t EmptyMessageSize = sizeof(int16_t) + sizeof(uint8_t) + sizeof(uint8_t);
	static const size_t ArgumentSize = BufferSize - EmptyMessageSize;
	struct Buffer
	{
		int16_t length = 0;
		uint8_t service = 0;
		uint8_t status = 0;
		uint8_t argument[BufferSize - EmptyMessageSize]{0};

		Buffer () {}
		Buffer (uint8_t service, uint8_t status = 0) : service(service), status(status) {}
	};

	typedef std::function<void(const TCP::Buffer&)> receiveCallback;
}