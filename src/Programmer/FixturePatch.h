#pragma once
#include <map>
#include "Fixture.h"

class FixturePatch
{
private:
	std::map<FixtureID, FixtureChannelRange> patch;

public:
	bool PatchFixture(Fixture& fixture);
	bool UnpatchFixture(FixtureID id);
};
