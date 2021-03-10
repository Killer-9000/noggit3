#if USE_NETWORKING

#include "packet.h"
#include "session.h"
#include "util/Log.h"

const EPACKET_OPCODE CPacketData::Opcode()
{
    if (data.size() < sizeof(EPACKET_OPCODE))
    {
        LOG_ERROR("For some reason the message buffer is soo small, we can't even get a simple opcode. What idiot messed up... oh wait me.");
        return static_cast<const EPACKET_OPCODE>(MSG_ERROR);
    }
    
    return static_cast<const EPACKET_OPCODE>(*data.data()); 
}

void CPacketData::HandleData(CSession* session, size_t readAmount)
{
    dataPos += readAmount;

    if (dataPos < 4)
        return;

    switch (Opcode())
    {
    case MSG_ERROR:
        LOG_ERROR("Error handling packet, check that the correct data is being sent.");
        ShiftDataLeftByN(sizeof(EPACKET_OPCODE));
        break;

    case SMSG_HELLO:
    {
        EPACKET_OPCODE hello = CMSG_HELLO;
        session->GetSocket()->write_some(buffer(&hello, sizeof(hello)));
        ShiftDataLeftByN(sizeof(EPACKET_OPCODE));
    } break;
    
    default:
        LOG_FATAL("Error handling packet, unknown opcode, closing connection");
        session->CloseSocket();
        break;
    }
}

void CPacketData::ShiftDataLeftByN(size_t n)
{
    if (n > data.size())
    {
        LOG_ERROR("Tried to shift data over, further than the data.");
        return;
    }

    memcpy(&data[0], &data[n], data.size());
    memset(&data[data.size() - n], 0x0, n);

    dataPos -= n;
}

#endif