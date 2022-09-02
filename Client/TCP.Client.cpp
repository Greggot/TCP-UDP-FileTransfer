#include "TCP.Client.hpp"
using namespace TCP;

Client::Client(char ip[], char port[])
{
    WSADATA wsaData;
    if (WSAStartup(0x101, &wsaData))
        RecordErrorAndReturn();

    sockaddr_in target = { 0 };
    target.sin_family = AF_INET;
    stringToIP(ip, target);
    stringToPort(port, target);

    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == INVALID_SOCKET)
        RecordErrorAndReturn();

    if (connect(server, (sockaddr*)&target, sizeof(target)) < 0)
        RecordErrorAndReturn();

    printf("Connected to TCP: %s:%s\n", ip, port);

    isRunning = true;
    receiveThread = std::thread([this]() {
        int length = 0;
        while (isRunning)
        {
            length = recv(server, (char*)&rx, sizeof(rx), 0);
            if (rx.length + EmptyMessageSize > length)
                continue;

            if (length < 0)
                RecordErrorAndReturn();

            printf("TCP service(%u) status(%u)\n", rx.service, rx.status);

            if (rx.service < service::Amount && callbacks[rx.service])
                callbacks[rx.service](rx);
            else
                printf("  Unknown service\n");
        }
    });
    receiveThread.detach();
}

void Client::Transmit(const service serv)
{
    tx.length = 0;
    tx.service = serv;
    if (send(server, (char*)&tx, EmptyMessageSize, 0))
        RecordErrorAndReturn();

    memset(&tx, 0, EmptyMessageSize);
}

void Client::Transmit(const service serv, void* data, int16_t size)
{
    tx.service = serv;
    tx.length = size < ArgumentSize ? size : ArgumentSize;
    memcpy(tx.argument, data, tx.length);
    if (send(server, (char*)&tx, EmptyMessageSize + tx.length, 0))
        RecordErrorAndReturn();
    
    memset(&tx, 0, EmptyMessageSize + tx.length);
}

void Client::CloseConnection()
{
    isRunning = false;
    closesocket(server);
    WSACleanup();
}

Client::~Client()
{
}