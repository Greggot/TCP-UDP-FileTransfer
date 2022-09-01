#pragma once
#include "TCP.UDP.Common.hpp"

namespace UDP
{
	static const size_t BufferSize = 1024;
	struct Buffer
	{
		uint16_t sequenceNumber = 0;
		uint8_t data[BufferSize - sizeof(sequenceNumber)]{ 0 };
	};
}