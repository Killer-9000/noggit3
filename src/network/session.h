#pragma once
#if USE_NETWORKING

#include "packet.h"
#include <boost/asio.hpp>

using namespace boost::asio;
using namespace boost::system;

class CSession : public std::enable_shared_from_this<CSession>
{
    boost::asio::io_context m_iocontext;

    ip::tcp::socket* m_socket;
    CPacketData m_buffer;
    uint32_t m_readAmount;

    std::string m_address;
    uint16_t m_port;

    bool m_closed = true;

    CSession() { };
    ~CSession() 
    { 
        if (!m_closed)
            CloseSocket();
    }

public:
    static CSession& Instance()
    {
        static CSession inst;
        return inst;
    }

    void StartSocket();
    void Update();
    void CloseSocket();

    std::string GetAddress() { return m_address; }
    uint16_t GetPort() { return m_port; }
    ip::tcp::socket* GetSocket() { return m_socket; }
    bool IsClosed() { return m_closed; }

    typedef std::shared_ptr<CSession> Ptr;
};

#define sSession CSession::Instance()

#endif