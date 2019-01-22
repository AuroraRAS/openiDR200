#ifndef IDRUSB_H
#define IDRUSB_H

#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstring>
#include <thread>

#include <libusb-1.0/libusb.h>

// headers
static constexpr uint8_t iDRPreamble[] = {
    0xaa, 0xaa, 0xaa, 0x96, 0x69
};

typedef struct iDRRequest {
    uint8_t preamble[sizeof (iDRPreamble)];
    uint16_t len;
    uint8_t cmd;
    uint8_t para;
    uint8_t data_and_chksum[1];
}__attribute__ ((packed)) * IDRRequest;

typedef struct iDRResponse {
    uint8_t preamble[sizeof (iDRPreamble)];
    uint16_t len;
    uint8_t sw[3];
    uint8_t data_and_chksum[1];
}__attribute__ ((packed)) * IDRResponse;

class IDRUSB
{
    // put request
    static const uint8_t ep2o_addr = 0x02;
    static const uint8_t ep6o_addr = 0x06;
    // get response
    static const uint8_t ep1i_addr = 0x81;
    static const uint8_t ep5i_addr = 0x85;

    libusb_context * libusb_ctx;
    libusb_device_handle * handle;

public:
    IDRUSB();
    ~IDRUSB();

private:
    void putRequest(uint8_t cmd, uint8_t para, uint8_t* data = nullptr, uint16_t dataSize = 0);
    IDRResponse getResponse();

public:
    bool samStatus();
    bool findCard();
    bool selectCard();
    bool readCard();
};

#endif // IDRUSB_H
