#ifndef _UDP_XBOX_H
#define _UDP_XBOX_H

#include <udpsocket.h>
#include <math/units.h>
#include <packets/control/xbox.h>

class UDPXbox
{
public:
    UDPXbox();
    ~UDPXbox();

    Units<float> getForward();
    Units<float> getRotation();

    bool isConnected();

    Control::Xbox inputs;
    absolute_time_t lastInputPacketTime;

    static constexpr int64_t MAX_PACKET_INTERVAL_US = 100 /* ms */ * 1000 /* ms to us */;

private:
    UdpSocket *socket;
};

#endif