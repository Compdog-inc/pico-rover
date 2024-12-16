// Hardware headers
#include <pico/stdlib.h>
#include <hardware/spi.h>
#include <cstring>

#include "communication.h"

static constexpr uint COMM_SPI_SCK = 2;
static constexpr uint COMM_SPI_TX_MAIN = 3;
static constexpr uint COMM_SPI_RX_MAIN = 4;
static constexpr uint COMM_SPI_TX_SENSE = 4;
static constexpr uint COMM_SPI_RX_SENSE = 3;
static constexpr uint COMM_SPI_CSN = 5;

static constexpr uint COMM_SPI_BAUDRATE = 1 * 1000 * 1000;

Communication::Communication(bool isMain) : isMain(isMain)
{
    baudrate = spi_init(spi0, COMM_SPI_BAUDRATE);
    spi_set_slave(spi0, !isMain);

    gpio_set_function(COMM_SPI_SCK, GPIO_FUNC_SPI);
    gpio_set_function(COMM_SPI_TX_MAIN, GPIO_FUNC_SPI); /* MAIN <--> SENSE, same function */
    gpio_set_function(COMM_SPI_RX_MAIN, GPIO_FUNC_SPI); /* MAIN <--> SENSE, same function */
    gpio_set_function(COMM_SPI_CSN, GPIO_FUNC_SPI);
}

Communication::~Communication()
{
    spi_deinit(spi0);
    gpio_deinit(COMM_SPI_SCK);
    gpio_deinit(COMM_SPI_TX_MAIN); /* MAIN <--> SENSE, same function */
    gpio_deinit(COMM_SPI_RX_MAIN); /* MAIN <--> SENSE, same function */
    gpio_deinit(COMM_SPI_CSN);
}

bool Communication::readStatus(CommunicationStatus *out_status)
{
    uint8_t buffer[CommunicationStatus_SIZE + 1];
    if (spi_read_blocking(spi0, (uint8_t)CommunicationCommand::ReadStatus, buffer, CommunicationStatus_SIZE + 1) != CommunicationStatus_SIZE + 1)
    {
        return false;
    }

    *out_status = {
        .version = ((uint32_t)buffer[1]) | ((uint32_t)buffer[2] << 8) | ((uint32_t)buffer[3] << 16) | ((uint32_t)buffer[4] << 24),
        .running = buffer[5] != 0};

    return true;
}

bool Communication::readDistanceSensors(CommunicationDistanceSensors *out_sensors)
{
    uint8_t buffer[CommunicationDistanceSensors_SIZE + 1];
    if (spi_read_blocking(spi0, (uint8_t)CommunicationCommand::ReadDistanceSensors, buffer, CommunicationDistanceSensors_SIZE + 1) != CommunicationDistanceSensors_SIZE + 1)
    {
        return false;
    }

    *out_sensors = {};

    std::memcpy(&out_sensors->distance0, &buffer[1 + 0 * sizeof(float)], sizeof(float));
    std::memcpy(&out_sensors->distance1, &buffer[1 + 1 * sizeof(float)], sizeof(float));
    std::memcpy(&out_sensors->distance2, &buffer[1 + 2 * sizeof(float)], sizeof(float));
    std::memcpy(&out_sensors->distance3, &buffer[1 + 3 * sizeof(float)], sizeof(float));
    std::memcpy(&out_sensors->distance4, &buffer[1 + 4 * sizeof(float)], sizeof(float));
    std::memcpy(&out_sensors->distance5, &buffer[1 + 5 * sizeof(float)], sizeof(float));

    return true;
}

bool Communication::hasData()
{
    return spi_is_readable(spi0);
}

CommunicationCommand Communication::getCommand()
{
    uint8_t buffer[1];
    if (spi_read_blocking(spi0, 0, buffer, 1) != 1)
    {
        return CommunicationCommand::Null;
    }

    return (CommunicationCommand)buffer[0];
}

bool Communication::writeStatus(const CommunicationStatus &status)
{
    uint8_t buffer[CommunicationStatus_SIZE];
    buffer[0] = (uint8_t)(status.version & 0xFF);
    buffer[1] = (uint8_t)((status.version >> 8) & 0xFF);
    buffer[2] = (uint8_t)((status.version >> 16) & 0xFF);
    buffer[3] = (uint8_t)((status.version >> 24) & 0xFF);
    buffer[4] = status.running ? 0xFF : 0;

    return spi_write_blocking(spi0, buffer, CommunicationStatus_SIZE) == CommunicationStatus_SIZE;
}

bool Communication::writeDistanceSensors(const CommunicationDistanceSensors &sensors)
{
    uint8_t buffer[CommunicationDistanceSensors_SIZE];
    std::memcpy(&buffer[0 + 0 * sizeof(float)], &sensors.distance0, sizeof(float));
    std::memcpy(&buffer[0 + 1 * sizeof(float)], &sensors.distance1, sizeof(float));
    std::memcpy(&buffer[0 + 2 * sizeof(float)], &sensors.distance2, sizeof(float));
    std::memcpy(&buffer[0 + 3 * sizeof(float)], &sensors.distance3, sizeof(float));
    std::memcpy(&buffer[0 + 4 * sizeof(float)], &sensors.distance4, sizeof(float));
    std::memcpy(&buffer[0 + 5 * sizeof(float)], &sensors.distance5, sizeof(float));

    return spi_write_blocking(spi0, buffer, CommunicationDistanceSensors_SIZE) == CommunicationDistanceSensors_SIZE;
}