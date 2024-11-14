#ifndef _DRIVER_STATION_H
#define _DRIVER_STATION_H

#include <wsserver.h>
#include <unordered_map>
#include <pico/stdlib.h>
#include <pico/time.h>

enum class PacketType : uint8_t
{
    ClockSync,
    RobotProperties
};

struct ClockSyncRequestPacket
{
    uint64_t clientTime;

    template <class T>
    void pack(T &pack)
    {
        pack(clientTime);
    }
};

struct ClockSyncPacket
{
    uint64_t clientTime;
    uint64_t serverTime;

    template <class T>
    void pack(T &pack) const
    {
        pack(clientTime, serverTime);
    }
};

class Driverstation
{
public:
    Driverstation();
    ~Driverstation();

    struct ClientData
    {
        alarm_id_t pongTimeoutAlarm;
        alarm_id_t pingIntervalAlarm;
    };

    std::unordered_map<Guid, ClientData> clients;
    WsServer *server;

    void ping(const Guid &guid);
    void handleFrame(const Guid &guid, const WebSocketFrame &frame);

private:
};

#endif