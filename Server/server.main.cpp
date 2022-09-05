#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "TCP.Server.hpp"
#include "UDP.Server.hpp"

#pragma warning(disable : 4996)

#include <filesystem>


enum Argument
{
    IP = 1,
    Port,
    Folder,

    Amount,
};

#define Kilobyte 1024

static inline FILE* clearFileAndOpen(const char path[])
{
    FILE* file = fopen(path, "w");
    if (file)
        fclose(file);
    file = fopen(path, "ab");
    return file;
}

int main(int argc, char* argv[])
{
    if (argc != Amount)
    {
        printf("Usage: Server <ip> <port> <save_catalog>\n");
        exit(0);
    }

    if (!std::filesystem::exists(argv[Folder]))
        std::filesystem::create_directory(argv[Folder]);

    while (true)
    {
        static FILE* out;
        TCP::Server tcp(argv[IP], argv[Port]);
        TryToReportErrorAndExit("Error appeared in TCP::Server", tcp);
        UDP::Server udp;
        size_t readsize = 0;

        static const auto updreceive = [&readsize, &tcp](const UDP::Buffer& request, size_t size) {
            readsize += size;
            printf("\r Received %.2f KB", (float)readsize / Kilobyte);

            if (out)
                fwrite(request.data, sizeof(char), size, out);
            tcp.Transmit(TCP::Confirmation);
        };

        tcp.set(TCP::FileName, [argv](const TCP::Buffer& request) {
            std::string filepath = std::string(argv[Folder]) + "/" + std::string((char*)request.argument);
            out = clearFileAndOpen(filepath.c_str());
            if (out)
                printf("Created file \"%s\"\n", request.argument);
        });

        tcp.set(TCP::UDPclose, [&udp, &tcp](const TCP::Buffer&) {
            udp.CloseConnection();
            tcp.CloseConnection();
            fclose(out);
            printf("\nFile closed! Server waiting one another connection...\n");
        });

        tcp.set(TCP::UDPstart, [&udp, argv](const TCP::Buffer& request) {
            udp = UDP::Server(argv[IP], (char*)request.argument);
            TryToReportErrorAndExit("Error appeared in UDP::Server", udp);
            udp.set(updreceive);
            udp.AcceptConnection();
        });

        tcp.AcceptConnection();
        
        int error = tcp.getLastError();
        if (error)
        {
            printf("\nConnection terminated: %i\n", error);
            udp.CloseConnection();
        }
        printf("\n\n");
    }

    return 0;
}