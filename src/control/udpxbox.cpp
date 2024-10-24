// Standard headers
#include <stdlib.h>
#include <string>
#include <cstdarg>
#include <cstring>

// Hardware headers
#include <pico/time.h>

#include "control/udpxbox.h"
#include "config/options.h"

UDPXbox::UDPXbox() : inputs({}), lastInputPacketTime(0), socket(new UdpSocket(Config::Control::XBOX_UDP_PORT))
{
    socket->callbackArgs = this;
    socket->receiveCallback = [](UdpSocket *socket, Datagram *datagram, void *args)
    {
        UDPXbox *xbox = (UDPXbox *)args;
        xbox->inputs.deserialize((uint8_t *)datagram->data, datagram->length);
        xbox->lastInputPacketTime = get_absolute_time();
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

bool UDPXbox::isConnected()
{
    return absolute_time_diff_us(lastInputPacketTime, get_absolute_time()) <= MAX_PACKET_INTERVAL_US;
}