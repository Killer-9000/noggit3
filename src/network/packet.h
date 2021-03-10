#if USE_NETWORKING
#pragma once

#include <boost/asio.hpp>

using namespace boost::asio;

// MSG  : Bothways
// CMSG : From Client
// SMSG : From Server
enum EPACKET_OPCODE : uint32_t
{
    MSG_ERROR = 0,

    // Ping/Pong
    SMSG_HELLO,
    CMSG_HELLO,

    // Getting Data
    
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
    // if there isn't enough data, will return MSG_NOT_READY
    const EPACKET_OPCODE Opcode();

    // Handle the data if its ready.
    void HandleData(CSession* session, size_t readAmount);

    uint8_t* GetData() { return data.data() + dataPos; }
    size_t GetFreeSpace() { return data.size() - dataPos; }

private:
    // Shift everything left by n, and replace last n with 0
    void ShiftDataLeftByN(size_t n);

    std::vector<uint8_t> data;
    size_t dataPos;
};

#endif