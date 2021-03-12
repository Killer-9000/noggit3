#pragma once
#if USE_NETWORKING

#include <boost/asio.hpp>

using namespace boost::asio;

// Packet structure
// EPACKET_OPCODE opcode
// uint32_t       dataSize
// uint8_t        data[dataSize]

// MSG  : Bothways
// CMSG : From Client
// SMSG : From Server
enum EPACKET_OPCODE : uint32_t
{
    MSG_ERROR = 0,

    /// Ping/Pong
    /// --------------
    SMSG_HELLO,
    CMSG_HELLO,

    /// Getting Data
    /// --------------
    CMSG_GET_MAP_INFO,
    SMSG_SEND_MAP_INFO,

    CMSG_CAN_OPEN_MAP,
    SMSG_CAN_OPEN_MAP,
};

class CSession;

class CPacketData
{
public:
    CPacketData() 
    {
        dataPos = 0;
        data.resize(0x20);
    }
    ~CPacketData() { }

    // Will return the opcode at the start of the data,
    // if there isn't enough data, will return MSG_ERROR
    EPACKET_OPCODE PacketOpcode();
    uint32_t PacketSize();

    // Handle the data if its ready.
    void HandleData(CSession* session, size_t readAmount);

    uint8_t* GetData() { return data.data() + dataPos; }
    size_t GetFreeSpace() { return data.size() - dataPos; }

private:
    // Shift everything left by n, and replace last n with 0
    void CleanPacketFromBuffer();

    std::vector<uint8_t> data;
    size_t dataPos;
};

#endif