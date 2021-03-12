#define USE_NETWORKING 1
#if USE_NETWORKING

#include "packet.h"
#include "session.h"
#include "util/log.h"

EPACKET_OPCODE CPacketData::PacketOpcode()
{
    if (data.size() < sizeof(EPACKET_OPCODE))
    {
        LOG_FATAL("For some reason the message buffer is soo small, "
        "we can't even get a simple opcode. What idiot messed up... oh wait me.");
        exit(EXIT_FAILURE);
    }
    
    return static_cast<EPACKET_OPCODE>(*data.data()); 
}

uint32_t CPacketData::PacketSize()
{
    if (data.size() < sizeof(EPACKET_OPCODE) + sizeof(uint32_t))
    {
        LOG_FATAL("For some reason the message buffer is soo small, "
        "we can't even get a simple size. What idiot messed up... oh wait me.");
        exit(EXIT_FAILURE);
    }
    
    return static_cast<uint32_t>(*data.data()); 
}

void CPacketData::HandleData(CSession* session, size_t readAmount)
{
    dataPos += readAmount;

    if (dataPos < sizeof(EPACKET_OPCODE) + sizeof(uint32_t))
        return;

    if (dataPos < PacketSize())
        return;

    if (data.size() < sizeof(EPACKET_OPCODE) + sizeof(uint32_t) + PacketSize())
    {
        data.resize(data.size() + 
            (sizeof(EPACKET_OPCODE) + sizeof(uint32_t) + PacketSize() - data.size()) +
            0x10);
        return;
    }

    switch (PacketOpcode())
    {
    case MSG_ERROR:
        LOG_ERROR("Error handling packet, check that the correct data is being sent.");
        CleanPacketFromBuffer();
        break;

    case SMSG_HELLO:
    {
        uint32_t helloOpcode[] = {CMSG_HELLO, 0};
        session->GetSocket()->write_some(buffer(&helloOpcode, sizeof(helloOpcode)));
        CleanPacketFromBuffer();
    } break;
    
    default:
        LOG_ERROR("Error handling packet, unknown opcode, closing connection");
        session->CloseSocket();
        break;
    }
}

void CPacketData::CleanPacketFromBuffer()
{
    uint32_t size = sizeof(EPACKET_OPCODE) + sizeof(uint32_t) + PacketSize();
    if (size > data.size())
    {
        LOG_ERROR("Tried to shift data over, further than the data.");
        return;
    }

    memcpy(&data[0], &data[size], data.size() - size);
    memset(&data[data.size() - size], 0x0, size);

    dataPos -= size;
}

#endif