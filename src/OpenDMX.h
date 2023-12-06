#pragma once
#include "FTD2XX.h"
#include <thread>

#define ARRAY_SIZE(arr) sizeof(arr)/sizeof(arr[0])

class OpenDMX
{

public:
    UCHAR buffer[512] { 0 };
    FT_HANDLE handle;
    bool done = false;
    int bytesWritten = 0;
    FT_STATUS status;
private:
    std::unique_ptr<std::thread> writeThread;

public:
    ~OpenDMX();
    void start();
    void stop();
    void blackout();
    void setChannel(int channel, UCHAR value);
    UCHAR getChannel(int channel);

private:
    void writeData();
    int write(FT_HANDLE handle, UCHAR* data, int length);
    void initOpenDMX();
};
