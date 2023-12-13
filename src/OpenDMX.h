#pragma once
#include "FTD2XX.h"
#include <thread>

#define ARRAY_SIZE(arr) sizeof(arr)/sizeof(arr[0])
constexpr auto DMX_CHANNELS = 513;
constexpr auto DMX_FIRST_CHANNEL = 1;

class OpenDMX
{

public:
    UCHAR buffer[DMX_CHANNELS] { 255 };
    UCHAR write_buffer[ARRAY_SIZE(buffer)]{ 0 };
    bool streaming{ false };
    bool connected{ false };
    FT_STATUS status;

    //FADER VARIABLES
    UCHAR masterFaderPercent{ 100 }; //0 - 100

private:
    FT_HANDLE handle;
    ULONG bytesWritten = 0;
    std::unique_ptr<std::thread> writeThread;

public:
    ~OpenDMX();
    void start();
    void stop();
    void bufferResetWithValue(UCHAR value);
    void setChannel(int channel, UCHAR value);
    UCHAR getChannel(int channel);

private:
    void writeData();
    int write(FT_HANDLE handle, UCHAR* data, int length);
    void initOpenDMX();
};
