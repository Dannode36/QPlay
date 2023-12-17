#pragma once
#include "FTD2XX.h"
#include <thread>

#define ARRAY_SIZE(arr) sizeof(arr)/sizeof(arr[0])
constexpr auto DMX_REFRESH_RATE = 40; //Full universe refresh rate for an Enttec Open DMX USB
constexpr auto BUFFER_SIZE = 513; //Start code + 512 DMX channels
constexpr auto DMX_FIRST_CHANNEL = 1; //First DMX channel index in a DMX buffer

class OpenDMX
{
public:
    UCHAR buffer[BUFFER_SIZE] { 0 }; 
    UCHAR write_buffer[BUFFER_SIZE]{ 0 };
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
