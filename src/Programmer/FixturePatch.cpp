#include "FixturePatch.h"

bool FixturePatch::PatchFixture(Fixture& fixture)
{
	return patch.insert({ fixture.id, fixture.channels }).second;
}

bool FixturePatch::UnpatchFixture(FixtureID id)
{
	return patch.erase(id);
}
