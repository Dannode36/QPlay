#pragma once
#include <string>

typedef int FixtureID;
typedef std::pair<int, int> FixtureChannelRange; //<Start Channel, Numeber of channels the fixture uses>

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
    FixtureID id;
	std::string name;
    FixtureChannelRange channels;
};
