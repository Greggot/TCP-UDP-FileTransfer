#include "UDP.Client.hpp"
#include "TCP.Client.hpp"
#include <condition_variable>

#include <cstdio>

#pragma warning(disable : 4996)

int main(int argc, char* argv[])
{
	char ip[] = "192.168.1.134";
	char port[] = "6969";
	char tcpport[] = "1337";
	static char filename[] = "C:/Users/007/Desktop/VOCOM RD Tests/16.08.MotherDecompress.txt";

	UDP::Client udp(ip, port);
	TCP::Client tcp(ip, tcpport);

	static std::condition_variable confirmation;
	static std::mutex mutex;
	static bool confirm = false;


	tcp.set(TCP::Confirmation, [](const TCP::Buffer&) {
		confirm = true;
		confirmation.notify_one();
	});

	std::thread([&udp, &tcp]() {
		
		FILE* in = fopen(filename, "r");
		UDP::Buffer& inbuffer = udp.getBuffer();
		inbuffer.sequenceNumber = 0;

		char filename[] = "16.08.MotherDecompress.txt";
		tcp.Transmit(TCP::FileName, filename);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		tcp.Transmit(TCP::UDPstart);
		size_t size;
		do
		{
			size = fread(inbuffer.data, sizeof(char), sizeof(inbuffer.data), in);
			std::unique_lock<std::mutex> lk(mutex);
			do
			{
				outputLongBuffer("UDP transmit", inbuffer.data, size);
				udp.Transmit(size);
			}
			while (!confirmation.wait_for(lk, std::chrono::seconds(1), [] { return confirm; }));
			confirm = false;
			++inbuffer.sequenceNumber;
		} while (size);

		tcp.Transmit(TCP::UDPclose);
		fclose(in);
	}).join();

	system("pause");

	return 0;
}