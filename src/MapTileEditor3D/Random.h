#pragma once

#include <random>
#include "ExternalTools/PCG/pcg_random.hpp"

class Random
{
public:
	Random();
	~Random();

	float		Randomf(float min, float max);
	int			Randomi(int min, int max);
	uint64_t	RandomGUID();

private:
	pcg_extras::seed_seq_from<std::random_device> seed_source;
	pcg32 rng;

	std::uniform_real_distribution<double> guid;
};

