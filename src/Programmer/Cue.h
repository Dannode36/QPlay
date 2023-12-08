#pragma once
#include <Windows.h>

struct Cue {
	float number;
	UCHAR buffer[512]{};

	Cue(float number, UCHAR* buffer, int bufferLength)
		: number(number)
	{
		memcpy(this->buffer, buffer, bufferLength);
	}
};
