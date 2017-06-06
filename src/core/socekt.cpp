#include "pch.hpp"
#include "core/socket.hpp"
#include "core/logging.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
SocketData::SocketData()
{
    clear();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void SocketData::clear()
{
    // ヘッダ分だけ確保
    data_.resize(4);
    data_[0] = 0x12;
    data_[1] = 0x34;
    data_[2] = 0;
    data_[3] = 0;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void SocketData::makeHeader()
{
    const size_t dataSize = data_.size();
    data_[0] = 0x12;
    data_[1] = 0x34;
    data_[2] = (dataSize >> 8) & 0xFF;
    data_[3] = (dataSize >> 0) & 0xFF;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void SocketData::pushUint8(uint8_t d)
{
    data_.push_back(d);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void SocketData::pushUint16(uint16_t d)
{
    data_.push_back((d >> 8) & 0xFF);
    data_.push_back(d & 0xFF);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void SocketData::pushUint32(uint32_t d)
{
    data_.push_back((d >> 24) & 0xFF);
    data_.push_back((d >> 16) & 0xFF);
    data_.push_back((d >> 8) & 0xFF);
    data_.push_back(d & 0xFF);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Socket::Socket()
{
    int err = WSAStartup(MAKEWORD(2, 2), &wsaData_);
    if (err)
    {
        printf("winsock init error! %d\n", err);
        exit(-1);
    }
    //
    
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Socket::~Socket()
{
    disconnect();
    WSACleanup();
    logging("Socket::~Socket()");
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool Socket::connect(const std::string& address, int32_t portNo)
{
    address_ = address;
    portNo_ = portNo;
    socket_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socket_ == INVALID_SOCKET)
    {
        isConnected_ = false;
        return false;
    }
    memset(&serverAddr_, 0, sizeof(serverAddr_));
    serverAddr_.sin_family = AF_INET;
    serverAddr_.sin_port = htons(portNo);
    inet_pton(AF_INET, address.c_str(), &serverAddr_.sin_addr);
    //
    if (::connect(socket_, (sockaddr*)&serverAddr_, sizeof(serverAddr_)) != 0)
    {
        isConnected_ = false;
        closesocket(socket_);
        return false;
    }
    isConnected_ = true;

    logging("Socket connectted.");
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void Socket::disconnect()
{
    if (isConnected_)
    {
        closesocket(socket_);
        isConnected_ = false;
    }
    logging("Socket::disconnect()");
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool Socket::send(const char* data, size_t dataSizeInBytes)
{
    if (!isConnected_)
    {
        logging("Try reconnect Socket");
        //
        if( !connect(address_, portNo_) )
        {
            return false;
        }
    }
    //
    cs_.enter();
    const int32_t dataSendInBytes = ::send(socket_, data, int32_t(dataSizeInBytes), 0);
    cs_.leave();
    // 送信に失敗している場合は接続先が切れている可能性がある。
    // そのまま接続を落とす
    if (dataSendInBytes < 0)
    {
        logging("Socket is disconected. Maybe server is lost.");
        disconnect();
        return false;
    }
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool Socket::send(SocketData& sd)
{
    sd.makeHeader();
    auto& d = sd.data_;
    return send((const char*)d.data(), d.size());
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool Socket::receive(std::vector<char>& data, int32_t tmpBufferSize)
{
    if (!isConnected_)
    {
        return false;
    }
    // 全てのバッファーを受け取る
    data.clear();
    std::vector<char> tmpBuffer;
    tmpBuffer.resize(tmpBufferSize);
    int recvInBytes;
    do
    {
        recvInBytes = ::recv(socket_, tmpBuffer.data(), int32_t(tmpBuffer.size()), 0);
        if (recvInBytes < 0)
        {
            return false;
        }
        data.insert(data.end(), tmpBuffer.begin(), tmpBuffer.begin() + recvInBytes);
    } while (recvInBytes == sizeof(tmpBuffer));
    return true;
}
