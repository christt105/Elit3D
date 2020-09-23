#include "Random.h"

pcg_extras::seed_seq_from<std::random_device> Random::seed_source;
pcg32 Random::rng = pcg32(seed_source);
std::uniform_int_distribution<unsigned long long> Random::guid = std::uniform_int_distribution<unsigned long long>(1ULL, UINT64_MAX);

float Random::Randomf(float min, float max)
{
	std::uniform_real_distribution<float> rand(min, max);
	return rand(rng);
}

int Random::Randomi(int min, int max)
{
	std::uniform_int_distribution<int> rand(min, ++max);
	return rand(rng);
}

uint64_t Random::RandomGUID()
{
	return guid(rng);
}
