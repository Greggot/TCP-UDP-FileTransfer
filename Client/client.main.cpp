#include "UDP.Client.hpp"
#include "TCP.Client.hpp"
#include <condition_variable>

#include <cstdio>

#pragma warning(disable : 4996)

// Works with relative and full paths
static inline char* getFileNamePtr(char* source, size_t size)
{
	while (--size)
		if (source[size] == '\\' || source[size] == '/')
			return &source[++size];
	return source;
}

enum Argument
{
	IP = 1, 
	TCPport,
	UDPport,
	FilePath,
	Timeout,

	Amount,
};

int main(int argc, char* argv[])
{
	if (argc != Amount)
	{
		printf("Usage: Client <ip> <tcp_port> <udp_port> <file_path> <udp_timeout_ms>\n");
		exit(0);
	}

	static char* filename = getFileNamePtr(argv[FilePath], sizeof(argv[FilePath]));
	size_t namelen = strlen(argv[FilePath]);
	if (filename == argv[FilePath])
		namelen = namelen - (filename - argv[FilePath]);

	UDP::Client udp(argv[IP], argv[UDPport]);
	TCP::Client tcp(argv[IP], argv[TCPport]);

	static std::condition_variable confirmation;
	static std::mutex mutex;
	static bool confirm = false;

	tcp.set(TCP::Confirmation, [](const TCP::Buffer&) {
		confirm = true;
		confirmation.notify_one();
	});

	std::thread([&udp, &tcp, argv, namelen]() {
		
		size_t milliseconds = strtoul(argv[Timeout], NULL, 10);
		
		FILE* in = fopen(argv[FilePath], "r");
		if (in == nullptr)
		{
			printf("Failed to open file.\n");
			exit(1);
		}
		printf("Opened file \"%s\"\n", argv[FilePath]);

		UDP::Buffer& inbuffer = udp.getBuffer();
		inbuffer.sequenceNumber = 0;

		tcp.Transmit(TCP::FileName, (void*)filename, namelen);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		tcp.Transmit(TCP::UDPstart, (void*)argv[UDPport], strlen(argv[UDPport]));

		size_t size = 1;
		while (size)
		{
			size = fread(inbuffer.data, sizeof(char), sizeof(inbuffer.data), in);
			std::unique_lock<std::mutex> lk(mutex);
			do
			{
				outputLongBuffer("UDP transmit", inbuffer.data, size);
				udp.Transmit(size);
			}
			while (!confirmation.wait_for(lk, std::chrono::milliseconds(milliseconds), [] { return confirm; }));
			confirm = false;
			++inbuffer.sequenceNumber;
		}

		tcp.Transmit(TCP::UDPclose);
		fclose(in);
	}).join();

	system("pause");

	return 0;
}