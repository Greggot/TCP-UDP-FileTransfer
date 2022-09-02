#include "TCP.Server.hpp"
using namespace TCP;


Server::Server(char ip[], char port[])
{
    WSADATA wsaData;
    if (WSAStartup(0x101, &wsaData))
        RecordErrorAndReturn();

    sockaddr_in local = {0};
    local.sin_family = AF_INET;
    stringToIP(ip, local);
    stringToPort(port, local);

    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == INVALID_SOCKET)
        RecordErrorAndReturn();

    if (bind(server, (sockaddr*)&local, sizeof(local)))
        RecordErrorAndReturn();
    
    if (listen(server, 1))
        RecordErrorAndReturn();

    printf("Server listening on %s:%s\n", ip, port);
}

void Server::AcceptConnection()
{
    client = accept(server, NULL, NULL);
    if (client < 0)
        return;
    printf("Client connected - %lli\n", client);

    isRunning = true;
    receiveThread = std::thread([this]() {
        int length = 0;
        while (isRunning)
        {
            length = recv(client, (char*)&rx, sizeof(rx), 0);
            if (length < 0)
                RecordErrorAndReturn();
            if (rx.length + EmptyMessageSize > length)
                continue;

            if (rx.service < service::Amount && callbacks[rx.service])
                callbacks[rx.service](rx);
            else
                printf("  Unknown service\n");
            
            memset(&rx, 0, length);
        }
    });
    receiveThread.join();
}

void Server::CloseConnection()
{
    isRunning = false;
    closesocket(server);
    WSACleanup();
}

void Server::Transmit(void* data, int16_t size)
{
    tx.length = size < ArgumentSize ? size : ArgumentSize;
    memcpy(tx.argument, data, tx.length);
    if (send(client, (char*)&tx, EmptyMessageSize + tx.length, 0))
        RecordErrorAndReturn();
}

void Server::Transmit(const service serv)
{
    tx.length = 0;
    tx.service = serv;
    if (send(client, (char*)&tx, EmptyMessageSize, 0))
        RecordErrorAndReturn();
}

Server::~Server()
{
    CloseConnection();
}
