#pragma once
#include <string>

enum ColourComponents {
    RGB = 0,
    RBG,
    GRB,
    GBR,
    BRG,
    BGR,
    RGBW
};

struct Fixture
{
	std::string name;
	int channelStart;
	int channelsRequired;
};
