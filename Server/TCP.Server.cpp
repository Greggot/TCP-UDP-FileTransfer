#include "TCP.Server.hpp"
using namespace TCP;


Server::Server(char ip[], char port[])
    : Socket()
{
    WSADATA wsaData;
    if (WSAStartup(0x101, &wsaData))
        RecordErrorAndReturn();

    address.sin_family = AF_INET;
    stringToIP(ip, address);
    stringToPort(port, address);

    me= socket(AF_INET, SOCK_STREAM, 0);
    if (me == INVALID_SOCKET)
        RecordErrorAndReturn();

    if (bind(me, (sockaddr*)&address, sizeof(address)))
        RecordErrorAndReturn();
    
    if (listen(me, 1))
        RecordErrorAndReturn();

    printf("Server listening on %s:%s\n", ip, port);
}

void Server::AcceptConnection()
{
    target = accept(me, NULL, NULL);
    if (target < 0)
        return;
    printf("Client connected - %lli\n", target);

    isRunning = true;
    receiveThread = std::thread([this]() {
        int length = 0;
        while (isRunning)
        {
            length = recv(target, (char*)&rx, sizeof(rx), 0);
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
    closesocket(me);
    WSACleanup();
}

void Server::Transmit(void* data, int16_t size)
{
    tx.length = size < ArgumentSize ? size : ArgumentSize;
    memcpy(tx.argument, data, tx.length);
    if (send(target, (char*)&tx, EmptyMessageSize + tx.length, 0))
        RecordErrorAndReturn();
}

void Server::Transmit(const service serv)
{
    tx.length = 0;
    tx.service = serv;
    if (send(target, (char*)&tx, EmptyMessageSize, 0))
        RecordErrorAndReturn();
}

Server::~Server()
{
    CloseConnection();
}
