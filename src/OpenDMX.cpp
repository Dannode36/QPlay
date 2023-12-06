#include "OpenDMX.h"
OpenDMX::~OpenDMX()
{
    stop();
}

void OpenDMX::start()
{
    handle = 0;
    status = FT_Open(0, &handle);
    writeThread = std::make_unique<std::thread>([this] { writeData(); });
    setChannel(0, 0);  //Set DMX Start Code
}

void OpenDMX::stop() {
    if (writeThread.get() != nullptr && writeThread.get()->joinable()) {
        done = true;
        writeThread.get()->join();
        writeThread.release();
    }
}

void OpenDMX::blackout() {
    memset(buffer, 0, ARRAY_SIZE(buffer));
}

void OpenDMX::setChannel(int channel, UCHAR value)
{
    //C# TRANSLATION TODO: No null check required, maybe validate channel is in array bounds
    buffer[channel + 1] = value;
}

UCHAR OpenDMX::getChannel(int channel)
{
    return buffer[channel];
}

void OpenDMX::writeData()
{
    while (!done)
    {
        initOpenDMX(); //TODO: Does this need to be called every write?
        FT_SetBreakOn(handle);
        FT_SetBreakOff(handle);
        bytesWritten = write(handle, buffer, ARRAY_SIZE(buffer));
        Sleep(20);
    }
}

int OpenDMX::write(FT_HANDLE handle, UCHAR* data, int length)
{
    //C# TRANSLATION TODO: Copy of array to pointer occured here. Is a copy necessary or was it just for the pointer?
    ULONG bytesWritten = 0;
    status = FT_Write(handle, data, length, &bytesWritten);
    return (int)bytesWritten;
}

void OpenDMX::initOpenDMX()
{
    status = FT_ResetDevice(handle);
    status = FT_SetDivisor(handle, (char)12);  // set baud rate
    status = FT_SetDataCharacteristics(handle, FT_BITS_8, FT_STOP_BITS_2, FT_PARITY_NONE);
    status = FT_SetFlowControl(handle, FT_FLOW_NONE, 0, 0);
    status = FT_ClrRts(handle);
    status = FT_Purge(handle, FT_PURGE_TX);
    status = FT_Purge(handle, FT_PURGE_RX);
}