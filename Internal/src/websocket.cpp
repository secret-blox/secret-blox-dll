#include <thread>

#include "Internal/logger.hpp"
#include "Internal/websocket.hpp"
#include "Internal/scheduler.hpp"

#include "Security/xor.hpp"

#include "easywsclient.hpp" 
#ifdef _WIN32
#pragma comment( lib, "ws2_32" )
#include <WinSock2.h>
#endif

using OpCode = SB::Websocket::OpCode;
using easywsclient::WebSocket;
static WebSocket::pointer ws = NULL;

void handleMessage(const std::string& message)
{
    OpCode op = static_cast<OpCode>(message[0]);
    const auto plain = message.substr(1);
    switch (op)
    {
    case OpCode::WS_EXEC:
        SB::Scheduler::queueScript(plain);
        break;
    case OpCode::WS_MSG:
        SB::Logger::printf(XORSTR("Received message: %s\n"), plain.c_str());
        break;
    case OpCode::WS_SETFPS: // TODO
    case OpCode::WS_NOP:
    default:
        break;
    };
}

namespace SB::Websocket
{
    bool listening = false;
}

void SB::Websocket::socketLoop()
{
    do
    {
        if (!ws)
            ws = WebSocket::from_url(XORSTR("ws://localhost:49152"));
        if (!ws)
            continue;
        while (listening && ws->getReadyState() != WebSocket::CLOSED) {
            ws->poll();
            ws->dispatch(handleMessage);
        }
        delete ws;
    } while(listening);
}

void SB::Websocket::setup()
{
    listening = true;
    std::thread t(socketLoop);
    t.detach();
}

void SB::Websocket::unload()
{
    listening = false;
    ws = NULL;
}