#if USE_NETWORKING

#include "session.h"
#include "util/Log.h"
#include <string_view>

void CSession::StartSocket()
{    
    m_socket = new ip::tcp::socket(m_iocontext);
    // TODO: Get this from settings
    error_code error;
    m_socket->connect(ip::tcp::endpoint(
        ip::address::from_string("127.0.0.1"), 9292), error);

    if (error)
    {
        LOG_ERROR("Error writing packet, message: (%s), closing socket.", error.message());
        CloseSocket();
        return;
    }

    m_address = m_socket->remote_endpoint().address().to_string();
    m_port = m_socket->remote_endpoint().port();

    LOG_INFO("Started connection: %s:%i", GetAddress(), GetPort());
    m_closed = false;
}

void CSession::Update()
{
    m_readAmount = 0;
    error_code error;
    m_readAmount = m_socket->read_some(buffer(
        m_buffer.GetData(), 
        m_buffer.GetFreeSpace()), error);

    if (error)
    {
        LOG_ERROR("Error handling session data, message: (%s), closing socket.", error.message());
        CloseSocket();
        return;
    }

    if (m_readAmount > 0)
    {
        LOG_DEBUG("Got packet with opcode: (%i)", m_buffer.Opcode());
        m_buffer.HandleData(this, m_readAmount);
    }

    Update();
}

void CSession::CloseSocket()
{
    LOG_INFO("Closed connection");

    if (m_socket->is_open())
    {
        // Can do something here if we close the session, instead of just dropping.
        // Like notifying other sessions.
        m_socket->close();
    }

    m_closed = true;
    delete m_socket;
}

#endif