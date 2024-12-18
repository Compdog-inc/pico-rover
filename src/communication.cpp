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

bool Communication::hasData()
{
    return spi_is_readable(spi0);
}

bool Communication::read(const CommunicationControl &control, CommunicationStatus *out_status, CommunicationDistanceSensors *out_sensors)
{
    uint8_t controlBuf[Communication_DataSize];

    controlBuf[0] = (uint8_t)control.command;
    std::memcpy(&controlBuf[1], control.data, Communication_DataSize - 1);

    uint8_t buffer[Communication_DataSize];
    if (spi_write_read_blocking(spi0, controlBuf, buffer, Communication_DataSize) != Communication_DataSize)
    {
        return false;
    }

    *out_status = {
        .version = ((uint32_t)buffer[0]) | ((uint32_t)buffer[1] << 8) | ((uint32_t)buffer[2] << 16) | ((uint32_t)buffer[3] << 24),
        .running = buffer[4] != 0};

    *out_sensors = {};

    std::memcpy(&out_sensors->distance0, &buffer[5 + 0 * sizeof(float)], sizeof(float));
    std::memcpy(&out_sensors->distance1, &buffer[5 + 1 * sizeof(float)], sizeof(float));
    std::memcpy(&out_sensors->distance2, &buffer[5 + 2 * sizeof(float)], sizeof(float));
    std::memcpy(&out_sensors->distance3, &buffer[5 + 3 * sizeof(float)], sizeof(float));
    std::memcpy(&out_sensors->distance4, &buffer[5 + 4 * sizeof(float)], sizeof(float));
    std::memcpy(&out_sensors->distance5, &buffer[5 + 5 * sizeof(float)], sizeof(float));

    return true;
}

bool Communication::write(const CommunicationStatus &status, const CommunicationDistanceSensors &sensors, CommunicationControl *out_control)
{
    uint8_t buffer[Communication_DataSize];
    buffer[0] = (uint8_t)(status.version & 0xFF);
    buffer[1] = (uint8_t)((status.version >> 8) & 0xFF);
    buffer[2] = (uint8_t)((status.version >> 16) & 0xFF);
    buffer[3] = (uint8_t)((status.version >> 24) & 0xFF);
    buffer[4] = status.running ? 0xFF : 0;

    std::memcpy(&buffer[5 + 0 * sizeof(float)], &sensors.distance0, sizeof(float));
    std::memcpy(&buffer[5 + 1 * sizeof(float)], &sensors.distance1, sizeof(float));
    std::memcpy(&buffer[5 + 2 * sizeof(float)], &sensors.distance2, sizeof(float));
    std::memcpy(&buffer[5 + 3 * sizeof(float)], &sensors.distance3, sizeof(float));
    std::memcpy(&buffer[5 + 4 * sizeof(float)], &sensors.distance4, sizeof(float));
    std::memcpy(&buffer[5 + 5 * sizeof(float)], &sensors.distance5, sizeof(float));

    uint8_t controlBuf[Communication_DataSize];

    if (spi_write_read_blocking(spi0, buffer, controlBuf, Communication_DataSize) != Communication_DataSize)
    {
        return false;
    }

    *out_control = {};
    out_control->command = (CommunicationCommand)controlBuf[0];
    std::memcpy(out_control->data, &controlBuf[1], Communication_DataSize - 1);

    return true;
}