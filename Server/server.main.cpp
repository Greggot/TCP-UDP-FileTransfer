#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "TCP.Server.hpp"
#include "UDP.Server.hpp"

#pragma warning(disable : 4996)

#include <filesystem>

int main(int argc, char* argv[])
{
    char ip[] = "192.168.1.134";
    char port[] = "6969";
    char tcpport[] = "1337";
    char savecatalog[] = "./udpreceive";

    static FILE* out;

    while (true)
    {
        UDP::Server udp(ip, port);
        TCP::Server tcp(ip, tcpport);
        udp.set([&tcp](const UDP::Buffer& request, size_t size) {
            outputLongBuffer("UDP receive", request.data, size);

            if (out)
                fwrite(request.data, sizeof(char), size, out);
            tcp.Transmit(TCP::Confirmation);
            });

        tcp.set(TCP::FileName, [savecatalog](const TCP::Buffer& request) {
            out = fopen((const char*)request.argument, "w");
            fclose(out);
            out = fopen((const char*)request.argument, "a");
            if (out)
                printf("Created file \"%s\"\n", request.argument);
            });

        tcp.set(TCP::UDPclose, [&udp, &tcp](const TCP::Buffer&) {
            udp.CloseConnection();
            fclose(out);
            printf("File closed! Server waiting one another connection...\n\n\n");
            tcp.CloseConnection();
            });

        tcp.set(TCP::UDPstart, [&udp](const TCP::Buffer&) {
            udp.AcceptConnection();
            });
        tcp.AcceptConnection();
    }

    return 0;
}