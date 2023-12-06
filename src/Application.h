#pragma once
#include <vector>
#include "OpenDMX.h"
class Application
{
private:
	std::vector<UCHAR[512]> cue_list;
	OpenDMX dmx_interface;

public:
	void Start();
	void Stop();
private:
	SetChannel()
};
