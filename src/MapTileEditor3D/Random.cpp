#include "Random.h"

#include "ExternalTools/mmgr/mmgr.h"

Random::Random()
{
	rng = pcg32(seed_source);
	guid = std::uniform_real_distribution<double>(1ULL, UINT64_MAX);
}

Random::~Random()
{
}

float Random::Randomf(float min, float max)
{
	std::uniform_real_distribution<float> rand(min, max);
	return rand(rng);
}

int Random::Randomi(int min, int max)
{
	std::uniform_real_distribution<double> rand(min, ++max);
	return (int)rand(rng);
}

uint64_t Random::RandomGUID()
{
	return (uint64_t)guid(rng);
}
