// Standard headers
#include <stdlib.h>
#include <string>
#include <cstdarg>
#include <cstring>
#include <algorithm>

// Hardware headers
#include <pico/time.h>

#include "control/driverstation.h"
#include "config/options.h"

#include <msgpack/msgpack.hpp>

using namespace std::literals;

int64_t TimeoutAlarmCallback(alarm_id_t id, void *user_data)
{
    Driverstation *ds = (Driverstation *)user_data;

    // https://stackoverflow.com/a/32780139
    auto it = std::find_if(std::begin(ds->clients), std::end(ds->clients),
                           [&id](auto &&p)
                           { return p.second.pongTimeoutAlarm == id; });

    if (it != std::end(ds->clients))
    {
        ds->server->disconnectClient(it->first);
    }

    return 0;
}

int64_t PingIntervalCallback(alarm_id_t id, void *user_data)
{
    Driverstation *ds = (Driverstation *)user_data;

    // https://stackoverflow.com/a/32780139
    auto it = std::find_if(std::begin(ds->clients), std::end(ds->clients),
                           [&id](auto &&p)
                           { return p.second.pingIntervalAlarm == id; });

    if (it != std::end(ds->clients))
    {
        ds->ping(it->first);
    }

    return 0;
}

Driverstation::Driverstation() : clients({}), server(new WsServer(Config::Control::DRIVERSTATION_PORT))
{
    server->callbackArgs = this;

    server->protocolCallback = [](const std::vector<std::string> &requestedProtocols, void *args) -> std::string_view
    {
        if (std::find(requestedProtocols.begin(), requestedProtocols.end(), Config::Control::DRIVERSTATION_PROTOCOL) != requestedProtocols.end())
            return Config::Control::DRIVERSTATION_PROTOCOL;
        else
            return ""sv;
    };

    server->clientConnected.Add([](WsServer *server, const WsServer::ClientEntry *entry, void *args)
                                { Driverstation *ds = (Driverstation *)args;
                                    ds->ping(entry->guid); });

    server->clientDisconnected.Add([](WsServer *server, const Guid &guid, WebSocketStatusCode statusCode, const std::string_view &reason, void *args)
                                   {
                                        Driverstation *ds = (Driverstation *)args;
                                        cancel_alarm(ds->clients[guid].pongTimeoutAlarm);
                                        ds->clients.erase(guid); });

    server->pongCallback = [](WsServer *server, const Guid &guid, const uint8_t *payload, size_t payloadLength, void *args)
    {
        Driverstation *ds = (Driverstation *)args;
        cancel_alarm(ds->clients[guid].pongTimeoutAlarm);
        ds->clients[guid] = {0, add_alarm_in_ms(Config::Control::DRIVERSTATION_TIMEOUT_MS, PingIntervalCallback, ds, true)};
    };

    server->messageReceived.Add([](WsServer *server, const Guid &guid, const WebSocketFrame &frame, void *args)
                                {
                                    if (!frame.isFragment)
                                    {
                                        switch (frame.opcode)
                                        {
                                        case WebSocketOpCode::TextFrame:
                                        {
                                            server->send(guid, "Text frames are not supported by this protocol."sv);
                                            break;
                                        }
                                        case WebSocketOpCode::BinaryFrame:
                                        {
                                            Driverstation *ds = (Driverstation *)args; 
                                            ds->handleFrame(guid, frame);
                                            break;
                                        }
                                        default:
                                            break;
                                    }
                                } });

    server->start();
    server->startDispatchQueue();
}

Driverstation::~Driverstation()
{
    server->~WsServer();
}

void Driverstation::ping(const Guid &guid)
{
    server->ping(guid);
    auto pongTimeout = add_alarm_in_ms(Config::Control::DRIVERSTATION_TIMEOUT_MS, TimeoutAlarmCallback, this, true);
    clients[guid] = {pongTimeout, clients[guid].pingIntervalAlarm};
}

void Driverstation::handleFrame(const Guid &guid, const WebSocketFrame &frame)
{
    if (frame.payloadLength > 0)
    {
        PacketType packetType = (PacketType)frame.payload[0];
        switch (packetType)
        {
        case PacketType::ClockSync:
        {
            std::error_code ec{};
            auto packet = msgpack::unpack<ClockSyncRequestPacket>(&frame.payload[1], frame.payloadLength - 1, ec);

            if (ec)
            {
                server->send(guid, "Error unpacking: "s + ec.message());
                break;
            }

            ClockSyncPacket response = {packet.clientTime, get_absolute_time()}; // populate response with client and current time

            auto data = msgpack::pack(response);
            data.emplace(data.begin(), (uint8_t)PacketType::ClockSync);
            server->send(guid, data);
            break;
        }
        case PacketType::RobotProperties:
        {
            auto data = msgpack::pack(Config::ROBOT_PROPERTIES);
            data.emplace(data.begin(), (uint8_t)PacketType::RobotProperties);
            server->send(guid, data);
            break;
        }
        default:
            server->send(guid, "Unsupported frame received."sv);
            break;
        }
    }
}