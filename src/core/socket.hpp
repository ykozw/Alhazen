#pragma once
#include "pch.hpp"
#include "core/criticalsection.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class SocketData
{
public:
    SocketData();
    void clear();
    void makeHeader();
    void pushUint8(uint8_t d);
    void pushUint16(uint16_t d);
    void pushUint32(uint32_t d);
public:
    std::vector<uint8_t> data_;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
class Socket
{
public:
    Socket();
    ~Socket();
    bool connect(const std::string& address, int32_t portNo);
    void disconnect();
    bool send(const char* data, size_t dataSizeInBytes);
    bool send(SocketData& sd);
    bool receive(std::vector<char>& data, int32_t tmpBufferSize = 2048);
private:
    std::string address_ = "";
    int32_t portNo_ = 0;
    WSADATA wsaData_ = {};
    SOCKET socket_ = {};
    sockaddr_in	serverAddr_ = {};
    bool isConnected_ = false;
    CriticalSection cs_;
};
