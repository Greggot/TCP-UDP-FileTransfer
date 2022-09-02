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


int main(int argc, char* argv[])
{
    if (argc != Amount)
    {
        printf("Usage: Server <ip> <port> <save_catalog>\n");
        exit(0);
    }

    if (!std::filesystem::exists(argv[Folder]))
        std::filesystem::create_directory(argv[Folder]);

    static FILE* out;

    while (true)
    {
        TCP::Server tcp(argv[IP], argv[Port]);
        UDP::Server udp;

        static const auto updreceive = [&tcp](const UDP::Buffer& request, size_t size) {
            printf("(%u)", request.sequenceNumber);
            outputLongBuffer("UDP receive", request.data, size);

            if (out)
                fwrite(request.data, sizeof(char), size, out);
            tcp.Transmit(TCP::Confirmation);
            };

        tcp.set(TCP::FileName, [argv](const TCP::Buffer& request) {
            std::string filepath = std::string(argv[Folder]) + "/" + std::string((char*)request.argument);
            
            out = fopen(filepath.c_str(), "w");
            fclose(out);
            out = fopen(filepath.c_str(), "a");
            if (out)
                printf("Created file \"%s\"\n", request.argument);
            });

        tcp.set(TCP::UDPclose, [&udp, &tcp](const TCP::Buffer&) {
            udp.CloseConnection();
            fclose(out);
            printf("File closed! Server waiting one another connection...\n\n\n");
            tcp.CloseConnection();
            });

        tcp.set(TCP::UDPstart, [&udp, argv](const TCP::Buffer& request) {
            udp = UDP::Server(argv[IP], (char*)request.argument);
            udp.set(updreceive);
            udp.AcceptConnection();
            });
        tcp.AcceptConnection();
    }

    return 0;
}