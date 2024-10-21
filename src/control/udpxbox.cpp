// Standard headers
#include <stdlib.h>
#include <string>
#include <cstdarg>
#include <cstring>

#include "control/udpxbox.h"
#include "config/options.h"

UDPXbox::UDPXbox() : inputs({}), socket(new UdpSocket(Config::Control::XBOX_UDP_PORT))
{
    socket->callbackArgs = this;
    socket->receiveCallback = [](UdpSocket *socket, Datagram *datagram, void *args)
    {
        UDPXbox *xbox = (UDPXbox *)args;
        xbox->inputs.fromBytes((uint8_t *)datagram->data, datagram->length);
    };
}

UDPXbox::~UDPXbox()
{
    socket->deinit();
    socket->~UdpSocket();
}

Units<float> UDPXbox::getForward()
{
    return Units<float>::meters(Control::Xbox::getAxis(inputs.axis_Y));
}

Units<float> UDPXbox::getRotation()
{
    return Units<float>::radians(Control::Xbox::getAxis(inputs.axis_X) * 10);
}