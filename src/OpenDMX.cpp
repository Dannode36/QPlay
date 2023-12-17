#include "OpenDMX.h"
#include <iostream>
OpenDMX::~OpenDMX()
{
    stop();
}

void OpenDMX::start() {
    if (!streaming) {
        handle = 0;
        status = FT_Open(0, &handle);
        if (FT_SUCCESS(status)) {
            connected = true;
            streaming = true;
            bufferResetWithValue(0);
            setChannel(0, 0);  //Set DMX Start Code
            writeThread = std::make_unique<std::thread>([this] { writeData(); });
        }
        else {
            FT_CyclePort(handle);
        }
    }
}

void OpenDMX::stop() {
    streaming = false;
    if (writeThread.get() != nullptr && writeThread.get()->joinable()) {
        writeThread.get()->join();
        writeThread.release();
    }
    status = FT_Close(handle);
    if (FT_SUCCESS(status)) {
        connected = false;
    }
}

void OpenDMX::bufferResetWithValue(UCHAR value) {
    memset(buffer, value, BUFFER_SIZE);
}

/// <summary>
/// Set DMX channel value in the buffer
/// </summary>
/// <param name="channel">A number in range 1-512</param>
/// <param name="value">The 8-bit value to assign to the channel</param>
void OpenDMX::setChannel(int channel, UCHAR value)
{
    //C# TRANSLATION TODO: No null check required, maybe validate channel is in array bounds
    if (channel < DMX_FIRST_CHANNEL || channel >= BUFFER_SIZE) { return; }
    buffer[channel] = value;
}

UCHAR OpenDMX::getChannel(int channel)
{
    return buffer[channel];
}

void OpenDMX::writeData()
{
    initOpenDMX(); //TODO: Does this need to be called every write?
    while (streaming)
    {
        FT_SetBreakOn(handle);
        FT_SetBreakOff(handle);
        for (int i = 0; i < ARRAY_SIZE(buffer); i++) {
            write_buffer[i] = buffer[i] * masterFaderPercent / 100;
        }

        bytesWritten = write(handle, write_buffer, ARRAY_SIZE(buffer));
        Sleep(25);
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