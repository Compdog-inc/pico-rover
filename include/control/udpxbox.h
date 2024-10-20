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

    Control::Xbox inputs;

private:
    UdpSocket *socket;
};

#endif