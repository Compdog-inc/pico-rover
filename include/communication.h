#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

#include <pico/stdlib.h>

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

static constexpr uint Communication_DataSize = CommunicationStatus_SIZE + CommunicationDistanceSensors_SIZE;

enum class CommunicationCommand : uint8_t
{
    Null = 0x00,
    Debug = 0x01
};

struct CommunicationControl
{
    CommunicationCommand command;             // first byte is command
    uint8_t data[Communication_DataSize - 1]; // rest is host data
};

class Communication
{
public:
    Communication(bool isMain);
    ~Communication();

    bool hasData();
    bool read(const CommunicationControl &control, CommunicationStatus *out_status, CommunicationDistanceSensors *out_sensors);
    bool write(const CommunicationStatus &status, const CommunicationDistanceSensors &sensors, CommunicationControl *out_control);

private:
    bool isMain;
    uint baudrate;
};

#endif