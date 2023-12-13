#pragma once
#include <Windows.h>

struct Cue {
	float number;
	UCHAR buffer[BUFFER_SIZE]{};

	Cue(float number, UCHAR* buffer, int bufferLength)
		: number(number)
	{
		memcpy(this->buffer, buffer, bufferLength);
	}


	bool operator==(const Cue& other) const
	{
		return number == other.number;
	}
};
