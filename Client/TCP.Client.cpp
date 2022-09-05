#include "TCP.Client.hpp"
using namespace TCP;

Client::Client(char ip[], char port[])
    : Socket()
{
    WSADATA wsaData;
    if (WSAStartup(0x101, &wsaData))
        RecordErrorAndReturn();

    address.sin_family = AF_INET;
    stringToIP(ip, address);
    stringToPort(port, address);

    target = socket(AF_INET, SOCK_STREAM, 0);
    if (target == INVALID_SOCKET)
        RecordErrorAndReturn();

    if (connect(target, (sockaddr*)&address, sizeof(address)) < 0)
        RecordErrorAndReturn();

    printf("Connected to TCP: %s:%s\n", ip, port);

    isRunning = true;
    receiveThread = std::thread([this]() {
        int length = 0;
        while (isRunning)
        {
            length = recv(target, (char*)&rx, sizeof(rx), 0);
            if (rx.length + EmptyMessageSize > length)
                continue;

            if (length < 0)
                RecordErrorAndReturn();

            if (rx.service < service::Amount && callbacks[rx.service])
                callbacks[rx.service](rx);
            else
                printf("  Unknown service\n");
        }
    });
}

void Client::Transmit(const service serv)
{
    tx.length = 0;
    tx.service = serv;
    if (send(target, (char*)&tx, EmptyMessageSize, 0))
        RecordErrorAndReturn();

    memset(&tx, 0, EmptyMessageSize);
}

void Client::Transmit(const service serv, void* data, int16_t size)
{
    tx.service = serv;
    tx.length = size < ArgumentSize ? size : ArgumentSize;
    memcpy(tx.argument, data, tx.length);
    if (send(target, (char*)&tx, EmptyMessageSize + tx.length, 0))
        RecordErrorAndReturn();
    
    memset(&tx, 0, EmptyMessageSize + tx.length);
}

void Client::CloseConnection()
{
    isRunning = false;
    receiveThread.join();
    closesocket(target);
    WSACleanup();
}

Client::~Client()
{
    CloseConnection();
}