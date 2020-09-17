#pragma once

#include <random>
#include "ExternalTools/PCG/pcg_random.hpp"

class Random
{
public:
	static float		Randomf(float min, float max);
	static int			Randomi(int min, int max);
	static uint64_t		RandomGUID();

private:
	static pcg_extras::seed_seq_from<std::random_device> seed_source;
	static pcg32 rng;

	static std::uniform_real_distribution<double> guid;
};

