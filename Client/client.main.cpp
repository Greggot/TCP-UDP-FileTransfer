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

static inline void ReadFileSize(FILE* file, size_t& size)
{
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);
}

static inline void InitiateFileTransfer(TCP::Client& tcp, const char filename[], const char port[])
{
	tcp.Transmit(TCP::FileName, (void*)filename, strlen(filename));
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	tcp.Transmit(TCP::UDPstart, (void*)port, strlen(port));
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
		return 0;
	}

	static char* filename = getFileNamePtr(argv[FilePath], sizeof(argv[FilePath]));
	FILE* in = fopen(argv[FilePath], "rb");
	if (in == nullptr)
	{
		printf("Failed to open file.\n");
		return 1;
	}
	printf("Opened file \"%s\"\n", argv[FilePath]);
	
	TCP::Client tcp(argv[IP], argv[TCPport]);
	TryToReportErrorAndExit("Error appeared in TCP::Client", tcp);

	static std::condition_variable confirmation;
	static std::mutex mutex;
	static bool confirm = false;

	tcp.set(TCP::Confirmation, [](const TCP::Buffer&) {
		confirm = true;
		confirmation.notify_one();
	});

	std::thread([&tcp, &in, argv]() {
		size_t milliseconds = strtoul(argv[Timeout], NULL, 10);

		UDP::Client udp(argv[IP], argv[UDPport]);
		TryToReportErrorAndExit("Error appeared in UDP::Client", udp);

		UDP::Buffer& inbuffer = udp.getBuffer();
		inbuffer.sequenceNumber = 0;

		InitiateFileTransfer(tcp, filename, argv[UDPport]);
		
		size_t overallsize = 0;
		ReadFileSize(in, overallsize);
		size_t readsize = 0;
		size_t size = 1;
		while (size)
		{
			size = fread(inbuffer.data, sizeof(char), sizeof(inbuffer.data), in);
			std::unique_lock<std::mutex> lk(mutex);
			do
				udp.Transmit(size);
			while (!confirmation.wait_for(lk, std::chrono::milliseconds(milliseconds), [] { return confirm; }));
			confirm = false;
			++inbuffer.sequenceNumber;
			readsize += size;
			printf("\rProgress: %.2f", 100 * (float)readsize / overallsize);
		}

		tcp.Transmit(TCP::UDPclose);
		fclose(in);
	}).join();

	return 0;
}