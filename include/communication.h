#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

#include <pico/stdlib.h>

enum class CommunicationCommand : uint8_t
{
    Null = 0x00,
    ReadStatus = 0x01,
    ReadDistanceSensors = 0x02
};

struct CommunicationStatus
{
    uint32_t version;
    bool running;
};

static constexpr uint CommunicationStatus_SIZE = 4 + 1;

struct CommunicationDistanceSensors
{
    float distance0;
    float distance1;
    float distance2;
    float distance3;
    float distance4;
    float distance5;
};

static constexpr uint CommunicationDistanceSensors_SIZE = 4 * 6;

class Communication
{
public:
    Communication(bool isMain);
    ~Communication();

    bool readStatus(CommunicationStatus *out_status);
    bool readDistanceSensors(CommunicationDistanceSensors *out_sensors);

    bool hasData();
    CommunicationCommand getCommand();

    bool writeStatus(const CommunicationStatus &status);
    bool writeDistanceSensors(const CommunicationDistanceSensors &sensors);

private:
    bool isMain;
    uint baudrate;
};

#endif