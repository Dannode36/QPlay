#include "OpenDMX.h"
OpenDMX::~OpenDMX()
{
    stop();
}

void OpenDMX::start() {
    if (!streaming) {
        handle = 0;
        status = FT_Open(0, &handle);
        if (FT_SUCCESS(status)) {
            writeThread = std::make_unique<std::thread>([this] { writeData(); });
            setChannel(0, 0);  //Set DMX Start Code
            connected = true;
            streaming = true;
            bufferResetWithValue(0);
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
    memset(buffer, value, ARRAY_SIZE(buffer));
}

void OpenDMX::setChannel(int channel, UCHAR value)
{
    //C# TRANSLATION TODO: No null check required, maybe validate channel is in array bounds
    if (channel < DMX_FIRST_CHANNEL || channel >= DMX_CHANNELS) { return; }
    buffer[channel - 1] = value;
}

UCHAR OpenDMX::getChannel(int channel)
{
    return buffer[channel - 1];
}

void OpenDMX::writeData()
{
    initOpenDMX(); //TODO: Does this need to be called every write?
    FT_SetBreakOn(handle);
    FT_SetBreakOff(handle);

    while (streaming)
    {
        for (int i = 0; i < ARRAY_SIZE(buffer); i++) {
            write_buffer[i] = buffer[i] * masterFaderPercent / 100;
        }

        printf("C1: %d\n", write_buffer[0]);

        bytesWritten = write(handle, write_buffer, ARRAY_SIZE(buffer));
        Sleep(10);
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