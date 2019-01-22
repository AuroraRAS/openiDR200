#include "idrusb.h"

#include <arpa/inet.h>

IDRUSB::IDRUSB()
{
    libusb_init(&libusb_ctx);
    handle = libusb_open_device_with_vid_pid(libusb_ctx, 0x0400, 0xc35a);
    if(!handle) {
        std::cout << "cannot open usb device!" << std::endl;
    }
}

IDRUSB::~IDRUSB()
{
    libusb_close(handle);
    libusb_exit(libusb_ctx);
}

void IDRUSB::putRequest(uint8_t cmd, uint8_t para, uint8_t *data, uint16_t dataSize)
{
    uint8_t buffer[0x10000];
    int len;

    IDRRequest request = (IDRRequest)buffer;
    memcpy(request->preamble, iDRPreamble, sizeof (iDRPreamble));
    request->len = htons(3 + dataSize);
    request->cmd = cmd;
    request->para = para;
    if(dataSize > 0)
        memcpy(request->data_and_chksum, data, dataSize);

    //xor chksum
    request->data_and_chksum[dataSize] = 0;
    for (uint32_t i = sizeof (request->preamble); i < ntohs(request->len)+sizeof (request->preamble)+1; ++i) {
        request->data_and_chksum[dataSize] ^= buffer[i];
    }

    libusb_bulk_transfer(handle, ep2o_addr, buffer, sizeof (request->preamble)+sizeof (request->len), &len, 200);
    libusb_bulk_transfer(handle, ep6o_addr, buffer+sizeof (request->preamble)+sizeof (request->len), ntohs(request->len), &len, 200);

    std::this_thread::sleep_for(std::chrono::milliseconds(400));
}

IDRResponse IDRUSB::getResponse()
{
    uint8_t buffer[0x10000];
    int len;
    IDRResponse result;
    static const uint headerLen = sizeof (result->preamble) + sizeof (result->len);

    libusb_bulk_transfer(handle, ep1i_addr, buffer, sizeof(buffer), &len, 200);
    libusb_bulk_transfer(handle, ep5i_addr, buffer+headerLen, sizeof(buffer)-headerLen, &len, 200);

    result = (IDRResponse)malloc(len+headerLen);
    memcpy(result, buffer, len+headerLen);
    return result;
}

bool IDRUSB::samStatus()
{
    putRequest(0x11, 0xff);
    IDRResponse response = getResponse();
    bool result = response->sw[2] == 0x90;
    free(response);
    return result;
}

bool IDRUSB::findCard()
{
    putRequest(0x20, 0x01);
    IDRResponse response = getResponse();
    bool result = response->sw[2] == 0x9f;
    free(response);
    return result;
}

bool IDRUSB::selectCard()
{
    putRequest(0x20, 0x02);
    IDRResponse response = getResponse();
    bool result = response->sw[2] == 0x90;
    free(response);
    return result;
}

bool IDRUSB::readCard()
{
    putRequest(0x30, 0x01);
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    IDRResponse response = getResponse();
    bool result = response->sw[2] == 0x90;

    for (int i = 0; i < ntohs(response->len); ++i)
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint32_t>(response->data_and_chksum[i]) << " ";
    std::cout << std::endl;

    free(response);
    return result;
}
